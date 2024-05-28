#ifndef __MODBUS_H
#define __MODBUS_H

/* 包含的头文件---------------------------------------------------------------*/
#include "Config.h"

extern uint8_t	sendCount;	//发送字节个数
extern uint8_t	receCount;	//接收到的字节个数
extern uint8_t	sendPosi;   //发送位置
extern uint8_t	localAddr ;	//单片机控制板的地址
extern uint8_t    UARTU1_RX_bit;	//UARTU接收数据查询标志位
extern uint8_t    UARTU1_TX_bit;	//UARTU发送数据查询标志位

extern uint32_t	dwTickCount,dwIntTick;	//时钟
extern uint8_t	sendBuf[256],receBuf[256]; //发送接收缓冲区
extern uint8_t	checkoutError;	// ==2 偶校验错  
extern uint16_t	receTimeOut;		//接收超时
extern uint8_t	c10ms;			//10ms 计时
extern uint8_t UART_addr;//modbus_rtu通讯地址
extern uint8_t save_parm_bit;//保存参数变量置1=保存参数
extern uint16_t baudRate_Selection;//波特率选择

extern void beginSend(void);//开始发送
extern void b485Send(uint8_t n);//使能485接收发送

void USART1_Init(uint16_t btl);
void checkComm0Modbus(void);
void readCoil(void);
void readRegisters(void);
void forceSingleCoil(void);
void presetSingleRegister(void);
void presetMultipleRegisters(void);
void forceMultipleCoils(void);
uint16_t getRegisterVal(uint16_t addr,uint16_t *tempData);
uint16_t setRegisterVal(uint16_t addr,uint16_t tempData);
uint16_t getCoilVal(uint16_t addr,uint16_t *tempData);
uint16_t setCoilVal(uint16_t addr,uint16_t tempData);


#endif 


