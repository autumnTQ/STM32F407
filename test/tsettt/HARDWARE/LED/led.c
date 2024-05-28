#include "led.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//LED驱动代码	   
////////////////////////////////////////////////////////////////////////////////// 	 

void LED_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOC, ENABLE); 	//初始化GPIOG时钟	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;   //输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	//上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //速度选择
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	GPIO_SetBits(GPIOC,GPIO_Pin_13); 
}




