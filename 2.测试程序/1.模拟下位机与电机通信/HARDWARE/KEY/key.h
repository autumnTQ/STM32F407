#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
#include "timer.h"
//����������������	   
////////////////////////////////////////////////////////////////////////////////// 	 
#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)	//PE0
#define KEY2 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1) //PE1
#define KEY1_PRES	1
#define KEY2_PRES	2

void KEY_Init(void);	//IO��ʼ��

#endif

