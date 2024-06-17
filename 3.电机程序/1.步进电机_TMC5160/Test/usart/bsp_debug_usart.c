/**
 ******************************************************************************
 * @file    bsp_debug_usart.c
 * @author  fire
 * @version V1.0
 * @date    2016-xx-xx
 * @brief   ʹ�ô���1���ض���c��printf������usart�˿ڣ��жϽ���ģʽ
 ******************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:Ұ�� STM32 F407 ������
 * ��̳    :http://www.firebbs.cn
 * �Ա�    :http://firestm32.taobao.com
 *
 ******************************************************************************
 */

#include "bsp_debug_usart.h"
#include "bsp_pid.h"
#include "protocol.h"

extern _pid pid;
// UART_HandleTypeDef UartHandle;

// ���ڽ�������
unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
// ���ڽ�������ָ��
unsigned char UART_RxPtr;
/* ���������� */
uint8_t receive_cmd = 0;

// ��ս��ջ���
void uart_FlushRxBuffer(void)
{
    UART_RxPtr = 0;
    UART_RxBuffer[UART_RxPtr] = 0;
}

/*****************  �����ַ� **********************/
void Usart_SendByte(uint8_t str)
{
    USART_SendData(USART1, str);
}

/*****************  �����ַ��� **********************/
void Usart_SendString(uint8_t *str)
{

    b485Send(1); // ��485��Ϊ����
    unsigned int k = 0;
    do
    {
        USART_SendData(USART1, (uint16_t)&str);
        k++;
    } while (*(str + k) != '\0');
    b485Send(0); // ��485��Ϊ����
}

/// �ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
int fputc(int c, FILE *fp)
{
    b485Send(1); // ��485��Ϊ����

    USART_SendData(USART1, (u8)c); // ���͵��ֽ�����
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // �ȴ��������

    return (c); // �����ַ�
}

/**
 * @brief �����жϷ�����
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
