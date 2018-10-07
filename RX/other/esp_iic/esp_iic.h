/*
Demo code for Relay Shield 
by Catalex
catalex.taobao.com
*/
//#define RELAY1 D7
//#define RELAY2 D6
//#define RELAY3 D5
//#define RELAY4 D4


#include<ESP8266WiFi.h>
#include<ESP8266WiFiMulti.h>
#include <time.h>
#define timezone 8

#include <Wire.h>
#include "Adafruit_INA219.h"
Adafruit_INA219 ina219_1(INA219_ADDRESS);


#include "Adafruit_NeoPixel.h"
#define PIN 6
#define MAX_LED 20
uint32_t color_array[MAX_LED];
Adafruit_NeoPixel strip = Adafruit_NeoPixel( MAX_LED, PIN, NEO_RGB + NEO_KHZ800 );


const char* ssid = "frye";  //Wifi����
const char* password = "52150337";  //Wifi����
unsigned long MS_TIMER = 0;
unsigned long timer,lasttime=0;
char *time_str;   
char H1,H2,M1,M2,S1,S2;




//#include <LCD4Bit_mod.h>
//LCD4Bit_mod lcd = LCD4Bit_mod(2);

void setup() 
{                
	//pinMode(RELAY1, OUTPUT);//set the pin to be OUTPUT pin.
	//pinMode(RELAY2, OUTPUT);//
	//pinMode(RELAY3, OUTPUT);//
	//pinMode(RELAY4, OUTPUT);//

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


	ina219_1.begin();


	// ��ʼ����
	//strip.begin();
	//// �������ݣ�Ĭ��ÿ�������ɫΪ0�����Գ�ʼ��ÿ���㶼�ǲ�����
	//strip.show();





}

void loop() 
{

	timer = millis();
	if ((timer - lasttime)>=1000){
		time_t now = time(nullptr); //��ȡ��ǰʱ��
		time_str = ctime(&now);
		H1 = time_str[11];
		H2 = time_str[12];
		M1 = time_str[14];
		M2 = time_str[15];
		S1 = time_str[17];
		S2 = time_str[18];
		Serial.printf("%c%c:%c%c:%c%c\n",H1,H2,M1,M2,S1,S2);
		Serial.printf(time_str);
		//DispTime();
		lasttime = timer;

		int Light = 100-(analogRead(A0)/11);
		Serial.printf("Light = %d \n",  Light);

		unsigned int Current = ina219_1.getCurrent_mA();
		Serial.printf("Current  =  %d mA \r\n",Current);


		unsigned int testvolt = ina219_1.getBusVoltage_V()*1000;
		Serial.printf("test volt  =  %d mv \r\n",testvolt);


		//// ���ɫ��ˮ
		//for(int i = MAX_LED; i > 0;i--)
		//{
		//	color_array[i] = color_array[i-1];
		//}

		//color_array[0] = strip.Color(random(20), random(20), random(20));

		//for(int i = 0; i < MAX_LED;i++)
		//{
		//	strip.setPixelColor(i, color_array[i]);
		//}

		//strip.show();

	}


}