/**
  ******************************************************************************
  * @file    i2c.h
  * @author  ���ĳ� 
	* @qq      269426626
  * @version V1.0
  * @date    2016.8.21
  * @note    �˳���ΪARM��IIC�ĵײ���
  ******************************************************************************
  */

#ifndef __CH_LIB_IIC_H__
#define __CH_LIB_IIC_H__
														
#ifdef __cplusplus
 extern "C" {
#endif
	
#include "sys.h"
#include "delay.h"
//IO��������

//#define SDA_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=8<<4;}
//#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=3<<4;}

#define SDA_IN()  {GPIOE->CRL&=0XFF0FFFFF;GPIOE->CRL|=8<<20;}
#define SDA_OUT() {GPIOE->CRL&=0XFF0FFFFF;GPIOE->CRL|=3<<20;}
//IO��������	 
//#define IIC_SCL    PBout(8) //SCL
//#define IIC_SDA    PBout(9) //SDA	 
//#define READ_SDA   PBin(9)  //����SDA 

#define IIC_SCL    PEout(4) //SCL
#define IIC_SDA    PEout(5) //SDA	 
#define READ_SDA   PEin(5)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

#endif
