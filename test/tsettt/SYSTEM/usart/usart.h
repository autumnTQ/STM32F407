#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "malloc.h"	  
#include   <stdio.h>

//********************************************************************************	  	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init1(uint32_t bound);
void uart_init2(uint32_t bound); 
void uart_init4(uint32_t bound);

/*******************************************/
//         测试串口1使用DMA进行通信
/*******************************************/
#define USART                    USART1
#define USART_CLK                RCC_APB2Periph_USART1
//*******************************************************************************
///* 串口Tx——PA9   PB6*/
//#define USART_TX_PIN             GPIO_Pin_6
//#define USART_TX_GPIO_PORT		 GPIOB
//#define USART_TX_GPIO_CLK		 RCC_AHB1Periph_GPIOB
//#define USART_TX_SOURCE			 GPIO_PinSource6
//#define USART_TX_AF				 GPIO_AF_USART1
// 
///* 串口Rx——PA10  PB7*/
//#define USART_RX_PIN  			 GPIO_Pin_7
//#define USART_RX_GPIO_PORT		 GPIOB 
//#define USART_RX_GPIO_CLK		 RCC_AHB1Periph_GPIOB
//#define USART_RX_SOURCE			 GPIO_PinSource7
//#define USART_RX_AF				 GPIO_AF_USART1
//**************************************************************************  
/* DMA */
#define USART_DR_BASE            (USART1_BASE+0x04)
 
#define USART_TX_DMA_CLK		 RCC_AHB1Periph_DMA2
#define USART_TX_DMA_CHANNEL	 DMA_Channel_4
#define	USART_TX_DMA_STREAM		 DMA2_Stream7
 
#define USART_RX_DMA_CLK		 RCC_AHB1Periph_DMA2
#define USART_RX_DMA_CHANNEL	 DMA_Channel_4
#define	USART_RX_DMA_STREAM		 DMA2_Stream5


//*********************************************************************
/* 串口Tx——PA9   PB6*/
#define USART_TX_PIN               GPIO_Pin_9
#define USART_TX_GPIO_PORT		 GPIOA
#define USART_TX_GPIO_CLK		     RCC_AHB1Periph_GPIOA
#define USART_TX_SOURCE			 GPIO_PinSource9
#define USART_TX_AF				 GPIO_AF_USART1
 
/* 串口Rx——PA10  PB7*/
#define USART_RX_PIN  			 GPIO_Pin_10
#define USART_RX_GPIO_PORT		 GPIOA
#define USART_RX_GPIO_CLK		 RCC_AHB1Periph_GPIOA
#define USART_RX_SOURCE			 GPIO_PinSource10
#define USART_RX_AF				 GPIO_AF_USART1
//*********************************************************************
#define RX_MAX_LEN				 255 
 
#endif


