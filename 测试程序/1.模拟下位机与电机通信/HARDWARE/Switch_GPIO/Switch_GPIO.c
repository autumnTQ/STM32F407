#include "Switch_GPIO.h"


//初始化GPIO端口为下拉输出函数
//type：选择的Sw_GPIOx(A, B, C,,,,,,,,)
//GPIO_Pin：引脚选择  GPIO_Pin_x(0,1,2,,,,,,,)
uint8_t Switch_GPIO(SwGPIOType type, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;	
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	switch(type)
	{
        case Sw_GPIOA:
                    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	

                    GPIO_Init(GPIOA, &GPIO_InitStructure);
                    GPIO_ResetBits(GPIOA, GPIO_Pin);	
        break;
        case Sw_GPIOB:
                    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
                    
                    GPIO_Init(GPIOB, &GPIO_InitStructure);
                    GPIO_ResetBits(GPIOB, GPIO_Pin);	
        break;
        case Sw_GPIOC:
                    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	
                    
                    GPIO_Init(GPIOC, &GPIO_InitStructure);
                    GPIO_ResetBits(GPIOC, GPIO_Pin);
                    	
        break;
        case Sw_GPIOD:
                    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	
                    
                    GPIO_Init(GPIOD, &GPIO_InitStructure);	
                    GPIO_ResetBits(GPIOD, GPIO_Pin);	
        break;
        case Sw_GPIOE:
                    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
                    
                    GPIO_Init(GPIOE, &GPIO_InitStructure);
                    GPIO_ResetBits(GPIOE, GPIO_Pin);	
        break;
        case Sw_GPIOF:
                    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	
                    
                    GPIO_Init(GPIOF, &GPIO_InitStructure);
                    GPIO_ResetBits(GPIOF, GPIO_Pin);	
        break;
        case Sw_GPIOG:
                    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	
                    
                    GPIO_Init(GPIOG, &GPIO_InitStructure);
                    GPIO_ResetBits(GPIOG, GPIO_Pin);	
        break;
        default:
               return 0;	
	}
	return 1;
}

//初始化用到的GPIO端口函数 下拉输出
void Switch_GPIOInit(void)
{
    Switch_GPIO(Sw_GPIOA, GPIO_Pin_12);     //蜂鸣器 PA12   
    
    Switch_GPIO(Sw_GPIOD, GPIO_Pin_3);      //抽1  PD3   
    Switch_GPIO(Sw_GPIOD, GPIO_Pin_5);      //注1  PD5
    
    Switch_GPIO(Sw_GPIOC, GPIO_Pin_10);     //抽2  PC10
    Switch_GPIO(Sw_GPIOB, GPIO_Pin_5);      //注2  PB5
//    Switch_GPIO(Sw_GPIOA, GPIO_Pin_15);     //注2  PA15   实际是PC12

    Switch_GPIO(Sw_GPIOD, GPIO_Pin_7);     //抽废1   PD7
    Switch_GPIO(Sw_GPIOB, GPIO_Pin_3);     //抽废2   PB3
    
    Switch_GPIO(Sw_GPIOG, GPIO_Pin_10);    //清洗液  PG10
    Switch_GPIO(Sw_GPIOG, GPIO_Pin_12);    //电磁阀  PG12
    Switch_GPIO(Sw_GPIOG, GPIO_Pin_14);    //清水    PG14

    Switch_GPIO(Sw_GPIOD, GPIO_Pin_1);     //混匀电机  PD1
    
}


