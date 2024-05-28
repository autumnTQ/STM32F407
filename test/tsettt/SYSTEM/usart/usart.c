#include "sys.h"
#include "usart.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////
// 如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h" //FreeRTOS使用
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "common.h"
#endif
//////////////////////////////////////////////////////////////////////////////////
// 串口使用配置
// USART1  优先级 5     ------->用于
// USART2  优先级 6     ------->用于与韦恩斯电机和推杆通信
// UART4   优先级 5     ------->用于与上位机进行通信
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

extern uint8_t Rec[RX_MAX_LEN];	 // 接收、发送数据缓存区
extern uint8_t Rec1[RX_MAX_LEN]; // 接收、发送数据缓存区

extern QueueHandle_t Message_Queue;		  // 信息队列句柄
extern QueueHandle_t Motor_Message_Queue; // 电机消息队列句柄
extern int g_MotorFlag;					  //
//////////////////////////////////////////////////////////////////
// 加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}

// 重定义fputc函数
#define Serial_DEBUG 1 // 串口调试打印 1：允许答应  0：不允许打印

int fputc(int ch, FILE *f)
{
	if (Serial_DEBUG)
	{
		while ((UART5->SR & 0X40) == 0)
			; // 循环发送,直到发送完毕
		UART5->DR = (u8)ch;
		return ch;
	}
}
#endif

// 初始化IO 串口2
// bound:波特率
void uart_init2(uint32_t bound)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // 使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 使能USART2时钟

	// 串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // GPIOA3复用为USART2

	// USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		   // 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   // 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		   // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		   // 上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   // 初始化PA2，PA3

	// USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式
	USART_Init(USART2, &USART_InitStructure);										// 初始化串口2

	USART_Cmd(USART2, ENABLE); // 使能串口2

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启相关中断

	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		  // 串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6; // 抢占优先级7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 子优先级0
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器、
}

// 初始化IO 串口4  用于给推杆发送指令  推杆波特率默认115200
// bound:波特率   PA0_TX   PA1_RX
void uart_init4(uint32_t bound)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); // 使能USART3时钟

	// 串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4); // GPIOA0复用为UART4
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4); // GPIOA1复用为UART4

	// UART4端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  // 复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	  // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // 上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			  // 初始化PB10，PB11

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; // 输入引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式
	USART_Init(UART4, &USART_InitStructure);										// 初始化串口4

	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		  // 串口4中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 子优先级0
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器、

	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); // 开启空闲中断
	USART_Cmd(UART4, ENABLE);					  // 使能串口4
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE); // 使能DMA接收

	// DMA配置
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); // 使能DMA1时钟

	DMA_DeInit(DMA1_Stream2); // 复位初始化DMA数据流
	// 确保DMA数据流复位完成  //当所读取 EN 位的值为 0 时，才表示可以配置数据流。因此在开始任何数据流配置之前，需要等待 EN 位置 0
	while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE)
	{
	}

	/* 配置接收 */
	DMA_InitStructure.DMA_BufferSize = RX_MAX_LEN;							// 接收数据的长度
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;							// 串口接收通道
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;					// DMA搬运方向：外设到存储器
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Rec;					// 存储器地址
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				// 存储器突发模式选择：单次模式
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			// 存储器数据宽度：字节
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// 使能存储器地址自动递增功能
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							// DMA 传输模式选择：循环接收
	DMA_InitStructure.DMA_PeripheralBaseAddr = UART4_BASE + 0x04;			// 外设地址
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		// 外设突发模式选择：单次模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度：字节
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		// 禁止外设地址自动递增功能
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					// 软件设置数据流的优先级：中等
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;					/* FIFO不用随便配置 */
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;

	DMA_Init(DMA1_Stream2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream2, ENABLE); // 接收使能
}

// 串口2中断服务程序
void USART2_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint32_t uart2_msg_q = 0;

	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 接收中断
	{
		uart2_t.Tmp = USART_ReceiveData(USART2); // 读取接收到的数据

		if (uart2_t.Flag == 1)
		{
			uart2_t.RxBuf[uart2_t.RxCnt++] = uart2_t.Tmp;
			if (uart2_t.RxCnt >= (uart2_t.RxBuf[2] + 5)) // 输入的数据必须满足这个长度
			{
				uart2_t.Flag = 0;

				if (uart2_t.RxBuf[2] == 0x02) // 推杆反馈的数据
				{
					mymemcpy(Push_RxBuf, uart2_t.RxBuf, uart2_t.RxCnt); // 接收到推杆发过来的数据放在Push_RxBuf
					uart2_msg_q = Push_Rx;
				}
				else if (uart2_t.RxBuf[2] == 0x0C) // 查询电机状态反馈的数据
				{

					unsigned short iCrc = CrcVerifice(PC_RxBuf, PC_RxLen - 2);

					if (((iCrc & 0Xff) == Rec[PC_RxLen - 2]) | (((iCrc >> 8) & 0xff) == Rec[PC_RxLen - 1]))
					{
						mymemcpy(Motor_RxBuf, uart2_t.RxBuf, uart2_t.RxCnt); // 接收到电机发过来的数据放在Motor_RxBuf
						uart2_msg_q = Motor_Rx;
					}
				}
				else if (uart2_t.RxBuf[2] == 0x12) // 查询电机配置反馈的数据
				{
					mymemcpy(Motor_RxBuf, uart2_t.RxBuf, uart2_t.RxCnt); // 接收到电机发过来的数据放在Motor_RxBuf
					uart2_msg_q = MotorConfigure_Rx;
				}
				else if (uart2_t.RxBuf[2] == 0x05) // 查询温度控制板反馈的数据
				{
					mymemcpy(Motor_RxBuf, uart2_t.RxBuf, uart2_t.RxCnt); // 接收到电机发过来的数据放在Motor_RxBuf
					uart2_msg_q = Temp_Type;
				}

				uart2_t.RxCnt = 0;
				mymemset(uart2_t.RxBuf, 0, sizeof(uart2_t.RxBuf));
				xQueueSendFromISR(Motor_Message_Queue, (void *)&uart2_msg_q, &xHigherPriorityTaskWoken); // 向队列中发送数据
			}
		}
		else
		{
			switch (uart2_t.Fsm)
			{
			case 0:
				if (uart2_t.Tmp == PC_RxBuf[8]) // 判断电机ID号
				{
					uart2_t.Fsm = 1;
					mymemset(uart2_t.RxBuf, 0, sizeof(uart2_t.RxBuf));
					uart2_t.RxBuf[0] = uart2_t.Tmp;
				}
				break;
			case 1:
				if ((uart2_t.Tmp == 0x04) || (uart2_t.Tmp == 0x03) || (uart2_t.Tmp == 0x10)) // 判断接收功能码
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
				if (uart2_t.Tmp == g_MotorFlag) // 判断读取信息寄存器个数
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
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // 如果需要的话进行一次任务切换
	}
}

// 串口4中断服务程序  接收推杆反馈的数据
void UART4_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint16_t temp; // 读取DMA的剩余的数据长度
	uint32_t uart0_msg_q = 0;

	if (USART_GetFlagStatus(UART4, USART_FLAG_IDLE) != RESET)
	{
		temp = UART4->SR;
		temp = UART4->DR; // 清除IDLE中断。

		DMA_Cmd(DMA1_Stream2, DISABLE);					  // 失能DMA接收
		temp = DMA_GetCurrDataCounter(DMA1_Stream2);	  // 获取剩余的接收数据的量
		DMA_ClearFlag(DMA1_Stream2, DMA_FLAG_TCIF4);	  // 清除标志位
		DMA_SetCurrDataCounter(DMA1_Stream2, RX_MAX_LEN); // 设置需要接收的量

		PC_RxLen = RX_MAX_LEN - temp; // 接收到的完整的数据长度

		mymemcpy(PC_RxBuf, Rec, PC_RxLen); // 接收到PC发过来的数据放在PC_RxBuf

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
//                  测试串口1使用DMA进行通信
/**********************************************************/

static void USART_RX_DMA_Config(void) // USART1_RX------DMA2_Stream5  DMA_Channel_4
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(USART_RX_DMA_CLK, ENABLE); // 使能DMA2时钟

	DMA_DeInit(USART_RX_DMA_STREAM); // 复位初始化DMA数据流
	// 确保DMA数据流复位完成  //当所读取 EN 位的值为 0 时，才表示可以配置数据流。因此在开始任何数据流配置之前，需要等待 EN 位置 0
	while (DMA_GetCmdStatus(USART_RX_DMA_STREAM) != DISABLE)
	{
	}

	/* 配置接收 */
	DMA_InitStructure.DMA_BufferSize = RX_MAX_LEN;							// 接收数据的长度
	DMA_InitStructure.DMA_Channel = USART_RX_DMA_CHANNEL;					// 串口接收通道
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;					// DMA搬运方向：外设到存储器
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Rec1;					// 存储器地址
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				// 存储器突发模式选择：单次模式
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			// 存储器数据宽度：字节
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// 使能存储器地址自动递增功能
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							// DMA 传输模式选择：循环接收
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_BASE;				// 外设地址
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		// 外设突发模式选择：单次模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度：字节
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		// 禁止外设地址自动递增功能
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					// 软件设置数据流的优先级：中等
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;					/* FIFO不用随便配置 */
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;

	DMA_Init(USART_RX_DMA_STREAM, &DMA_InitStructure);
	DMA_Cmd(USART_RX_DMA_STREAM, ENABLE); // 接收使能
}

// USART1初始化配置
void uart_init1(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* 使能时钟 */
	RCC_AHB1PeriphClockCmd(USART_TX_GPIO_CLK | USART_RX_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(USART_CLK, ENABLE);
	/* GPIO相关配置 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	 // 复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	 // 推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 无上拉  无下拉
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; // 高速

	GPIO_InitStructure.GPIO_Pin = USART_TX_PIN; // 配置发送引脚
	GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART_RX_PIN; // 配置接收引脚
	GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStructure);
	/* 将对应的IO口连接到外设，开始启动复用功能 */
	GPIO_PinAFConfig(USART_TX_GPIO_PORT, USART_TX_SOURCE, USART_TX_AF);
	GPIO_PinAFConfig(USART_RX_GPIO_PORT, USART_RX_SOURCE, USART_RX_AF);

	/* USART1的相关配置 */
	USART_InitStructure.USART_BaudRate = bound;										// 波特率115200
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 不使用硬件流
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					// USART模式控制：同时使能接收和发送
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 校验位选择：不使用校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 停止位：1个停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长(数据位+校验位)：8
	USART_Init(USART, &USART_InitStructure);

	// 中断配置	/* 配置USART为中断源 */
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	//	USART_TX_DMA_Config(); // DMA发送配置
	USART_RX_DMA_Config(); // DMA接收配置

	USART_ITConfig(USART, USART_IT_IDLE, ENABLE); // 开启空闲中断
	USART_DMACmd(USART, USART_DMAReq_Rx, ENABLE); // 使能DMA接收
	USART_Cmd(USART, ENABLE);					  // 使能串口
}

// 串口中断服务函数
void USART1_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint16_t temp; // 读取DMA的剩余的数据长度

	if (USART_GetFlagStatus(USART, USART_FLAG_IDLE) != RESET)
	{
		temp = USART1->SR;
		temp = USART1->DR; // 清除IDLE中断。

		DMA_Cmd(USART_RX_DMA_STREAM, DISABLE);					 // 失能DMA接收
		temp = DMA_GetCurrDataCounter(USART_RX_DMA_STREAM);		 // 获取剩余的接收数据的量
		DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5);			 // 清除标志位
		DMA_SetCurrDataCounter(USART_RX_DMA_STREAM, RX_MAX_LEN); // 设置需要接收的量

		PC_RxLen = RX_MAX_LEN - temp; // 接收到的完整的数据长度

		union HEXINT rec_len;

		mymemset(rec_len.bnum, 0, sizeof(rec_len.bnum));
		rec_len.bnum[1] = Rec[1];
		rec_len.bnum[0] = Rec[2];

		rec_len.num += 3; // 重新计算接收数据帧长度

		if ((Rec[0] == 0xA1) && (PC_RxLen == rec_len.num)) // 判断接收数据帧帧头和数据长度
		{
			mymemcpy(PC_RxBuf, Rec, PC_RxLen); // 接收到PC发过来的数据放在PC_RxBuf

			uint32_t uart0_msg_q = PC_Type;

			xQueueSendFromISR(Message_Queue, (void *)&uart0_msg_q, &xHigherPriorityTaskWoken); // 向队列中发送数据
		}

		DMA_Cmd(USART_RX_DMA_STREAM, ENABLE); // 使能DMA接收
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // 如果需要的话进行一次任务切换
}

/********************************************************/
