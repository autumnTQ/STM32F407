#include "led.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//LED��������	   
////////////////////////////////////////////////////////////////////////////////// 	 

void LED_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ��
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOC, ENABLE); 	//��ʼ��GPIOGʱ��	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;   //���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //�ٶ�ѡ��
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	GPIO_SetBits(GPIOC,GPIO_Pin_13); 
}




