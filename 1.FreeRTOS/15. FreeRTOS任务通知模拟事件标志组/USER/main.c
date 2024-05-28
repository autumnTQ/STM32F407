#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "freertos_demo.h"
 
int main(void)
{        
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);            //初始化延时函数
	uart_init(115200);		    //初始化串口波特率为115200
	LED_Init();					//初始化LED 
 	KEY_Init();					//按键初始化
    
 	FSMC_SRAM_Init();			//初始化外部SRAM  	
	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池
    
    freertos_demo();            //运行FreeRTOS例程
}


