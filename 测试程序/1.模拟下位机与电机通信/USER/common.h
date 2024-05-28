#ifndef __COMMON_H
#define __COMMON_H	
#include "sys.h"

#define bool    _Bool
#define true    1
#define false   0
	
//������
union HEXINT
{
	int num;
	unsigned char bnum[4];
};

union HEXSHORT
{
    unsigned short num;
    unsigned char bnum[2];
};

union HEXFLOAT
{
	float num;
	unsigned char bnum[4];
};

typedef enum
{
	PC_Type = 0x11,
    Motor_TimerTimeout = 0x12,
    Motor_TimerTimeout_1 = 0x13,  
    Push_TimerTimeout = 0x14, //�Ƹ˶�ʱ������־

}DeviceType;

typedef enum     //�����ʱ������־
{
	Motor_Rx = 0x12,              //��ѯ���״̬�������ݱ�־
	MotorConfigure_Rx = 0x13,     //��ѯ������÷������ݱ�־

}MotorOperateType;

typedef enum       
{
	Grip_TimerTimeout = 0x21, //��צ��ʱ������־
	Grip_Rx = 0x22,	
	Push_Rx = 0x32,	
    Sensor_Type = 0x41,       //��Ӵ���������־
	Temp_Type = 0x51,    //�¶ȿ��ư����־
    LevelDet = 0x61,
}MyOperateType;    


typedef enum PROTOCOLTYPE
{
	/***Э������************/
	ProtocolType_1,
	ProtocolType_2,
}ProtocolType;


//״̬���ı�־λ
typedef enum 
{
    No_State = 0,
	Motor_State,
	Motor_State_1,
	Push_State
}TIMER_STATE;


 typedef struct {
        uint8_t Tmp;
		uint8_t Flag; 
		uint8_t Fsm; 
		uint8_t RxCnt;   
		uint8_t RxLen;
		uint8_t RxBuf[100]; 
}UART_T;
 

unsigned char SerialSendData_PC(unsigned char *fixProtocol, int dataLength);


unsigned char SerialSendData(unsigned char *fixProtocol, char * data, int dataLength);
bool SerialSendData2Motor(ProtocolType type, unsigned char *fixProtocol, char * data, int dataLength);
bool  SerialSendData2Grip(ProtocolType type, unsigned char *fixProtocol);
bool  SerialSendData2Push(ProtocolType type, unsigned char *fixProtocol, char * data, int dataLength);
bool  SerialSendData2Temp(ProtocolType type, unsigned char *fixProtocol, char * data, int dataLength);
bool SerialSendData2Pc(ProtocolType type, unsigned char *fixProtocol, char * data, int dataLength);
unsigned short CrcVerifice(unsigned char * pStr, unsigned int iLen);
#endif
