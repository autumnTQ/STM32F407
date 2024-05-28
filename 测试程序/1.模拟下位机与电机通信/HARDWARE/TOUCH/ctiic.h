/**
 ****************************************************************************************************
 * @file        ctiic.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       电容触摸屏IIC驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 APM32F407最小系统板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __CTIIC_H
#define __CTIIC_H

#include "sys.h"

/* 引脚定义 */
#define CT_IIC_SCL_GPIO_PORT            GPIOB
#define CT_IIC_SCL_GPIO_PIN             GPIO_PIN_0
#define CT_IIC_SCL_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

#define CT_IIC_SDA_GPIO_PORT            GPIOF
#define CT_IIC_SDA_GPIO_PIN             GPIO_PIN_11
#define CT_IIC_SDA_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOF); }while(0)

/* IO操作 */
#define CT_IIC_SCL(x)                   do{ x ?                                                         \
                                            GPIO_SetBit(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN) :    \
                                            GPIO_ResetBit(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN);   \
                                        }while(0)

#define CT_IIC_SDA(x)                   do{ x ?                                                         \
                                            GPIO_SetBit(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN) :    \
                                            GPIO_ResetBit(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN);   \
                                        }while(0)

#define CT_IIC_SDA_READ                 GPIO_ReadInputBit(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN)

/* 函数声明 */
void ct_iic_init(void);                 /* 初始化触摸屏IIC */
void ct_iic_start(void);                /* 产生触摸屏IIC起始信号 */
void ct_iic_stop(void);                 /* 产生触摸屏IIC停止信号 */
uint8_t ct_iic_wait_ack(void);          /* 等待应答信号 */
void ct_iic_ack(void);                  /* 产生ACK应答 */
void ct_iic_nack(void);                 /* 不产生ACK应答 */
void ct_iic_send_byte(uint8_t data);    /* 触摸屏IIC发送一个字节 */
uint8_t ct_iic_read_byte(uint8_t ack);  /* 触摸屏IIC读取一个字节 */

#endif
