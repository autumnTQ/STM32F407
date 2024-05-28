#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "PWM.h"
#include "string.h"
#include "malloc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "protocol.h"
#include "common.h"
#include "lcd_spi_154.h"
#include "Sensor.h"
#include "Internal_FLASH.h" //读写内部FLASH
#include "Switch_GPIO.h"	//GPIO端口开关

int ID_Value = 0;		// 设备号ID
int Pos_Value = 0;		// 当前位置值
int SafePos_Value = 0;	// 接收到的安全位置值 用于电机回零点
int Speed_Value = 0;	// 运行速度
int RunFlag_Value = 0;	// 停止标志位   0:运转    1:停止
int GoodPos_Value = 0;	// 当前位置与目标位置是否相等标志位    0:不相等   1:相等
int ZeroFlag_Value = 0; // 原点标志位   0:不在原点 1:在原点
int S1_Value = 0;		// 传感器状态
int S2_Value = 0;
int S3_Value = 0;
int S5_Value = 0;
int S6_Value = 0;

int ConfigureSpeed_Value = 0;	 // 电机配置运行速度
int ConfigureAccSpeed_Value = 0; // 电机配置加速度
int ConfigureDecSpeed_Value = 0; // 电机配置减速度
int pcSetCurrentPos = 0;		 // 接收PC设置的运行目标位置
int pcSetSpeed = 0;				 // 接收PC设置的运行速度
int g_MotorFlag = 0;			 // 用于记录读取电机和推杆的寄存器的个数

static uint32_t usart4_num = 0;
static uint8_t tpype = 0;

TIMER_STATE Timer9_State = No_State; // 定时器9的状态

// 开始任务
#define START_TASK_PRIO 1			 // 任务优先级
#define START_STK_SIZE 256			 // 任务堆栈大小
TaskHandle_t StartTask_Handler;		 // 任务句柄
void Start_task(void *pvParameters); // 任务函数

// 串口接收上位机数据处理任务
#define TASK2_TASK_PRIO 1					// 任务优先级
#define TASK1_STK_SIZE 256					// 任务堆栈大小
TaskHandle_t SerialRxRroc_Task_Handler;		// 任务句柄
void SerialRxRroc_Task(void *pvParameters); // 任务函数

// 电机状态查询任务
#define TASK2_TASK_PRIO 1				  // 任务优先级
#define TASK1_STK_SIZE 256				  // 任务堆栈大小
TaskHandle_t MotorState_Task_Handler;	  // 任务句柄
void MotorState_Task(void *pvParameters); // 任务函数

uint8_t PC_RxBuf[256]; // 上位机通过串口1发送过来的数据缓存区
uint8_t PC_RxLen = 0;  // 接收数据长度

uint8_t Motor_RxBuf[256]; // 串口2接收电机反馈过来的指令缓存区
uint8_t Motor_RxLen = 0;  // 接收数据长度

uint8_t Grip_RxBuf[256]; // 串口3接收夹爪反馈过来的指令的缓存区
uint8_t Grip_RxLen = 0;	 // 接收数据长度

uint8_t Push_RxBuf[256]; // 串口4接收推杆反馈过来的指令的缓存区
uint8_t Push_RxLen = 0;	 // 接收数据长度

uint8_t Rec[RX_MAX_LEN]; // 串口1+DMA 接收、发送数据缓存区

uint8_t Rec1[RX_MAX_LEN]; // 串口1+DMA 接收、发送数据缓存区

QueueHandle_t Message_Queue;	   // 串口接收上位机数据消息队列句柄
QueueHandle_t Motor_Message_Queue; // 电机消息队列句柄

UART_T uart2_t;
UART_T uart4_t;

int main(void)
{
	uart2_t.Flag = 0;
	uart2_t.Fsm = 0;
	uart4_t.Flag = 0;
	uart4_t.Fsm = 0;

	Message_Queue = xQueueCreate(1, sizeof(uint8_t));		// 创建消息Message_Queue
	Motor_Message_Queue = xQueueCreate(1, sizeof(uint8_t)); // 创建消息Motor_Message_Queue

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置系统中断优先级分组4

	delay_init(168);					// 初始化延时函数
	Switch_GPIOInit();					// 高边驱动GPIO端口开关初始化
	TIM5_Int_Init(10000 - 1, 8400 - 1); // 初始化定时器5， 周期1s      控制电机超时停止运行
	TIM9_Int_Init(1000 - 1, 16800 - 1); // 初始化定时器9,周期100ms
	uart_init2(9600);					// 初始化串口2
	uart_init4(9600);					// 初始化串口4 与上位机进行通信
	//	uart_init1(9600);               // 初始化串口1+DMA

	my_mem_init(SRAMIN); // 初始化内部内存池
	mymemset(Motor_RxBuf, 0, sizeof(Motor_RxBuf));

	// 创建开始任务
	xTaskCreate((TaskFunction_t)Start_task,			 // 任务函数
				(const char *)"start_task",			 // 任务名称
				(uint16_t)START_STK_SIZE,			 // 任务堆栈大小
				(void *)NULL,						 // 传递给任务函数的参数
				(UBaseType_t)START_TASK_PRIO,		 // 任务优先级
				(TaskHandle_t *)&StartTask_Handler); // 任务句柄

	vTaskStartScheduler(); // 开启任务调度
}

// 开始任务函数
void Start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); // 进入临界区

	// 创建LCD屏幕显示任务
	//	xTaskCreate((TaskFunction_t )Display_Task,
	//				(const char *   )"Display_Task",
	//				(uint16_t       )TASK1_STK_SIZE,
	//				(void *         )NULL,
	//				(UBaseType_t    )TASK1_TASK_PRIO,
	//				(TaskHandle_t * )&Display_Task_Handler);

	// 创建串口接收数据处理任务
	xTaskCreate((TaskFunction_t)SerialRxRroc_Task,
				(const char *)"SerialRxRroc_Task",
				(uint16_t)TASK1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)TASK2_TASK_PRIO,
				(TaskHandle_t *)&SerialRxRroc_Task_Handler);

	// 创建电机状态查询任务
	xTaskCreate((TaskFunction_t)MotorState_Task,
				(const char *)"MotorState_Task",
				(uint16_t)TASK1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)TASK2_TASK_PRIO,
				(TaskHandle_t *)&MotorState_Task_Handler);

	vTaskDelete(StartTask_Handler); // 删除开始任务
	taskEXIT_CRITICAL();			// 退出临界区
}

/************************************************
**下发指令任务
**************************************************/
void SerialRxRroc_Task(void *pvParameters)
{
	uint8_t deviceDevice = 0;
	BaseType_t xHigherPriorityTaskWoken;
	while (1)
	{
		if (Message_Queue != NULL)
		{
			if (xQueueReceive(Message_Queue, &deviceDevice, portMAX_DELAY)) // 请求消息
			{
				// 判断接收到的数据是否正确————校验和
				unsigned short iCrc = CrcVerifice(PC_RxBuf, PC_RxLen - 2);

				if (((iCrc & 0Xff) != Rec[PC_RxLen - 2]) | (((iCrc >> 8) & 0xff) != Rec[PC_RxLen - 1]))
				{
					continue; // 校验和失败
				}

				switch (deviceDevice)
				{
				case 0x03:
				{
                    usart4_num++;

                    if(usart4_num == 6)
                    {
                        usart4_num = 0;
                        
                        xQueueSendFromISR(Motor_Message_Queue, (void *)&tpype, &xHigherPriorityTaskWoken); 
                        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                    }

				}
				break;
				case 0x04: 
				{
                    if(usart4_num == 0)
                    {
                        PC_QuerySensorState[0] = PC_RxBuf[0];
						tpype = Sensor_Type;
                    }
                    
					usart4_num++;
                    
					if(usart4_num == 6)
					{
						usart4_num = 0;
						xQueueSendFromISR(Motor_Message_Queue, (void *)&tpype, &xHigherPriorityTaskWoken); 
						portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
					}
				}
				break;
				case 0x06:  //电机归零和推杆运行
				{
                      usart4_num = 1;

					//电机归零
                    if((PC_RxBuf[2] == 0x00) & (PC_RxBuf[3] == 0x00))
					{
                        Set_MotorZero[0] = PC_RxBuf[0];
						tpype = Motor_Rx;
					}
					//推杆运行
					else if((PC_RxBuf[2] == 0x00) & (PC_RxBuf[3] == 0x02))
					{
                        Set_PushTargetPosition[0] = PC_RxBuf[0];
                        
                        Set_PushTargetPosition[3] = PC_RxBuf[4];
                        Set_PushTargetPosition[4] = PC_RxBuf[5];
                        
						tpype = Push_Rx;
					}
				}
				break;
				case 0x10: 
				{
                    usart4_num = 1;
                    
					//设置电机速度
                    if((PC_RxBuf[2] == 0x00) & (PC_RxBuf[3] == 0x03))
					{
                        Set_MotorSpeed[0] = PC_RxBuf[0];
                        
                        Set_MotorSpeed[9] = PC_RxBuf[7];
                        Set_MotorSpeed[10] = PC_RxBuf[8];
                        Set_MotorSpeed[11] = PC_RxBuf[9];
                        Set_MotorSpeed[12] = PC_RxBuf[10];
                        
                        Set_MotorSpeed[13] = PC_RxBuf[7];
                        Set_MotorSpeed[14] = PC_RxBuf[8];
                        Set_MotorSpeed[15] = PC_RxBuf[9];
                        Set_MotorSpeed[16] = PC_RxBuf[10];
                        
                        Set_MotorSpeed[17] = PC_RxBuf[7];
                        Set_MotorSpeed[18] = PC_RxBuf[8];
                        Set_MotorSpeed[19] = PC_RxBuf[9];
                        Set_MotorSpeed[20] = PC_RxBuf[10];
                        
                        
						tpype = MotorConfigure_Rx;
					}
					//电机运行目标位置
					else if((PC_RxBuf[2] == 0x00) & (PC_RxBuf[3] == 0x01))
					{
                        //液面探测
                        if((PC_RxBuf[0] == 0x01) & (PC_RxBuf[7] == 0x2E) & (PC_RxBuf[8] == 0xE0))
                        {
                            PC_Set_LevelDet[0] = PC_RxBuf[0];
                            
                            PC_Set_LevelDet[7] = 0x00;
                            PC_Set_LevelDet[8] = 0x00;
                            PC_Set_LevelDet[9] = 0x00;
                            PC_Set_LevelDet[10] = 0x00;

                            tpype = LevelDet;
                            tpype = LevelDet;
                        }
                        else  //运行目标位置  增加注释
                        {
                            Set_TargetPosition[0] = PC_RxBuf[0];
                            Set_TargetPosition[7] = PC_RxBuf[7];
                            Set_TargetPosition[8] = PC_RxBuf[8];
                            Set_TargetPosition[9] = PC_RxBuf[9];
                            Set_TargetPosition[10] = PC_RxBuf[10];

                            tpype = Temp_Type;
                        }
                        
					}
				}
				break;
				}
			}
		}
		vTaskDelay(10); // 延时10ms，也就是10个时钟节拍
	}
}

/************************************************
**反馈数据给上位机任务
**************************************************/
void MotorState_Task(void *pvParameters)
{
	// uint8_t tpype = 0;

	while (1)
	{
		if (Motor_Message_Queue != NULL)
		{
			if (xQueueReceive(Motor_Message_Queue, &tpype, portMAX_DELAY)) // 请求消息
			{
				switch (tpype)
				{
				case Motor_Rx: // 回馈电机归零的数据
				{
					SerialSendData_PC(Set_MotorZero, sizeof(Set_MotorZero)); // 发送数据给上位机
				}
				break;
				case Temp_Type: // 回馈电机运行目标位置的数据
				{
					SerialSendData_PC(Set_TargetPosition, sizeof(Set_TargetPosition)); // 发送数据给上位机
				}
				break;
				case MotorConfigure_Rx: // 回馈电机设置速度的数据
				{
					SerialSendData_PC(Set_MotorSpeed, sizeof(Set_MotorSpeed)); // 发送数据给上位机
				}
				break;
				case Push_Rx: // 回馈推杆运行的数据
				{
					SerialSendData_PC(Set_PushTargetPosition, sizeof(Set_PushTargetPosition)); // 发送数据给上位机
				}
				break;
				case Sensor_Type: // 微动开关触发
				{
					SerialSendData_PC(PC_QuerySensorState, sizeof(PC_QuerySensorState)); // 发送数据给上位机
				}
				break;
                case LevelDet:
                {
					SerialSendData_PC(PC_Set_LevelDet, sizeof(PC_Set_LevelDet)); // 发送数据给上位机
                }
                break;    
				default:
					break;
				}
			}
			vTaskDelay(10); // 延时10ms，也就是10个时钟节拍
		}
	}
}
