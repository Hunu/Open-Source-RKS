




//D0 = GPIO16; 
//D1 = GPIO5; 
//D2 = GPIO4;	LED on esp8266
//D3 = GPIO0;can not download when connected to low
//D4 = GPIO2;	
//D5 = GPIO14;  
//D6 = GPIO12;
//D7 = GPIO13;
//D8 = GPIO15;  can not start when high input
//D9 = GPIO3; UART RX
//D10 = GPIO1; UART TX
//LED_BUILTIN = GPIO16 (auxiliary constant for the board LED, not a board pin);


#define RELAY				D0
#define DETECTOR			D1





#include<ESP8266WiFi.h>
#include<WiFiUdp.h>
#include<ESP8266mDNS.h>
#include<ArduinoOTA.h>
#include<ESP8266WiFiMulti.h>
#include<time.h>
#define timezone 8


const char* ssid = "frye";  //Wifi����
const char* password = "52150337";  //Wifi����
WiFiUDP m_WiFiUDP;


char *time_str;   
char H1,H2,M1,M2,S1,S2;


#include "Z:\bt\web\datastruct.h"
tCompressorData CompressorData;
unsigned char RoomIndex = 21;
bool FullReached = false;
unsigned long RunningCounter = 60;
unsigned long LastRunningFinishTenthSeconds;
unsigned long LastDetectorGotoIdelTenthSeconds;


unsigned long TenthSecondsSinceStart = 0;
void TenthSecondsSinceStartTask();
void OnTenthSecond();
void OnSecond();


void MyPrintf(const char *fmt, ...);


void setup() 
{       

	pinMode(RELAY, OUTPUT);//set the pin to be OUTPUT pin.
	digitalWrite(RELAY, LOW);
	pinMode(DETECTOR,INPUT_PULLUP);
	

	CompressorData.DataType = 4;
	CompressorData.isOn = false;


	delay(50);                      
	Serial.begin(115200);


	WiFi.disconnect();
	WiFi.mode(WIFI_STA);//����ģʽΪSTA
	Serial.print("Is connection routing, please wait");  
	WiFi.begin(ssid, password); //Wifi���뵽����
	Serial.println("\nConnecting to WiFi");
	//���Wifi״̬����WL_CONNECTED�����ʾ����ʧ��
	while (WiFi.status() != WL_CONNECTED) {  
		Serial.print("."); 
		delay(1000);    //��ʱ�ȴ���������
	}



	//����ʱ���ʽ�Լ�ʱ�����������ַ
	configTime(timezone * 3600, 0, "pool.ntp.org", "time.nist.gov");
	Serial.println("\nWaiting for time");
	while (!time(nullptr)) {
		Serial.print(".");
		delay(1000);    
	}
	Serial.println("");




	byte mac[6];
	WiFi.softAPmacAddress(mac);
	//printf("macAddress 0x%02X:0x%02X:0x%02X:0x%02X:0x%02X:0x%02X\r\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	MyPrintf("macAddress 0x%02X:0x%02X:0x%02X:0x%02X:0x%02X:0x%02X\r\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	//for (byte i=0;i<6;i++)
	//{
	//	RoomData.Mac[i] = mac[i];
	//}

	//for (unsigned char i = 0;i<ROOM_NUMBER;i++)
	//{
	//	if (memcmp(&RoomData.Mac[0],&RoomMacAddress[i][0],sizeof(unsigned long)*6) == 0)
	//	{
	//		MyPrintf("room ID=%d \r\n",i);
	//		RoomIndex = i;
	//		break;
	//	}
	//}


	m_WiFiUDP.begin(5050); 

	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		} else { // U_SPIFFS
			type = "filesystem";
		}

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) {
			Serial.println("Auth Failed");
		} else if (error == OTA_BEGIN_ERROR) {
			Serial.println("Begin Failed");
		} else if (error == OTA_CONNECT_ERROR) {
			Serial.println("Connect Failed");
		} else if (error == OTA_RECEIVE_ERROR) {
			Serial.println("Receive Failed");
		} else if (error == OTA_END_ERROR) {
			Serial.println("End Failed");
		}
	});
	ArduinoOTA.begin();
	Serial.println("Ready");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());


}

void loop() 
{
	ArduinoOTA.handle();

	TenthSecondsSinceStartTask();


	//m_WiFiUDP.parsePacket(); 
	//unsigned int UdpAvailable = m_WiFiUDP.available();
	//if (UdpAvailable == sizeof(tCompressorCommand))
	//{
	//	//MyPrintf(" m_WiFiUDP.available() = %d\r\n",UdpAvailable);
	//	tCompressorCommand tempCompressorCommand;
	//	m_WiFiUDP.read((char *)&tempCompressorCommand,sizeof(tRoomCommand));

	//	if (tempCompressorCommand.Triger == true)
	//	{
	//		RunningCounter = 60;
	//		MyPrintf("get Compressor trig from control\r\n");
	//	}
	//}
}


unsigned long LastMillis = 0;
void TenthSecondsSinceStartTask()
{
	unsigned long CurrentMillis = millis();
	if (abs(CurrentMillis - LastMillis) > 100)
	{
		LastMillis = CurrentMillis;
		TenthSecondsSinceStart++;
		OnTenthSecond();
		//MyPrintf("TenthSecondsSinceStart = %d \r\n",TenthSecondsSinceStart);
		//MyPrintf("TenthSecondsSinceStart = %d \r\n",TenthSecondsSinceStart);
	}
}

void OnSecond()
{
	time_t now = time(nullptr); //��ȡ��ǰʱ��
	time_str = ctime(&now);
	H1 = time_str[11];
	H2 = time_str[12];
	M1 = time_str[14];
	M2 = time_str[15];
	S1 = time_str[17];
	S2 = time_str[18];
	//printf("%c%c:%c%c:%c%c\n",H1,H2,M1,M2,S1,S2);
	//Serial.printf(time_str);

	struct   tm     *timenow;
	timenow   =   localtime(&now);
	unsigned char Hour = timenow->tm_hour;
	unsigned char Minute = timenow->tm_min;


	static bool LastDetectorState;
	static unsigned long LastDetectorChangeTenthSeconds;
	bool ThisDetectorState;


	ThisDetectorState = digitalRead(DETECTOR);

	if (ThisDetectorState != LastDetectorState)
	{

		//MyPrintf("DetectorState goto %d    %d   .\r\n",ThisDetectorState,(TenthSecondsSinceStart-LastDetectorChangeTenthSeconds)/10);
		if(!ThisDetectorState)
		{
			LastDetectorGotoIdelTenthSeconds = TenthSecondsSinceStart;
		}
		else
		{
			LastDetectorGotoIdelTenthSeconds = 0;
		}
		LastDetectorChangeTenthSeconds = TenthSecondsSinceStart;
		LastDetectorState = ThisDetectorState;
	}

	//if (now%10 == 0)
	//{
	//	CompressorData.isOn = true;
	//}

	//if (now%10 == 5)
	//{
	//	CompressorData.isOn = false;
	//}

	//if (now%300 == 0)
	//{
	//	RunningCounter = 60;
	//	MyPrintf("RunningCounter = 60 on 300s\r\n");
	//}


	if ((TenthSecondsSinceStart - LastRunningFinishTenthSeconds > 6000)//in 0.1s
		&&(RunningCounter == 0)
		&&(Hour > 5)
		&&(Hour < 22))
	{
		RunningCounter = 60;
		MyPrintf("RunningCounter = 60 after last running 10mins\r\n");
	}



	if ((RunningCounter > 0)&&(true))
	{
		if ((LastDetectorGotoIdelTenthSeconds!=0)&&(TenthSecondsSinceStart - LastDetectorGotoIdelTenthSeconds)>600)
		{
			RunningCounter--;
			if (RunningCounter == 0)
			{
				if (CompressorData.isOn)
				{
					LastRunningFinishTenthSeconds = TenthSecondsSinceStart;
					CompressorData.isOn = false;
					MyPrintf("CompressorData.isOn = false. when timeout\r\n");
				}
			} 
			else
			{
							if (!CompressorData.isOn)
			{
				CompressorData.isOn = true;
				MyPrintf("CompressorData.isOn = true\r\n");
			}
			}

		}
		else
		{
			if (CompressorData.isOn)
			{
				CompressorData.isOn = false;
				MyPrintf("CompressorData.isOn = false. when Detected\r\n");
			}
		}
	}
	else
	{

	}


	//	//printf("Hour = %d   Minute = %d  \r\n",Hour,Minute);

	//	if ((Hour > 5)&&(Hour < 22))
	//	{
	//		if (Minute == 0)
	//		{
	//			CompressorData.isOn = true;
	//		}
	//		if (Minute == 1)
	//		{
	//			CompressorData.isOn = false;
	//		}

	//		//if (Minute == 59)
	//		//{
	//		//	MyPrintf("Pressure = %d mBar \r\n",CompressorData.Pressure);
	//		//}

	//		//if (Minute == 2)
	//		//{
	//		//	MyPrintf("Pressure = %d mBar \r\n",CompressorData.Pressure);
	//		//}
	//	}

	//}


	//unsigned long AnalogValue= analogRead(A0);
	////printf("AnalogValue = %d  \r\n",AnalogValue);
	////printf("Volt = %d  \r\n",AnalogValue*3*1000/1024);
	//CompressorData.Pressure = abs(AnalogValue*3*1000/1024-425)/4*10;
	////printf("Pressure = %d mBar \r\n",CompressorData.Pressure);




	//if ((CompressorData.Pressure>5500)&&(!FullReached))
	//{
	//	MyPrintf("Reached 5.5 Bar \r\n");
	//	FullReached = true;
	//}


	//if ((CompressorData.Pressure<4000)&&(FullReached))
	//{
	//	if ((Hour > 5)&&(Hour < 22))
	//	{
	//		MyPrintf("In day time, running for 60s \r\n");
	//		RunningCounter = 60;
	//		FullReached = false;
	//	}
	//}

	//if (RunningCounter > 0)
	//{
	//	CompressorData.isOn = true;
	//	RunningCounter--;
	//	if (RunningCounter == 0)
	//	{
	//		MyPrintf("Running for 60s finished \r\n");
	//		CompressorData.isOn = false;
	//	}
	//}


	if (CompressorData.isOn)
	{
		digitalWrite(RELAY,HIGH);
	}
	else
	{
		digitalWrite(RELAY,LOW);
	}
	


	//m_WiFiUDP.beginPacket("192.168.0.17", 5050);
	//m_WiFiUDP.write((const char*)&CompressorData, sizeof(tCompressorData));
	//m_WiFiUDP.endPacket(); 

}

void OnTenthSecond()
{









	if (TenthSecondsSinceStart%10 == 0)
	{
		OnSecond();
	}
	

}



void MyPrintf(const char *fmt, ...)
{


	static char send_buf[1024];
	char * sprint_buf = send_buf+sizeof(tDebugData);
	tDebugData * pDebugData = (tDebugData*)send_buf;

	int n;
	va_list args;

	va_start(args,fmt);
	n = vsprintf(sprint_buf, fmt, args);
	va_end(args);

	printf(sprint_buf);

	pDebugData->DataType = 3;
	pDebugData->RoomId = RoomIndex;
	pDebugData->Length = n;

	m_WiFiUDP.beginPacket("192.168.0.17", 5050);
	m_WiFiUDP.write((const char*)send_buf, sizeof(tDebugData)+n);
	m_WiFiUDP.endPacket(); 


}
