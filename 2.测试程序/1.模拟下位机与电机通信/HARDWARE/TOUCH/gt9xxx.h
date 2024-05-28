/**
 ****************************************************************************************************
 * @file        gt9xxx.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       ���ݴ�����-GT9XXX��������
 * @note        GTϵ�е��ݴ�����ICͨ��������������֧��GT9147/GT917S/GT968/GT1151/GT9271�ȶ��ִ�����IC
 *              ��Щ������IC��ID��ͬ����������������Ҫ���κ��޸Ŀ�ͨ��������ֱ������
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

#ifndef __GT9XXX_H
#define __GT9XXX_H

#include "sys.h"

/* GT9XXX���Ŷ��� */
#define GT9XXX_RST_GPIO_PORT            GPIOC
#define GT9XXX_RST_GPIO_PIN             GPIO_PIN_13
#define GT9XXX_RST_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC); }while(0)

#define GT9XXX_INT_GPIO_PORT            GPIOB
#define GT9XXX_INT_GPIO_PIN             GPIO_PIN_1
#define GT9XXX_INT_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

/* GT9XXX����IO���� */
#define GT9XXX_RST(x)                   do{ x ?                                                         \
                                            GPIO_SetBit(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN) :    \
                                            GPIO_ResetBit(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN);   \
                                        }while(0)

#define GT9XXX_INT                      GPIO_ReadInputBit(GT9XXX_INT_GPIO_PORT, GT9XXX_INT_GPIO_PIN)

/* GT9XXX IIC��д��ַ���� */
#define GT9XXX_CMD_WR                   0x28    /* д��ַ */
#define GT9XXX_CMD_RD                   0x29    /* ����ַ */

/* GT9XXX���ּĴ�����ַ���� */
#define GT9XXX_CTRL_REG                 0x8040  /* GT9XXX���ƼĴ��� */
#define GT9XXX_CFGS_REG                 0x8047  /* GT9XXX������ʼ��ַ�Ĵ��� */
#define GT9XXX_CHECK_REG                0x80FF  /* GT9XXXУ��ͼĴ��� */
#define GT9XXX_PID_REG                  0x8140  /* GT9XXX��ƷID�Ĵ��� */
#define GT9XXX_GSTID_REG                0x814E  /* GT9XXX��ǰ��⵽�Ĵ������ */
#define GT9XXX_TP1_REG                  0x8150  /* ��һ�����������ݵ�ַ */
#define GT9XXX_TP2_REG                  0x8158  /* �ڶ������������ݵ�ַ */
#define GT9XXX_TP3_REG                  0x8160  /* ���������������ݵ�ַ */
#define GT9XXX_TP4_REG                  0x8168  /* ���ĸ����������ݵ�ַ */
#define GT9XXX_TP5_REG                  0x8170  /* ��������������ݵ�ַ */
#define GT9XXX_TP6_REG                  0x8178  /* ���������������ݵ�ַ */
#define GT9XXX_TP7_REG                  0x8180  /* ���߸����������ݵ�ַ */
#define GT9XXX_TP8_REG                  0x8188  /* �ڰ˸����������ݵ�ַ */
#define GT9XXX_TP9_REG                  0x8190  /* �ھŸ����������ݵ�ַ */
#define GT9XXX_TP10_REG                 0x8198  /* ��ʮ�����������ݵ�ַ */

/* �������� */
uint8_t gt9xxx_init(void);          /* ��ʼ��GT9XXX */
uint8_t gt9xxx_scan(uint8_t mode);  /* ɨ��GT9XXX������ */

#endif
