#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "common.h"

void TIM3_Int_Init(uint16_t arr,uint16_t psc);
void TIM5_Int_Init(uint16_t arr,uint16_t psc);
void TIM9_Int_Init(uint16_t arr,uint16_t psc);  
void TIM_Switch(TIM_TypeDef* TIMx, uint32_t TIM_Period, FunctionalState NewState);
uint16_t Get_TimerState(TIM_TypeDef* TIMx);
extern volatile unsigned long long FreeRTOSRunTimeTicks;
void ConfigureTimeForRunTimeStats(void);
#endif
