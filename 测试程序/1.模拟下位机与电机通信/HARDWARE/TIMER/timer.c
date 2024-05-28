#include "timer.h"
#include "led.h"
#include "malloc.h"
#include "usart.h"
#include "string.h"

// 如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h" //FreeRTOS使用
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//定时器使用配置          APB1-(84MHz): 
//TIM3  优先级 1   ------->使其为FreeRTOS的时间统计提供时基 
//TIM5  优先级 10  ------->用于检测电机超时运行，超时10s停止电机运行
//
//                        APB2-(168MHz):
//TIM9   优先级 7  ------->用于定时查询电机的状态 100ms

static uint8_t timer5_num = 0;      	// 记录定时器5的中断次数
extern QueueHandle_t Motor_Message_Queue;   //电机消息队列句柄
extern QueueHandle_t Message_Queue;		    // 信息队列句柄

extern TIMER_STATE Timer9_State;

//FreeRTOS时间统计所用的节拍计数器
volatile unsigned long long FreeRTOSRunTimeTicks;

//初始化TIM3使其为FreeRTOS的时间统计提供时基
void ConfigureTimeForRunTimeStats(void)
{
	//定时器3初始化，定时器时钟为84M，分频系数为84-1，所以定时器3的频率
	//为84M/84=1M，自动重装载为50-1，那么定时器周期就是50us
	FreeRTOSRunTimeTicks=0;
	TIM3_Int_Init(50-1,84-1);	//初始化TIM3
}

//通用定时器使能
void TIM_Switch(TIM_TypeDef* TIMx, uint32_t Period, FunctionalState NewState)
{
	TIMx->CNT = 0;	//清理定时器计数
    TIMx->ARR = Period;   
	TIM_Cmd(TIMx,NewState);
}

//读取定时器的开启关闭的状态    1为运行， 0为停止
uint16_t Get_TimerState(TIM_TypeDef* TIMx)
{
    uint16_t state = TIMx->CR1 & TIM_CR1_CEN;   
    return state;    
}


/*************************************************************************************************************/
//通用定时器3中断初始化
void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;    //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;               //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;  //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;         //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM5_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);	           //使能TIM5时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 		           //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler= psc;  		           //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode= TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision= TIM_CKD_DIV1; 	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	//初始化TIM25
	
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);             //使能定时器5更新中断
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);             //清除更新中断标志位

	TIM_Cmd(TIM5, DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel= TIM5_IRQn; 		        //定时器5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 10; 	//抢占优先级10
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0x00;        //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

//通用定时器9中断初始化 查询电机状态
void TIM9_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);	//使能TIM9时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 		//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  		//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStructure);	//初始化TIM9	
	
	TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);      //使能定时器1更新中断
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);      //清除更新中断标志位

	TIM_Cmd(TIM9, DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; 		//定时器9中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7; 	//抢占优先级8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;         //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update)==SET)
	{
		FreeRTOSRunTimeTicks++;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  
}


//定时器5中断函数   超时关闭电机运行 
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

//定时器9中断服务函数
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
                xQueueSendFromISR(Message_Queue,(void *)&msg_q,&xHigherPriorityTaskWoken);//向队列中发送数据
            }
            break;
            case Motor_State_1:
            {
                uint32_t msg_q = Motor_TimerTimeout_1;
                xQueueSendFromISR(Message_Queue,(void *)&msg_q,&xHigherPriorityTaskWoken);//向队列中发送数据
            }
            break;
            case Push_State:
            {
                uint32_t msg_q = Push_TimerTimeout;
                xQueueSendFromISR(Message_Queue,(void *)&msg_q,&xHigherPriorityTaskWoken);//向队列中发送数据
            }
            break;
            default:
                break;
        }
		
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
	}
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);  
}




