/*
 * Copyright (c) 2006-2100, Tommy Guo
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-27     Tommy        first version
 */

#include "wdg.h"

/* 独立看门狗喂狗标志 */
uint8_t iwdg_feed_flg = 0;
/* 保存WWDG计数器的设置值，默认为最大 */
static uint8_t wwdg_cnt = 0x7f;

/**
 * @brief  独立看门狗初始化，独立看门狗时钟为40KHz，最大超时值为0xFFF(4095)
 * @param  prer：预分频值
 * @param  rlr：重装载值
 * @retval None
 * @eg     分频数(4~256)：64，最小超时时间(ms)：64/40KHz=1.6，最大超时时间(ms)：1.6*4095=6553.6
 */
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    /* 检测系统是否从独立看门狗复位中恢复 */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        RCC_ClearFlag();
    }
    else
    {
        /* 标志没有设置 */
    }

    /* 写入0x5555表示允许访问PR和RLR寄存器 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(prer);
    IWDG_SetReload(rlr);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

/**
 * @brief  独立看门狗喂狗
 * @param  None
 * @retval None
 */
void iwdg_feed(void)
{
    IWDG_ReloadCounter();
}

/**
 * @brief  窗口看门狗初始化，窗口看门狗时钟为72MHz，窗口值范围为64~127
 * @param  tr：计数器值
 * @param  wr：窗口值
 * @param  prer：分频系数 
 * @retval None
 * @eg     分频数(1~8)：8，最小超时时间(ms)：8*4096/72MHz=0.4551，最大超时时间(ms)：0.4551*64=29.127
 * @eg     wwdgInit(127, 80, WWDG_Prescaler_8); 窗口刷新时间0.4551*(127-80)=21.4ms
 */
void wwdg_init(uint8_t tr, uint8_t wr, uint32_t prer)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE);
    wwdg_cnt = tr & wwdg_cnt;
    WWDG_SetPrescaler(prer);
    WWDG_SetWindowValue(wr);
    WWDG_Enable(wwdg_cnt);
    WWDG_ClearFlag();

    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE; 
    NVIC_Init(&NVIC_InitStructure);

    WWDG_EnableIT();
}

/**
 * @brief  窗口看门狗中断服务函数，可以放系统去世前的遗嘱，比如存储重要数据等
 * @param  None
 * @retval None
 */
void WWDG_IRQHandler(void)
{
    /* 窗口看门狗喂狗，正式用不要放在这里 */
    WWDG_SetCounter(wwdg_cnt);
    WWDG_ClearFlag();
}
/*****END OF FILE*****/
