
#include "stm32f4xx.h"
#include "led.h"   
#include "delay.h"
#include "key.h"  
#include "usart.h"
#include "PWM.h"

/***************************************************************************************************
*	函 数 名: main
*	入口参数: 无
*	返 回 值: 无
*	函数功能: 运行主程序
*	说    明: 无
****************************************************************************************************/

int main(void)
{
    uint16_t compare = 0; 
    
	Delay_Init();		//延时函数初始化
	LED_Init();			//LED初始化
	KEY_Init();			//按键IO口初始化
	Usart_Config ();	// USART初始化函数
    PWM_Init(200-1, 840-1, 0);     //频率500Hz   
    
	while (1)
	{
        if (KEY_Scan() == KEY_ON)        //如果按键按下
        {       
            compare += 20;
            PWM_SetCompare1(compare);

            if (compare >= 200)
            {
                compare = 0;
            }
        }            
	}
	
}





