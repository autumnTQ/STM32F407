/*
 * Copyright (c) 2006-2100, Tommy Guo
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-27     Tommy        first version
 */
 
#include "tim.h"

/**
 * @brief  TIM7定时器初始化
 * @param  None
 * @retval None
 */
void tim7_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    TIM_DeInit(TIM7);
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearFlag(TIM7, TIM_FLAG_Update);
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM7, ENABLE);
}

/**
 * @brief  1ms运行1次
 * @param  None
 * @retval None
 */
static void per_1ms_run(void)
{
    /* modbus接收定时 */

    mbslave_timer(&mb_usart3_t);
}

/**
 * @brief  500ms运行1次
 * @param  None
 * @retval None
 */
static void per_500ms_run(void)
{

    /* 独立看门狗喂狗标志置位 */
    iwdg_feed_flg = 1;
}

/**
 * @brief  TIM7中断服务程序
 * @param  None
 * @retval None
 */
void TIM7_IRQHandler(void)
{
    static uint16_t ms500 = 0;

    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM7, TIM_FLAG_Update);
        /* 用于调用周期性循环函数 */
        per_1ms_run();
        if (++ms500 >= 2000)   //2秒后。喂狗
        {
            per_500ms_run();
            ms500 = 0;
        }
    }
}
/*****END OF FILE*****/
