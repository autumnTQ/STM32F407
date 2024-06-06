#include "bsp_timer.h"


/*******************************************************************************
 * @函数名称
 * @函数说明   主函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void TIM2_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_Period = 100 - 1; /* 自动重装载寄存器周期的值(计数值)    10MS*/
  TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* 向上计数模式 */
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM2, TIM_FLAG_Update); /* 清除溢出中断标志 */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
   
  TIM_Cmd(TIM2, ENABLE); /* 开启时钟 */
}

/*******************************************************************************
 * @函数名称
 * @函数说明   主函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void TIM3_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_Period = 100 - 1; /* 自动重装载寄存器周期的值(计数值) */
  TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* 向上计数模式 */
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM3, TIM_FLAG_Update); /* 清除溢出中断标志 */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
   
  TIM_Cmd(TIM3, ENABLE); /* 开启时钟 */
}

void NVIC_cfg(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);           // 先占优先级4位
                                                              ////////////定时器 3 中断//////////
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;           // 选择ADC
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占式优先级设置0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 响应式中断优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断
  NVIC_Init(&NVIC_InitStructure);                           // 初始化外设中断


  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;           // 选择ADC
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占式优先级设置0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 响应式中断优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断
  NVIC_Init(&NVIC_InitStructure);                           // 初始化外设中断

}




