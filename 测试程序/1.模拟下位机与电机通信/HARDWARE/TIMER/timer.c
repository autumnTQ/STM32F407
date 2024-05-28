#include "timer.h"
#include "led.h"
#include "malloc.h"
#include "usart.h"
#include "string.h"

// ���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h" //FreeRTOSʹ��
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//��ʱ��ʹ������          APB1-(84MHz): 
//TIM3  ���ȼ� 1   ------->ʹ��ΪFreeRTOS��ʱ��ͳ���ṩʱ�� 
//TIM5  ���ȼ� 10  ------->���ڼ������ʱ���У���ʱ10sֹͣ�������
//
//                        APB2-(168MHz):
//TIM9   ���ȼ� 7  ------->���ڶ�ʱ��ѯ�����״̬ 100ms

static uint8_t timer5_num = 0;      	// ��¼��ʱ��5���жϴ���
extern QueueHandle_t Motor_Message_Queue;   //�����Ϣ���о��
extern QueueHandle_t Message_Queue;		    // ��Ϣ���о��

extern TIMER_STATE Timer9_State;

//FreeRTOSʱ��ͳ�����õĽ��ļ�����
volatile unsigned long long FreeRTOSRunTimeTicks;

//��ʼ��TIM3ʹ��ΪFreeRTOS��ʱ��ͳ���ṩʱ��
void ConfigureTimeForRunTimeStats(void)
{
	//��ʱ��3��ʼ������ʱ��ʱ��Ϊ84M����Ƶϵ��Ϊ84-1�����Զ�ʱ��3��Ƶ��
	//Ϊ84M/84=1M���Զ���װ��Ϊ50-1����ô��ʱ�����ھ���50us
	FreeRTOSRunTimeTicks=0;
	TIM3_Int_Init(50-1,84-1);	//��ʼ��TIM3
}

//ͨ�ö�ʱ��ʹ��
void TIM_Switch(TIM_TypeDef* TIMx, uint32_t Period, FunctionalState NewState)
{
	TIMx->CNT = 0;	//����ʱ������
    TIMx->ARR = Period;   
	TIM_Cmd(TIMx,NewState);
}

//��ȡ��ʱ���Ŀ����رյ�״̬    1Ϊ���У� 0Ϊֹͣ
uint16_t Get_TimerState(TIM_TypeDef* TIMx)
{
    uint16_t state = TIMx->CR1 & TIM_CR1_CEN;   
    return state;    
}


/*************************************************************************************************************/
//ͨ�ö�ʱ��3�жϳ�ʼ��
void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;    //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;               //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;  //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;         //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM5_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);	           //ʹ��TIM5ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 		           //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler= psc;  		           //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode= TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision= TIM_CKD_DIV1; 	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	//��ʼ��TIM25
	
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);             //ʹ�ܶ�ʱ��5�����ж�
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);             //��������жϱ�־λ

	TIM_Cmd(TIM5, DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel= TIM5_IRQn; 		        //��ʱ��5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 10; 	//��ռ���ȼ�10
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0x00;        //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

//ͨ�ö�ʱ��9�жϳ�ʼ�� ��ѯ���״̬
void TIM9_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);	//ʹ��TIM9ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 		//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  		//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStructure);	//��ʼ��TIM9	
	
	TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);      //ʹ�ܶ�ʱ��1�����ж�
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);      //��������жϱ�־λ

	TIM_Cmd(TIM9, DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; 		//��ʱ��9�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7; 	//��ռ���ȼ�8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;         //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update)==SET)
	{
		FreeRTOSRunTimeTicks++;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  
}


//��ʱ��5�жϺ���   ��ʱ�رյ������ 
void TIM5_IRQHandler(void)
{   
	if(TIM_GetITStatus(TIM5,TIM_IT_Update) == SET)
	{
        timer5_num++;
        
        if(timer5_num == 5)
        {
            timer5_num = 0;
            GPIO_ResetBits(GPIOA, GPIO_Pin_12);
            TIM_Switch(TIM5, 10000-1, DISABLE);
        }
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  
}

//��ʱ��9�жϷ�����
void TIM1_BRK_TIM9_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	 
	if(TIM_GetITStatus(TIM9,TIM_IT_Update)==SET)
	{        
        switch(Timer9_State)
        {
            case Motor_State:
            {
                uint32_t msg_q = Motor_TimerTimeout;
                xQueueSendFromISR(Message_Queue,(void *)&msg_q,&xHigherPriorityTaskWoken);//������з�������
            }
            break;
            case Motor_State_1:
            {
                uint32_t msg_q = Motor_TimerTimeout_1;
                xQueueSendFromISR(Message_Queue,(void *)&msg_q,&xHigherPriorityTaskWoken);//������з�������
            }
            break;
            case Push_State:
            {
                uint32_t msg_q = Push_TimerTimeout;
                xQueueSendFromISR(Message_Queue,(void *)&msg_q,&xHigherPriorityTaskWoken);//������з�������
            }
            break;
            default:
                break;
        }
		
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
	}
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);  
}




