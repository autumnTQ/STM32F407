#ifndef __PWM_H
#define __PWM_H

#include "stm32f4xx.h"                  // Device header

void PWM_Init(uint32_t arr, uint16_t psc, uint32_t ccr);                      //ʹ��TIM2ͨ��2���PWM��
void PWM_SetCompare1(uint16_t compare);   //�޸�ռ�ձ�   �޸ıȽϼĴ���CCR��ֵ 
   



#endif 


