#ifndef __MODBUS_H
#define __MODBUS_H

/* ������ͷ�ļ�---------------------------------------------------------------*/
#include "Config.h"

extern uint8_t	sendCount;	//�����ֽڸ���
extern uint8_t	receCount;	//���յ����ֽڸ���
extern uint8_t	sendPosi;   //����λ��
extern uint8_t	localAddr ;	//��Ƭ�����ư�ĵ�ַ
extern uint8_t    UARTU1_RX_bit;	//UARTU�������ݲ�ѯ��־λ
extern uint8_t    UARTU1_TX_bit;	//UARTU�������ݲ�ѯ��־λ

extern uint32_t	dwTickCount,dwIntTick;	//ʱ��
extern uint8_t	sendBuf[256],receBuf[256]; //���ͽ��ջ�����
extern uint8_t	checkoutError;	// ==2 żУ���  
extern uint16_t	receTimeOut;		//���ճ�ʱ
extern uint8_t	c10ms;			//10ms ��ʱ
extern uint8_t UART_addr;//modbus_rtuͨѶ��ַ
extern uint8_t save_parm_bit;//�������������1=�������
extern uint16_t baudRate_Selection;//������ѡ��

extern void beginSend(void);//��ʼ����
extern void b485Send(uint8_t n);//ʹ��485���շ���

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


