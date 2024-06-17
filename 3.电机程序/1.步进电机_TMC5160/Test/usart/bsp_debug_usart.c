/**
 ******************************************************************************
 * @file    bsp_debug_usart.c
 * @author  fire
 * @version V1.0
 * @date    2016-xx-xx
 * @brief   使用串口1，重定向c库printf函数到usart端口，中断接收模式
 ******************************************************************************
 * @attention
 *
 * 实验平台:野火 STM32 F407 开发板
 * 论坛    :http://www.firebbs.cn
 * 淘宝    :http://firestm32.taobao.com
 *
 ******************************************************************************
 */

#include "bsp_debug_usart.h"
#include "bsp_pid.h"
#include "protocol.h"

extern _pid pid;
// UART_HandleTypeDef UartHandle;

// 串口接收数组
unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
// 串口接收数组指针
unsigned char UART_RxPtr;
/* 命令接收完成 */
uint8_t receive_cmd = 0;

// 清空接收缓冲
void uart_FlushRxBuffer(void)
{
    UART_RxPtr = 0;
    UART_RxBuffer[UART_RxPtr] = 0;
}

/*****************  发送字符 **********************/
void Usart_SendByte(uint8_t str)
{
    USART_SendData(USART1, str);
}

/*****************  发送字符串 **********************/
void Usart_SendString(uint8_t *str)
{

    b485Send(1); // 将485设为发送
    unsigned int k = 0;
    do
    {
        USART_SendData(USART1, (uint16_t)&str);
        k++;
    } while (*(str + k) != '\0');
    b485Send(0); // 将485设为接收
}

/// 重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
int fputc(int c, FILE *fp)
{
    b485Send(1); // 将485设为发送

    USART_SendData(USART1, (u8)c); // 发送单字节数据
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // 等待发送完毕

    return (c); // 返回字符
}

/**
 * @brief 串口中断服务函数
 */
// void USART1_IRQHandler(void)
//{
//     uint8_t ucTemp;
//     if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//     {
//         ucTemp = USART_ReceiveData(USART1);
//         protocol_data_recv(&ucTemp, 1);
//     }
// }

/*********************************************END OF FILE**********************/
