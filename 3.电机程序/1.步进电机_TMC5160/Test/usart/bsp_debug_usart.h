#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f10x.h"
#include <stdio.h>

//串口接收缓冲数组大小
#define UART_RX_BUFFER_SIZE 256 

extern unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
extern uint8_t receive_cmd;


void uart_FlushRxBuffer(void);
void Usart_SendByte(uint8_t str);
void Usart_SendString(uint8_t *str);
void DEBUG_USART_Config(uint16_t btl);


#endif /* __USART1_H */
