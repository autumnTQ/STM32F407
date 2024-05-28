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

extern int ID_Value;		// 设备号ID
extern int Pos_Value;		// 当前位置值
extern int SafePos_Value;	// 接收到的安全位置值 用于电机回零点
extern int Speed_Value;	// 运行速度
extern int RunFlag_Value;	// 停止标志位   0:运转    1:停止
extern int GoodPos_Value;	// 当前位置与目标位置是否相等标志位    0:不相等   1:相等
extern int ZeroFlag_Value; // 原点标志位   0:不在原点 1:在原点
extern int S1_Value;		// 传感器状态
extern int S2_Value;
extern int S3_Value;
extern int S5_Value;
extern int S6_Value;
int LCDUpdate = 0;			     // LCD屏幕刷新标志位
int LCDUpdate1 = 1;			  // LCD屏幕刷新标志位
int LCDUpdate2 = 0;			  // LCD屏幕刷新标志位




/*************************************************************************************************
 *	函数功能: 清屏测试
 *************************************************************************************************/
void LCD_Test_Clear(void)
{
	LCD_SetTextFont(&CH_Font24); // 设置2424中文字体,ASCII字体对应为2412
	LCD_SetColor(LCD_BLACK);	 // 设置画笔颜色

	LCD_SetBackColor(LCD_BLACK); //	设置背景色
	LCD154_Clear();				 // 清屏

	LCD_SetTextFont(&CH_Font24);
	LCD_SetColor(LIGHT_CYAN); // 设置画笔，蓝绿色
	LCD_DisplayText(0, ID, "ID:");
	LCD_DisplayText(0, Pos, "Pos:");

	LCD_SetColor(LIGHT_YELLOW); // 设置画笔，亮黄色
	LCD_DisplayText(0, Speed, "Speed:");
	LCD_DisplayText(0, RunFlag, "RunFlag:");

	LCD_SetColor(LIGHT_RED); // 设置画笔	，亮红色
	LCD_DisplayText(0, GoodPos, "GoodPos:");
	LCD_DisplayText(0, ZeroFlag, "ZeroFlag:");

	LCD_SetColor(LCD_BLUE); // 设置画笔	，亮红色
	LCD_DisplayText(0, S1, "S1:");
	LCD_DisplayText(0, S2, "S2:");
	LCD_DisplayText(0, S3, "S3:");
	LCD_DisplayText(0, S5, "S5:");

	delay_ms(1000);
}

/************************************************************************************************
 *	函数功能: 变量显示，包括整数和小数
 *************************************************************************************************/
void LCD_Test_Variable(void)
{
	LCD_SetColor(LIGHT_CYAN);	 // 设置画笔	，蓝绿色
	LCD_ShowNumMode(Fill_Space); // 多余位填充空格
	// ID
	LCD_DisplayNumber(80, ID, ID_Value, 4); // 显示变量
	// 当前位置
	LCD_DisplayNumber(80, Pos, Pos_Value, 7); // 显示变量

	// 当前速度
	LCD_SetColor(LIGHT_YELLOW);					  // 设置画笔，亮黄色
	LCD_ShowNumMode(Fill_Space);				  // 多余位填充 空格
	LCD_DisplayNumber(80, Speed, Speed_Value, 7); // 显示变量

	// 运行标志
	LCD_SetColor(LIGHT_YELLOW);						   // 设置画笔，亮黄色
	LCD_ShowNumMode(Fill_Space);					   // 多余位填充 空格
	LCD_DisplayNumber(110, RunFlag, RunFlag_Value, 1); // 显示变量

	// 当前位置与目标位置是否相等
	LCD_SetColor(LIGHT_RED);						   // 设置画笔，亮黄色
	LCD_ShowNumMode(Fill_Space);					   // 多余位填充 空格
	LCD_DisplayNumber(110, GoodPos, GoodPos_Value, 1); // 显示变量

	// 归零标志位
	LCD_SetColor(LIGHT_RED);							 // 设置画笔，亮黄色
	LCD_ShowNumMode(Fill_Space);						 // 多余位填充 空格
	LCD_DisplayNumber(110, ZeroFlag, ZeroFlag_Value, 1); // 显示变量

	// S1状态
	LCD_SetColor(LCD_BLUE);					 // 设置画笔，亮黄色
	LCD_ShowNumMode(Fill_Space);			 // 多余位填充 空格
	LCD_DisplayNumber(110, S1, S1_Value, 1); // 显示变量

	LCD_ShowNumMode(Fill_Space);			 // 多余位填充 空格
	LCD_DisplayNumber(110, S2, S2_Value, 1); // 显示变量

	LCD_ShowNumMode(Fill_Space);			 // 多余位填充 空格
	LCD_DisplayNumber(110, S3, S3_Value, 1); // 显示变量

	LCD_ShowNumMode(Fill_Space);			 // 多余位填充 空格
	LCD_DisplayNumber(110, S5, S5_Value, 1); // 显示变量
}



// LCD屏幕显示任务
#define TASK1_TASK_PRIO 1			   // 任务优先级
#define TASK1_STK_SIZE 256			   // 任务堆栈大小
TaskHandle_t Display_Task_Handler;	   // 任务句柄
void Display_Task(void *pvParameters); // 任务函数

/************************************************
**LCD屏幕显示任务
**************************************************/
void Display_Task(void *pvParameters)
{
	while (1)
	{
		if (LCDUpdate)
		{
			LCD_Test_Variable(); // 变量显示，包括整数和小数
		}
		if (LCDUpdate1)
		{
			LCD_SetTextFont(&CH_Font32); // 设置2424中文字体,ASCII字体对应为2412
			LCD_SetColor(LCD_BLACK);	 // 设置画笔颜色

			LCD_SetBackColor(LCD_BLACK); //	设置背景色
			LCD154_Clear();				 // 清屏

			LCD_SetTextFont(&CH_Font32);
			LCD_SetColor(LIGHT_RED); // 设置画笔，蓝绿色
			LCD_DisplayText(0, 10, "Freq: 500Hz");
			LCD_DisplayText(0, 50, "Duty:     %");
			LCDUpdate1 = 0;
			LCDUpdate2 = 1;
		}
		if (LCDUpdate2)
		{
			LCD_DisplayNumber(100, 50, 40 / 2, 3); // 显示变量
		}
		vTaskDelay(10); // 延时10ms，也就是10个时钟节拍
	}
}


























