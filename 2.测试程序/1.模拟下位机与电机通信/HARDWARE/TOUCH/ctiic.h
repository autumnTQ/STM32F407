/**
 ****************************************************************************************************
 * @file        ctiic.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       ���ݴ�����IIC��������
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

#ifndef __CTIIC_H
#define __CTIIC_H

#include "sys.h"

/* ���Ŷ��� */
#define CT_IIC_SCL_GPIO_PORT            GPIOB
#define CT_IIC_SCL_GPIO_PIN             GPIO_PIN_0
#define CT_IIC_SCL_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

#define CT_IIC_SDA_GPIO_PORT            GPIOF
#define CT_IIC_SDA_GPIO_PIN             GPIO_PIN_11
#define CT_IIC_SDA_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOF); }while(0)

/* IO���� */
#define CT_IIC_SCL(x)                   do{ x ?                                                         \
                                            GPIO_SetBit(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN) :    \
                                            GPIO_ResetBit(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN);   \
                                        }while(0)

#define CT_IIC_SDA(x)                   do{ x ?                                                         \
                                            GPIO_SetBit(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN) :    \
                                            GPIO_ResetBit(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN);   \
                                        }while(0)

#define CT_IIC_SDA_READ                 GPIO_ReadInputBit(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN)

/* �������� */
void ct_iic_init(void);                 /* ��ʼ��������IIC */
void ct_iic_start(void);                /* ����������IIC��ʼ�ź� */
void ct_iic_stop(void);                 /* ����������IICֹͣ�ź� */
uint8_t ct_iic_wait_ack(void);          /* �ȴ�Ӧ���ź� */
void ct_iic_ack(void);                  /* ����ACKӦ�� */
void ct_iic_nack(void);                 /* ������ACKӦ�� */
void ct_iic_send_byte(uint8_t data);    /* ������IIC����һ���ֽ� */
uint8_t ct_iic_read_byte(uint8_t ack);  /* ������IIC��ȡһ���ֽ� */

#endif
