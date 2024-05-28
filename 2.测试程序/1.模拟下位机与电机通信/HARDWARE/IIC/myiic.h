/**
 ****************************************************************************************************
 * @file        myiic.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       IIC��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 * 
 * ʵ��ƽ̨:����ԭ�� APM32F407��Сϵͳ��
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __MYIIC_H
#define __MYIIC_H

#include "sys.h"

/* ���Ŷ��� */
#define IIC_SCL_GPIO_PORT           GPIOB
#define IIC_SCL_GPIO_PIN            GPIO_PIN_8
#define IIC_SCL_GPIO_CLK_ENABLE()   do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

#define IIC_SDA_GPIO_PORT           GPIOB
#define IIC_SDA_GPIO_PIN            GPIO_PIN_9
#define IIC_SDA_GPIO_CLK_ENABLE()   do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

/* IO���� */
#define IIC_SCL(x)                  do{ x ?                                                 \
                                        GPIO_SetBit(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN) :  \
                                        GPIO_ResetBit(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN); \
                                    }while(0)

#define IIC_SDA(x)                  do{ x ?                                                 \
                                        GPIO_SetBit(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN) :  \
                                        GPIO_ResetBit(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN); \
                                    }while(0)

#define IIC_SDA_READ                GPIO_ReadInputBit(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN)

/* �������� */
void iic_init(void);                /* ��ʼ��IIC */
void iic_start(void);               /* ����IIC��ʼ�ź� */
void iic_stop(void);                /* ����IICֹͣ�ź� */
uint8_t iic_wait_ack(void);         /* �ȴ�Ӧ���ź� */
void iic_ack(void);                 /* ����ACKӦ�� */
void iic_nack(void);                /* ������ACKӦ�� */
void iic_send_byte(uint8_t data);   /* IIC����һ���ֽ� */
uint8_t iic_read_byte(uint8_t ack); /* IIC��ȡһ���ֽ� */

#endif
