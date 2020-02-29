

//ESP8266�����󣬶�ȡ�̵���״̬ �� ��������־��
//������ʱ�Ļ����Դ�Ϊ׼���������Ļ����ӷ�������ȡ״̬��



struct tIicCommand
{
	//byte OperationCode;

	//0-8 RelayState
	byte RelayState;

	//0-8 RelayNeedSet
	byte RelayNeedSet;
	byte Rsv;
	byte Rsv2;
	unsigned long Sum;
};


struct tIicData
{
	//0-4 Relay 
	//5:Start more than 30s 
	byte RelayState;

	//0-8 TagOnline
	byte TagState;
	byte Rsv;
	byte Rsv2;
	unsigned long Sum;
};

#define crc_mul 0x1021  //���ɶ���ʽ
unsigned long cal_crc(unsigned char *ptr, unsigned char len)
{
	unsigned char i;
	unsigned long crc=0;
	while(len-- != 0)
	{
		for(i=0x80; i!=0; i>>=1)
		{
			if((crc&0x8000)!=0)
			{
				crc<<=1;
				crc^=(crc_mul);
			}else{
				crc<<=1;
			}
			if((*ptr&i)!=0)
			{
				crc ^= (crc_mul);
			}
		}
		ptr ++;
	}
	return (crc);
}
