/*
 * Copyright (c) 2006-2100, Tommy Guo
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-27     Tommy        first version
 */

#include "wdg.h"

/* �������Ź�ι����־ */
uint8_t iwdg_feed_flg = 0;
/* ����WWDG������������ֵ��Ĭ��Ϊ��� */
static uint8_t wwdg_cnt = 0x7f;

/**
 * @brief  �������Ź���ʼ�����������Ź�ʱ��Ϊ40KHz�����ʱֵΪ0xFFF(4095)
 * @param  prer��Ԥ��Ƶֵ
 * @param  rlr����װ��ֵ
 * @retval None
 * @eg     ��Ƶ��(4~256)��64����С��ʱʱ��(ms)��64/40KHz=1.6�����ʱʱ��(ms)��1.6*4095=6553.6
 */
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    /* ���ϵͳ�Ƿ�Ӷ������Ź���λ�лָ� */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        RCC_ClearFlag();
    }
    else
    {
        /* ��־û������ */
    }

    /* д��0x5555��ʾ�������PR��RLR�Ĵ��� */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(prer);
    IWDG_SetReload(rlr);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

/**
 * @brief  �������Ź�ι��
 * @param  None
 * @retval None
 */
void iwdg_feed(void)
{
    IWDG_ReloadCounter();
}

/**
 * @brief  ���ڿ��Ź���ʼ�������ڿ��Ź�ʱ��Ϊ72MHz������ֵ��ΧΪ64~127
 * @param  tr��������ֵ
 * @param  wr������ֵ
 * @param  prer����Ƶϵ�� 
 * @retval None
 * @eg     ��Ƶ��(1~8)��8����С��ʱʱ��(ms)��8*4096/72MHz=0.4551�����ʱʱ��(ms)��0.4551*64=29.127
 * @eg     wwdgInit(127, 80, WWDG_Prescaler_8); ����ˢ��ʱ��0.4551*(127-80)=21.4ms
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
 * @brief  ���ڿ��Ź��жϷ����������Է�ϵͳȥ��ǰ������������洢��Ҫ���ݵ�
 * @param  None
 * @retval None
 */
void WWDG_IRQHandler(void)
{
    /* ���ڿ��Ź�ι������ʽ�ò�Ҫ�������� */
    WWDG_SetCounter(wwdg_cnt);
    WWDG_ClearFlag();
}
/*****END OF FILE*****/
