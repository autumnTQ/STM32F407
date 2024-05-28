
#include "stm32f4xx.h"
#include "led.h"   
#include "delay.h"
#include "key.h"  
#include "usart.h"
#include "PWM.h"

/***************************************************************************************************
*	�� �� ��: main
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: ����������
*	˵    ��: ��
****************************************************************************************************/

int main(void)
{
    uint16_t compare = 0; 
    
	Delay_Init();		//��ʱ������ʼ��
	LED_Init();			//LED��ʼ��
	KEY_Init();			//����IO�ڳ�ʼ��
	Usart_Config ();	// USART��ʼ������
    PWM_Init(200-1, 840-1, 0);     //Ƶ��500Hz   
    
	while (1)
	{
        if (KEY_Scan() == KEY_ON)        //�����������
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





