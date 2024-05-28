#include "timer.h"
#include "led.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////
// 定时器 驱动代码
// 版本：V1.0
//////////////////////////////////////////////////////////////////////////////////
//
uint32_t FreeRTOSRunTimeTicks = 0;    /* FreeRTOS 时间统计所用的节拍计数器 */
//时基定时器的初始化
void ConfigureTimeForRunTimeStats(void)
{
    TIM3_Int_Init(10-1, 84-1);   // 10us  100倍的系统时钟节拍，系统时钟1ms=1000us
    FreeRTOSRunTimeTicks = 0;
}

// 通用定时器3中断初始化
// arr：自动重装值。
// psc：时钟预分频数
// 定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=定时器工作频率,单位:Mhz
// 这里使用的是定时器3!
void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); /// 使能TIM3时钟

	TIM_TimeBaseInitStructure.TIM_Period = arr;						// 自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;					// 定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //分频
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             // 初始化TIM3

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // 允许定时器3更新中断
	TIM_Cmd(TIM3, ENABLE);					   // 使能定时器3
    TIM_ClearFlag(TIM3, TIM_IT_Update);        //清除标志位

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;			  // 定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; // 抢占优先级5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) 
	{
        FreeRTOSRunTimeTicks++;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  
}
