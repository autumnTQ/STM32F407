/*
 * Copyright (c) 2006-2100, Tommy Guo
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-27     Tommy        first version
 */
 
#ifndef __USART_H__
#define __USART_H__

#include "main.h"
#include "mbslave.h"
#define TX  0
#define RX  1


void usart3_init(uint32_t baud);

void usart3_mode(uint8_t mode);

void usart3_send(uint8_t *data, uint8_t len);

#endif
/*****END OF FILE*****/
