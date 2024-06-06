#include "bsp_timer.h"


/*******************************************************************************
 * @��������
 * @����˵��   ������
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void TIM2_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_Period = 100 - 1; /* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ)    10MS*/
  TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* ���ϼ���ģʽ */
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM2, TIM_FLAG_Update); /* �������жϱ�־ */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
   
  TIM_Cmd(TIM2, ENABLE); /* ����ʱ�� */
}

/*******************************************************************************
 * @��������
 * @����˵��   ������
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void TIM3_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_Period = 100 - 1; /* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
  TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* ���ϼ���ģʽ */
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM3, TIM_FLAG_Update); /* �������жϱ�־ */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
   
  TIM_Cmd(TIM3, ENABLE); /* ����ʱ�� */
}

void NVIC_cfg(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);           // ��ռ���ȼ�4λ
                                                              ////////////��ʱ�� 3 �ж�//////////
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;           // ѡ��ADC
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռʽ���ȼ�����0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // ��Ӧʽ�ж����ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ʹ���ж�
  NVIC_Init(&NVIC_InitStructure);                           // ��ʼ�������ж�


  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;           // ѡ��ADC
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ��ռʽ���ȼ�����0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // ��Ӧʽ�ж����ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ʹ���ж�
  NVIC_Init(&NVIC_InitStructure);                           // ��ʼ�������ж�

}




