#include "sys.h"
#include "usart.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////
// ���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h" //FreeRTOSʹ��
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "common.h"
#endif
//////////////////////////////////////////////////////////////////////////////////
// ����ʹ������
// USART1  ���ȼ� 5     ------->����
// USART2  ���ȼ� 6     ------->������Τ��˹������Ƹ�ͨ��
// UART4   ���ȼ� 5     ------->��������λ������ͨ��
extern UART_T uart2_t;
extern UART_T uart4_t;

extern uint8_t PC_RxBuf[256];
extern uint8_t PC_RxLen;

extern uint8_t Motor_RxBuf[256];
extern uint8_t Motor_RxLen;

extern uint8_t Grip_RxBuf[256];
extern uint8_t Grip_RxLen;

extern uint8_t Push_RxBuf[256];
extern uint8_t Push_RxLen;

extern uint8_t Rec[RX_MAX_LEN];	 // ���ա��������ݻ�����
extern uint8_t Rec1[RX_MAX_LEN]; // ���ա��������ݻ�����

extern QueueHandle_t Message_Queue;		  // ��Ϣ���о��
extern QueueHandle_t Motor_Message_Queue; // �����Ϣ���о��
extern int g_MotorFlag;					  //
//////////////////////////////////////////////////////////////////
// �������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// ��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
// ����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}

// �ض���fputc����
#define Serial_DEBUG 1 // ���ڵ��Դ�ӡ 1�������Ӧ  0���������ӡ

int fputc(int ch, FILE *f)
{
	if (Serial_DEBUG)
	{
		while ((UART5->SR & 0X40) == 0)
			; // ѭ������,ֱ���������
		UART5->DR = (u8)ch;
		return ch;
	}
}
#endif

// ��ʼ��IO ����2
// bound:������
void uart_init2(uint32_t bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // ʹ��USART2ʱ��

	// ����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // GPIOA3����ΪUSART2

	// USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		   // ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   // �ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		   // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		   // ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   // ��ʼ��PA2��PA3

	// USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;										// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ
	USART_Init(USART2, &USART_InitStructure);										// ��ʼ������2

	USART_Cmd(USART2, ENABLE); // ʹ�ܴ���2

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // ��������ж�

	// Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		  // ����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6; // ��ռ���ȼ�7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // �����ȼ�0
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

// ��ʼ��IO ����4  ���ڸ��Ƹ˷���ָ��  �Ƹ˲�����Ĭ��115200
// bound:������   PA0_TX   PA1_RX
void uart_init4(uint32_t bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); // ʹ��USART3ʱ��

	// ����3��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4); // GPIOA0����ΪUART4
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4); // GPIOA1����ΪUART4

	// UART4�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  // ���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // ��ʼ��PB10��PB11

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; // ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;										// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ
	USART_Init(UART4, &USART_InitStructure);										// ��ʼ������4

	// Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		  // ����4�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; // ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // �����ȼ�0
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); // ���������ж�
	USART_Cmd(UART4, ENABLE);					  // ʹ�ܴ���4
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE); // ʹ��DMA����

	// DMA����
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); // ʹ��DMA1ʱ��

	DMA_DeInit(DMA1_Stream2); // ��λ��ʼ��DMA������
	// ȷ��DMA��������λ���  //������ȡ EN λ��ֵΪ 0 ʱ���ű�ʾ��������������������ڿ�ʼ�κ�����������֮ǰ����Ҫ�ȴ� EN λ�� 0
	while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE)
	{
	}

	/* ���ý��� */
	DMA_InitStructure.DMA_BufferSize = RX_MAX_LEN;							// �������ݵĳ���
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;							// ���ڽ���ͨ��
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;					// DMA���˷������赽�洢��
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Rec;					// �洢����ַ
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				// �洢��ͻ��ģʽѡ�񣺵���ģʽ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			// �洢�����ݿ�ȣ��ֽ�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// ʹ�ܴ洢����ַ�Զ���������
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							// DMA ����ģʽѡ��ѭ������
	DMA_InitStructure.DMA_PeripheralBaseAddr = UART4_BASE + 0x04;			// �����ַ
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		// ����ͻ��ģʽѡ�񣺵���ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ�ȣ��ֽ�
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		// ��ֹ�����ַ�Զ���������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					// ������������������ȼ����е�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;					/* FIFO����������� */
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;

	DMA_Init(DMA1_Stream2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream2, ENABLE); // ����ʹ��
}

// ����2�жϷ������
void USART2_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint32_t uart2_msg_q = 0;

	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // �����ж�
	{
		uart2_t.Tmp = USART_ReceiveData(USART2); // ��ȡ���յ�������

		if (uart2_t.Flag == 1)
		{
			uart2_t.RxBuf[uart2_t.RxCnt++] = uart2_t.Tmp;
			if (uart2_t.RxCnt >= (uart2_t.RxBuf[2] + 5)) // ��������ݱ��������������
			{
				uart2_t.Flag = 0;

				if (uart2_t.RxBuf[2] == 0x02) // �Ƹ˷���������
				{
					mymemcpy(Push_RxBuf, uart2_t.RxBuf, uart2_t.RxCnt); // ���յ��Ƹ˷����������ݷ���Push_RxBuf
					uart2_msg_q = Push_Rx;
				}
				else if (uart2_t.RxBuf[2] == 0x0C) // ��ѯ���״̬����������
				{

					unsigned short iCrc = CrcVerifice(PC_RxBuf, PC_RxLen - 2);

					if (((iCrc & 0Xff) == Rec[PC_RxLen - 2]) | (((iCrc >> 8) & 0xff) == Rec[PC_RxLen - 1]))
					{
						mymemcpy(Motor_RxBuf, uart2_t.RxBuf, uart2_t.RxCnt); // ���յ���������������ݷ���Motor_RxBuf
						uart2_msg_q = Motor_Rx;
					}
				}
				else if (uart2_t.RxBuf[2] == 0x12) // ��ѯ������÷���������
				{
					mymemcpy(Motor_RxBuf, uart2_t.RxBuf, uart2_t.RxCnt); // ���յ���������������ݷ���Motor_RxBuf
					uart2_msg_q = MotorConfigure_Rx;
				}
				else if (uart2_t.RxBuf[2] == 0x05) // ��ѯ�¶ȿ��ư巴��������
				{
					mymemcpy(Motor_RxBuf, uart2_t.RxBuf, uart2_t.RxCnt); // ���յ���������������ݷ���Motor_RxBuf
					uart2_msg_q = Temp_Type;
				}

				uart2_t.RxCnt = 0;
				mymemset(uart2_t.RxBuf, 0, sizeof(uart2_t.RxBuf));
				xQueueSendFromISR(Motor_Message_Queue, (void *)&uart2_msg_q, &xHigherPriorityTaskWoken); // ������з�������
			}
		}
		else
		{
			switch (uart2_t.Fsm)
			{
			case 0:
				if (uart2_t.Tmp == PC_RxBuf[8]) // �жϵ��ID��
				{
					uart2_t.Fsm = 1;
					mymemset(uart2_t.RxBuf, 0, sizeof(uart2_t.RxBuf));
					uart2_t.RxBuf[0] = uart2_t.Tmp;
				}
				break;
			case 1:
				if ((uart2_t.Tmp == 0x04) || (uart2_t.Tmp == 0x03) || (uart2_t.Tmp == 0x10)) // �жϽ��չ�����
				{
					uart2_t.Fsm = 2;
					uart2_t.RxBuf[1] = uart2_t.Tmp;
				}
				else
				{
					uart2_t.Fsm = 0;
				}
				break;
			case 2:
				if (uart2_t.Tmp == g_MotorFlag) // �ж϶�ȡ��Ϣ�Ĵ�������
				{
					g_MotorFlag = 0;

					uart2_t.Fsm = 0;
					uart2_t.RxBuf[2] = uart2_t.Tmp;
					uart2_t.Flag = 1;
					uart2_t.RxCnt = 3;
				}
				else
				{
					uart2_t.Fsm = 0;
				}
				break;
			default:
				break;
			}
		}
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // �����Ҫ�Ļ�����һ�������л�
	}
}

// ����4�жϷ������  �����Ƹ˷���������
void UART4_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint16_t temp; // ��ȡDMA��ʣ������ݳ���
	uint32_t uart0_msg_q = 0;

	if (USART_GetFlagStatus(UART4, USART_FLAG_IDLE) != RESET)
	{
		temp = UART4->SR;
		temp = UART4->DR; // ���IDLE�жϡ�

		DMA_Cmd(DMA1_Stream2, DISABLE);					  // ʧ��DMA����
		temp = DMA_GetCurrDataCounter(DMA1_Stream2);	  // ��ȡʣ��Ľ������ݵ���
		DMA_ClearFlag(DMA1_Stream2, DMA_FLAG_TCIF4);	  // �����־λ
		DMA_SetCurrDataCounter(DMA1_Stream2, RX_MAX_LEN); // ������Ҫ���յ���

		PC_RxLen = RX_MAX_LEN - temp; // ���յ������������ݳ���

		mymemcpy(PC_RxBuf, Rec, PC_RxLen); // ���յ�PC�����������ݷ���PC_RxBuf

		if (PC_RxBuf[1] == 0x03)
		{
			uart0_msg_q = 0x03;
		}
		else if(PC_RxBuf[1] == 0x04)
		{
			uart0_msg_q = 0x04;
		}
		else if(PC_RxBuf[1] == 0x06)
		{
			uart0_msg_q = 0x06;
		}
		else if(PC_RxBuf[1] == 0x10)
		{
			uart0_msg_q = 0x10;
		}
		
		xQueueSendFromISR(Message_Queue, (void *)&uart0_msg_q, &xHigherPriorityTaskWoken);

		DMA_Cmd(DMA1_Stream2, ENABLE);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**********************************************************/
//                  ���Դ���1ʹ��DMA����ͨ��
/**********************************************************/

static void USART_RX_DMA_Config(void) // USART1_RX------DMA2_Stream5  DMA_Channel_4
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(USART_RX_DMA_CLK, ENABLE); // ʹ��DMA2ʱ��

	DMA_DeInit(USART_RX_DMA_STREAM); // ��λ��ʼ��DMA������
	// ȷ��DMA��������λ���  //������ȡ EN λ��ֵΪ 0 ʱ���ű�ʾ��������������������ڿ�ʼ�κ�����������֮ǰ����Ҫ�ȴ� EN λ�� 0
	while (DMA_GetCmdStatus(USART_RX_DMA_STREAM) != DISABLE)
	{
	}

	/* ���ý��� */
	DMA_InitStructure.DMA_BufferSize = RX_MAX_LEN;							// �������ݵĳ���
	DMA_InitStructure.DMA_Channel = USART_RX_DMA_CHANNEL;					// ���ڽ���ͨ��
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;					// DMA���˷������赽�洢��
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Rec1;					// �洢����ַ
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				// �洢��ͻ��ģʽѡ�񣺵���ģʽ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			// �洢�����ݿ�ȣ��ֽ�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// ʹ�ܴ洢����ַ�Զ���������
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							// DMA ����ģʽѡ��ѭ������
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_BASE;				// �����ַ
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		// ����ͻ��ģʽѡ�񣺵���ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ�ȣ��ֽ�
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		// ��ֹ�����ַ�Զ���������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					// ������������������ȼ����е�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;					/* FIFO����������� */
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;

	DMA_Init(USART_RX_DMA_STREAM, &DMA_InitStructure);
	DMA_Cmd(USART_RX_DMA_STREAM, ENABLE); // ����ʹ��
}

// USART1��ʼ������
void uart_init1(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* ʹ��ʱ�� */
	RCC_AHB1PeriphClockCmd(USART_TX_GPIO_CLK | USART_RX_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(USART_CLK, ENABLE);
	/* GPIO������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	 // ���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	 // �������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // ������  ������
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; // ����

	GPIO_InitStructure.GPIO_Pin = USART_TX_PIN; // ���÷�������
	GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART_RX_PIN; // ���ý�������
	GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStructure);
	/* ����Ӧ��IO�����ӵ����裬��ʼ�������ù��� */
	GPIO_PinAFConfig(USART_TX_GPIO_PORT, USART_TX_SOURCE, USART_TX_AF);
	GPIO_PinAFConfig(USART_RX_GPIO_PORT, USART_RX_SOURCE, USART_RX_AF);

	/* USART1��������� */
	USART_InitStructure.USART_BaudRate = bound;										// ������115200
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��ʹ��Ӳ����
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					// USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ���
	USART_InitStructure.USART_Parity = USART_Parity_No;								// У��λѡ�񣺲�ʹ��У��
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// ֹͣλ��1��ֹͣλ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�(����λ+У��λ)��8
	USART_Init(USART, &USART_InitStructure);

	// �ж�����	/* ����USARTΪ�ж�Դ */
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	//	USART_TX_DMA_Config(); // DMA��������
	USART_RX_DMA_Config(); // DMA��������

	USART_ITConfig(USART, USART_IT_IDLE, ENABLE); // ���������ж�
	USART_DMACmd(USART, USART_DMAReq_Rx, ENABLE); // ʹ��DMA����
	USART_Cmd(USART, ENABLE);					  // ʹ�ܴ���
}

// �����жϷ�����
void USART1_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint16_t temp; // ��ȡDMA��ʣ������ݳ���

	if (USART_GetFlagStatus(USART, USART_FLAG_IDLE) != RESET)
	{
		temp = USART1->SR;
		temp = USART1->DR; // ���IDLE�жϡ�

		DMA_Cmd(USART_RX_DMA_STREAM, DISABLE);					 // ʧ��DMA����
		temp = DMA_GetCurrDataCounter(USART_RX_DMA_STREAM);		 // ��ȡʣ��Ľ������ݵ���
		DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5);			 // �����־λ
		DMA_SetCurrDataCounter(USART_RX_DMA_STREAM, RX_MAX_LEN); // ������Ҫ���յ���

		PC_RxLen = RX_MAX_LEN - temp; // ���յ������������ݳ���

		union HEXINT rec_len;

		mymemset(rec_len.bnum, 0, sizeof(rec_len.bnum));
		rec_len.bnum[1] = Rec[1];
		rec_len.bnum[0] = Rec[2];

		rec_len.num += 3; // ���¼����������֡����

		if ((Rec[0] == 0xA1) && (PC_RxLen == rec_len.num)) // �жϽ�������֡֡ͷ�����ݳ���
		{
			mymemcpy(PC_RxBuf, Rec, PC_RxLen); // ���յ�PC�����������ݷ���PC_RxBuf

			uint32_t uart0_msg_q = PC_Type;

			xQueueSendFromISR(Message_Queue, (void *)&uart0_msg_q, &xHigherPriorityTaskWoken); // ������з�������
		}

		DMA_Cmd(USART_RX_DMA_STREAM, ENABLE); // ʹ��DMA����
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // �����Ҫ�Ļ�����һ�������л�
}

/********************************************************/
