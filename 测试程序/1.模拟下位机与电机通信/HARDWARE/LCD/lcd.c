#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "delay.h"	 
#include "lcd_spi_154.h"
#include "FreeRTOS.h"
#include "task.h"

#define ID 0
#define Pos 25
#define Speed 50
#define RunFlag 75
#define GoodPos 100
#define ZeroFlag 125
#define S1 148
#define S2 171
#define S3 194
#define S5 217

extern int ID_Value;		// �豸��ID
extern int Pos_Value;		// ��ǰλ��ֵ
extern int SafePos_Value;	// ���յ��İ�ȫλ��ֵ ���ڵ�������
extern int Speed_Value;	// �����ٶ�
extern int RunFlag_Value;	// ֹͣ��־λ   0:��ת    1:ֹͣ
extern int GoodPos_Value;	// ��ǰλ����Ŀ��λ���Ƿ���ȱ�־λ    0:�����   1:���
extern int ZeroFlag_Value; // ԭ���־λ   0:����ԭ�� 1:��ԭ��
extern int S1_Value;		// ������״̬
extern int S2_Value;
extern int S3_Value;
extern int S5_Value;
extern int S6_Value;
int LCDUpdate = 0;			     // LCD��Ļˢ�±�־λ
int LCDUpdate1 = 1;			  // LCD��Ļˢ�±�־λ
int LCDUpdate2 = 0;			  // LCD��Ļˢ�±�־λ




/*************************************************************************************************
 *	��������: ��������
 *************************************************************************************************/
void LCD_Test_Clear(void)
{
	LCD_SetTextFont(&CH_Font24); // ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(LCD_BLACK);	 // ���û�����ɫ

	LCD_SetBackColor(LCD_BLACK); //	���ñ���ɫ
	LCD154_Clear();				 // ����

	LCD_SetTextFont(&CH_Font24);
	LCD_SetColor(LIGHT_CYAN); // ���û��ʣ�����ɫ
	LCD_DisplayText(0, ID, "ID:");
	LCD_DisplayText(0, Pos, "Pos:");

	LCD_SetColor(LIGHT_YELLOW); // ���û��ʣ�����ɫ
	LCD_DisplayText(0, Speed, "Speed:");
	LCD_DisplayText(0, RunFlag, "RunFlag:");

	LCD_SetColor(LIGHT_RED); // ���û���	������ɫ
	LCD_DisplayText(0, GoodPos, "GoodPos:");
	LCD_DisplayText(0, ZeroFlag, "ZeroFlag:");

	LCD_SetColor(LCD_BLUE); // ���û���	������ɫ
	LCD_DisplayText(0, S1, "S1:");
	LCD_DisplayText(0, S2, "S2:");
	LCD_DisplayText(0, S3, "S3:");
	LCD_DisplayText(0, S5, "S5:");

	delay_ms(1000);
}

/************************************************************************************************
 *	��������: ������ʾ������������С��
 *************************************************************************************************/
void LCD_Test_Variable(void)
{
	LCD_SetColor(LIGHT_CYAN);	 // ���û���	������ɫ
	LCD_ShowNumMode(Fill_Space); // ����λ���ո�
	// ID
	LCD_DisplayNumber(80, ID, ID_Value, 4); // ��ʾ����
	// ��ǰλ��
	LCD_DisplayNumber(80, Pos, Pos_Value, 7); // ��ʾ����

	// ��ǰ�ٶ�
	LCD_SetColor(LIGHT_YELLOW);					  // ���û��ʣ�����ɫ
	LCD_ShowNumMode(Fill_Space);				  // ����λ��� �ո�
	LCD_DisplayNumber(80, Speed, Speed_Value, 7); // ��ʾ����

	// ���б�־
	LCD_SetColor(LIGHT_YELLOW);						   // ���û��ʣ�����ɫ
	LCD_ShowNumMode(Fill_Space);					   // ����λ��� �ո�
	LCD_DisplayNumber(110, RunFlag, RunFlag_Value, 1); // ��ʾ����

	// ��ǰλ����Ŀ��λ���Ƿ����
	LCD_SetColor(LIGHT_RED);						   // ���û��ʣ�����ɫ
	LCD_ShowNumMode(Fill_Space);					   // ����λ��� �ո�
	LCD_DisplayNumber(110, GoodPos, GoodPos_Value, 1); // ��ʾ����

	// �����־λ
	LCD_SetColor(LIGHT_RED);							 // ���û��ʣ�����ɫ
	LCD_ShowNumMode(Fill_Space);						 // ����λ��� �ո�
	LCD_DisplayNumber(110, ZeroFlag, ZeroFlag_Value, 1); // ��ʾ����

	// S1״̬
	LCD_SetColor(LCD_BLUE);					 // ���û��ʣ�����ɫ
	LCD_ShowNumMode(Fill_Space);			 // ����λ��� �ո�
	LCD_DisplayNumber(110, S1, S1_Value, 1); // ��ʾ����

	LCD_ShowNumMode(Fill_Space);			 // ����λ��� �ո�
	LCD_DisplayNumber(110, S2, S2_Value, 1); // ��ʾ����

	LCD_ShowNumMode(Fill_Space);			 // ����λ��� �ո�
	LCD_DisplayNumber(110, S3, S3_Value, 1); // ��ʾ����

	LCD_ShowNumMode(Fill_Space);			 // ����λ��� �ո�
	LCD_DisplayNumber(110, S5, S5_Value, 1); // ��ʾ����
}



// LCD��Ļ��ʾ����
#define TASK1_TASK_PRIO 1			   // �������ȼ�
#define TASK1_STK_SIZE 256			   // �����ջ��С
TaskHandle_t Display_Task_Handler;	   // ������
void Display_Task(void *pvParameters); // ������

/************************************************
**LCD��Ļ��ʾ����
**************************************************/
void Display_Task(void *pvParameters)
{
	while (1)
	{
		if (LCDUpdate)
		{
			LCD_Test_Variable(); // ������ʾ������������С��
		}
		if (LCDUpdate1)
		{
			LCD_SetTextFont(&CH_Font32); // ����2424��������,ASCII�����ӦΪ2412
			LCD_SetColor(LCD_BLACK);	 // ���û�����ɫ

			LCD_SetBackColor(LCD_BLACK); //	���ñ���ɫ
			LCD154_Clear();				 // ����

			LCD_SetTextFont(&CH_Font32);
			LCD_SetColor(LIGHT_RED); // ���û��ʣ�����ɫ
			LCD_DisplayText(0, 10, "Freq: 500Hz");
			LCD_DisplayText(0, 50, "Duty:     %");
			LCDUpdate1 = 0;
			LCDUpdate2 = 1;
		}
		if (LCDUpdate2)
		{
			LCD_DisplayNumber(100, 50, 40 / 2, 3); // ��ʾ����
		}
		vTaskDelay(10); // ��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}


























