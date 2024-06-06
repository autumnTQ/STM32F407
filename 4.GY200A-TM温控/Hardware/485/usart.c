
#include "usart.h"

/* 串口3硬件层定义 */
#define USART3_RCC        RCC_APB2Periph_GPIOB
#define USART3_PORT       GPIOB
#define USART3_TX_PIN     GPIO_Pin_10
#define USART3_RX_PIN     GPIO_Pin_11
/* RS485发送使能端硬件层定义 */
#define RS485_TXEN3_RCC   RCC_APB2Periph_GPIOE
#define RS485_TXEN3_PORT  GPIOE
#define RS485_TXEN3_PIN   GPIO_Pin_15
#define RS485_TX_EN3()    RS485_TXEN3_PORT->BSRR = RS485_TXEN3_PIN
#define RS485_RX_EN3()    RS485_TXEN3_PORT->BRR = RS485_TXEN3_PIN



/**
 * @brief  串口3初始化
 * @param  baud：波特率
 * @retval None
 */
void usart3_init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(USART3_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(RS485_TXEN3_RCC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = USART3_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART3_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = RS485_TXEN3_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RS485_TXEN3_PORT, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);
    USART_ClearFlag(USART3, USART_FLAG_TC);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RS485_RX_EN3();
}


/**
 * @brief  串口3工作模式设置
 * @param  mode：TX/RX
 * @retval None
 */
void usart3_mode(uint8_t mode)
{
    if (mode == TX)
    {
        RS485_TX_EN3();
    }
    else
    {
        RS485_RX_EN3();
    }
}


/**
 * @brief  串口3发送一串数据
 * @param  data：数据缓冲区
 * @param  len：数据长度
 * @retval None
 */
void usart3_send(uint8_t *data, uint8_t len)
{
    uint8_t i;

    for (i = 0; i < len; i++)
    {
        USART_SendData(USART3, data[i]);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }

    USART_ClearFlag(USART3, USART_FLAG_TC);
}



/**
 * @brief  串口3中断服务程序
 * @param  None
 * @retval None
 */
void USART3_IRQHandler(void)
{
    uint8_t data;

    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        data = USART_ReceiveData(USART3);
        
        /* 一帧数据接收完成 */
        if (mb_usart3_t.rx_end_flg == 1)
        {
            return;
        }

        /* 接收数据长度小于接收缓冲区 */
        if (mb_usart3_t.rx_byte_cnt < MB_BUF_SIZE)
        {
            /* 把接收数据放到接收缓冲区 */
            mb_usart3_t.rx_buf[mb_usart3_t.rx_byte_cnt++] = data;
            /* 定时清零 */
            mb_usart3_t.rx_timeout = 0;
        }

        /* 收到一帧数据的第一个字节 */
        if (mb_usart3_t.rx_byte_cnt == 1)
        {
            /* 启动接收定时 */
            mb_usart3_t.rx_timer_flg = 1;
        }
    }
}

///重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART3, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART3);
}


/*****END OF FILE*****/
