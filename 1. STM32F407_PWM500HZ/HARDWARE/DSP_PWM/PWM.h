#ifndef __PWM_H
#define __PWM_H

#include "stm32f4xx.h"                  // Device header

void PWM_Init(uint32_t arr, uint16_t psc, uint32_t ccr);                      //使用TIM2通道2输出PWM波
void PWM_SetCompare1(uint16_t compare);   //修改占空比   修改比较寄存器CCR的值 
   



#endif 


