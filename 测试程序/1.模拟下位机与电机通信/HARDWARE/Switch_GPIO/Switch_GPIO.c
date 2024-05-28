#include "Switch_GPIO.h"


//��ʼ��GPIO�˿�Ϊ�����������
//type��ѡ���Sw_GPIOx(A, B, C,,,,,,,,)
//GPIO_Pin������ѡ��  GPIO_Pin_x(0,1,2,,,,,,,)
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

//��ʼ���õ���GPIO�˿ں��� �������
void Switch_GPIOInit(void)
{
    Switch_GPIO(Sw_GPIOA, GPIO_Pin_12);     //������ PA12   
    
    Switch_GPIO(Sw_GPIOD, GPIO_Pin_3);      //��1  PD3   
    Switch_GPIO(Sw_GPIOD, GPIO_Pin_5);      //ע1  PD5
    
    Switch_GPIO(Sw_GPIOC, GPIO_Pin_10);     //��2  PC10
    Switch_GPIO(Sw_GPIOB, GPIO_Pin_5);      //ע2  PB5
//    Switch_GPIO(Sw_GPIOA, GPIO_Pin_15);     //ע2  PA15   ʵ����PC12

    Switch_GPIO(Sw_GPIOD, GPIO_Pin_7);     //���1   PD7
    Switch_GPIO(Sw_GPIOB, GPIO_Pin_3);     //���2   PB3
    
    Switch_GPIO(Sw_GPIOG, GPIO_Pin_10);    //��ϴҺ  PG10
    Switch_GPIO(Sw_GPIOG, GPIO_Pin_12);    //��ŷ�  PG12
    Switch_GPIO(Sw_GPIOG, GPIO_Pin_14);    //��ˮ    PG14

    Switch_GPIO(Sw_GPIOD, GPIO_Pin_1);     //���ȵ��  PD1
    
}


