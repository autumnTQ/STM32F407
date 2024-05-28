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
#include "Internal_FLASH.h" //��д�ڲ�FLASH
#include "Switch_GPIO.h"	//GPIO�˿ڿ���

int ID_Value = 0;		// �豸��ID
int Pos_Value = 0;		// ��ǰλ��ֵ
int SafePos_Value = 0;	// ���յ��İ�ȫλ��ֵ ���ڵ�������
int Speed_Value = 0;	// �����ٶ�
int RunFlag_Value = 0;	// ֹͣ��־λ   0:��ת    1:ֹͣ
int GoodPos_Value = 0;	// ��ǰλ����Ŀ��λ���Ƿ���ȱ�־λ    0:�����   1:���
int ZeroFlag_Value = 0; // ԭ���־λ   0:����ԭ�� 1:��ԭ��
int S1_Value = 0;		// ������״̬
int S2_Value = 0;
int S3_Value = 0;
int S5_Value = 0;
int S6_Value = 0;

int ConfigureSpeed_Value = 0;	 // ������������ٶ�
int ConfigureAccSpeed_Value = 0; // ������ü��ٶ�
int ConfigureDecSpeed_Value = 0; // ������ü��ٶ�
int pcSetCurrentPos = 0;		 // ����PC���õ�����Ŀ��λ��
int pcSetSpeed = 0;				 // ����PC���õ������ٶ�
int g_MotorFlag = 0;			 // ���ڼ�¼��ȡ������Ƹ˵ļĴ����ĸ���

static uint32_t usart4_num = 0;
static uint8_t tpype = 0;

TIMER_STATE Timer9_State = No_State; // ��ʱ��9��״̬

// ��ʼ����
#define START_TASK_PRIO 1			 // �������ȼ�
#define START_STK_SIZE 256			 // �����ջ��С
TaskHandle_t StartTask_Handler;		 // ������
void Start_task(void *pvParameters); // ������

// ���ڽ�����λ�����ݴ�������
#define TASK2_TASK_PRIO 1					// �������ȼ�
#define TASK1_STK_SIZE 256					// �����ջ��С
TaskHandle_t SerialRxRroc_Task_Handler;		// ������
void SerialRxRroc_Task(void *pvParameters); // ������

// ���״̬��ѯ����
#define TASK2_TASK_PRIO 1				  // �������ȼ�
#define TASK1_STK_SIZE 256				  // �����ջ��С
TaskHandle_t MotorState_Task_Handler;	  // ������
void MotorState_Task(void *pvParameters); // ������

uint8_t PC_RxBuf[256]; // ��λ��ͨ������1���͹��������ݻ�����
uint8_t PC_RxLen = 0;  // �������ݳ���

uint8_t Motor_RxBuf[256]; // ����2���յ������������ָ�����
uint8_t Motor_RxLen = 0;  // �������ݳ���

uint8_t Grip_RxBuf[256]; // ����3���ռ�צ����������ָ��Ļ�����
uint8_t Grip_RxLen = 0;	 // �������ݳ���

uint8_t Push_RxBuf[256]; // ����4�����Ƹ˷���������ָ��Ļ�����
uint8_t Push_RxLen = 0;	 // �������ݳ���

uint8_t Rec[RX_MAX_LEN]; // ����1+DMA ���ա��������ݻ�����

uint8_t Rec1[RX_MAX_LEN]; // ����1+DMA ���ա��������ݻ�����

QueueHandle_t Message_Queue;	   // ���ڽ�����λ��������Ϣ���о��
QueueHandle_t Motor_Message_Queue; // �����Ϣ���о��

UART_T uart2_t;
UART_T uart4_t;

int main(void)
{
	uart2_t.Flag = 0;
	uart2_t.Fsm = 0;
	uart4_t.Flag = 0;
	uart4_t.Fsm = 0;

	Message_Queue = xQueueCreate(1, sizeof(uint8_t));		// ������ϢMessage_Queue
	Motor_Message_Queue = xQueueCreate(1, sizeof(uint8_t)); // ������ϢMotor_Message_Queue

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // ����ϵͳ�ж����ȼ�����4

	delay_init(168);					// ��ʼ����ʱ����
	Switch_GPIOInit();					// �߱�����GPIO�˿ڿ��س�ʼ��
	TIM5_Int_Init(10000 - 1, 8400 - 1); // ��ʼ����ʱ��5�� ����1s      ���Ƶ����ʱֹͣ����
	TIM9_Int_Init(1000 - 1, 16800 - 1); // ��ʼ����ʱ��9,����100ms
	uart_init2(9600);					// ��ʼ������2
	uart_init4(9600);					// ��ʼ������4 ����λ������ͨ��
	//	uart_init1(9600);               // ��ʼ������1+DMA

	my_mem_init(SRAMIN); // ��ʼ���ڲ��ڴ��
	mymemset(Motor_RxBuf, 0, sizeof(Motor_RxBuf));

	// ������ʼ����
	xTaskCreate((TaskFunction_t)Start_task,			 // ������
				(const char *)"start_task",			 // ��������
				(uint16_t)START_STK_SIZE,			 // �����ջ��С
				(void *)NULL,						 // ���ݸ��������Ĳ���
				(UBaseType_t)START_TASK_PRIO,		 // �������ȼ�
				(TaskHandle_t *)&StartTask_Handler); // ������

	vTaskStartScheduler(); // �����������
}

// ��ʼ������
void Start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); // �����ٽ���

	// ����LCD��Ļ��ʾ����
	//	xTaskCreate((TaskFunction_t )Display_Task,
	//				(const char *   )"Display_Task",
	//				(uint16_t       )TASK1_STK_SIZE,
	//				(void *         )NULL,
	//				(UBaseType_t    )TASK1_TASK_PRIO,
	//				(TaskHandle_t * )&Display_Task_Handler);

	// �������ڽ������ݴ�������
	xTaskCreate((TaskFunction_t)SerialRxRroc_Task,
				(const char *)"SerialRxRroc_Task",
				(uint16_t)TASK1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)TASK2_TASK_PRIO,
				(TaskHandle_t *)&SerialRxRroc_Task_Handler);

	// �������״̬��ѯ����
	xTaskCreate((TaskFunction_t)MotorState_Task,
				(const char *)"MotorState_Task",
				(uint16_t)TASK1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)TASK2_TASK_PRIO,
				(TaskHandle_t *)&MotorState_Task_Handler);

	vTaskDelete(StartTask_Handler); // ɾ����ʼ����
	taskEXIT_CRITICAL();			// �˳��ٽ���
}

/************************************************
**�·�ָ������
**************************************************/
void SerialRxRroc_Task(void *pvParameters)
{
	uint8_t deviceDevice = 0;
	BaseType_t xHigherPriorityTaskWoken;
	while (1)
	{
		if (Message_Queue != NULL)
		{
			if (xQueueReceive(Message_Queue, &deviceDevice, portMAX_DELAY)) // ������Ϣ
			{
				// �жϽ��յ��������Ƿ���ȷ��������У���
				unsigned short iCrc = CrcVerifice(PC_RxBuf, PC_RxLen - 2);

				if (((iCrc & 0Xff) != Rec[PC_RxLen - 2]) | (((iCrc >> 8) & 0xff) != Rec[PC_RxLen - 1]))
				{
					continue; // У���ʧ��
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
				case 0x06:  //���������Ƹ�����
				{
                      usart4_num = 1;

					//�������
                    if((PC_RxBuf[2] == 0x00) & (PC_RxBuf[3] == 0x00))
					{
                        Set_MotorZero[0] = PC_RxBuf[0];
						tpype = Motor_Rx;
					}
					//�Ƹ�����
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
                    
					//���õ���ٶ�
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
					//�������Ŀ��λ��
					else if((PC_RxBuf[2] == 0x00) & (PC_RxBuf[3] == 0x01))
					{
                        //Һ��̽��
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
                        else  //����Ŀ��λ��  ����ע��
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
		vTaskDelay(10); // ��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}

/************************************************
**�������ݸ���λ������
**************************************************/
void MotorState_Task(void *pvParameters)
{
	// uint8_t tpype = 0;

	while (1)
	{
		if (Motor_Message_Queue != NULL)
		{
			if (xQueueReceive(Motor_Message_Queue, &tpype, portMAX_DELAY)) // ������Ϣ
			{
				switch (tpype)
				{
				case Motor_Rx: // ����������������
				{
					SerialSendData_PC(Set_MotorZero, sizeof(Set_MotorZero)); // �������ݸ���λ��
				}
				break;
				case Temp_Type: // �����������Ŀ��λ�õ�����
				{
					SerialSendData_PC(Set_TargetPosition, sizeof(Set_TargetPosition)); // �������ݸ���λ��
				}
				break;
				case MotorConfigure_Rx: // ������������ٶȵ�����
				{
					SerialSendData_PC(Set_MotorSpeed, sizeof(Set_MotorSpeed)); // �������ݸ���λ��
				}
				break;
				case Push_Rx: // �����Ƹ����е�����
				{
					SerialSendData_PC(Set_PushTargetPosition, sizeof(Set_PushTargetPosition)); // �������ݸ���λ��
				}
				break;
				case Sensor_Type: // ΢�����ش���
				{
					SerialSendData_PC(PC_QuerySensorState, sizeof(PC_QuerySensorState)); // �������ݸ���λ��
				}
				break;
                case LevelDet:
                {
					SerialSendData_PC(PC_Set_LevelDet, sizeof(PC_Set_LevelDet)); // �������ݸ���λ��
                }
                break;    
				default:
					break;
				}
			}
			vTaskDelay(10); // ��ʱ10ms��Ҳ����10��ʱ�ӽ���
		}
	}
}
