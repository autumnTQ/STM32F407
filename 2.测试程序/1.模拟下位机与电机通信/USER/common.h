#ifndef __COMMON_H
#define __COMMON_H	
#include "sys.h"

#define bool    _Bool
#define true    1
#define false   0
	
//共用体
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
    Push_TimerTimeout = 0x14, //推杆定时器检测标志

}DeviceType;

typedef enum     //电机定时器检测标志
{
	Motor_Rx = 0x12,              //查询电机状态反馈数据标志
	MotorConfigure_Rx = 0x13,     //查询电机配置反馈数据标志

}MotorOperateType;

typedef enum       
{
	Grip_TimerTimeout = 0x21, //夹爪定时器检测标志
	Grip_Rx = 0x22,	
	Push_Rx = 0x32,	
    Sensor_Type = 0x41,       //外接传感器检测标志
	Temp_Type = 0x51,    //温度控制板检测标志
    LevelDet = 0x61,
}MyOperateType;    


typedef enum PROTOCOLTYPE
{
	/***协议类型************/
	ProtocolType_1,
	ProtocolType_2,
}ProtocolType;


//状态机的标志位
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
