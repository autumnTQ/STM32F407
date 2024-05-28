#include "key.h"
//////////////////////////////////////////////////////////////////////////////////	 
//����������������	   
////////////////////////////////////////////////////////////////////////////////// 	 

//������ʼ������
void KEY_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;             //��������PF6
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;        
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;          //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    //�����ж�Դ
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);
    
    EXTI_InitTypeDef EXTI_InitStructure;                        //�����ж�ͨ��    
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;       //�����ش���   
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;     //�ж���
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;     //�ж���
    NVIC_Init(&NVIC_InitStructure);
} 





















