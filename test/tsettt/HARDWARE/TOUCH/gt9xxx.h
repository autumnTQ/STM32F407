/**
 ****************************************************************************************************
 * @file        gt9xxx.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       电容触摸屏-GT9XXX驱动代码
 * @note        GT系列电容触摸屏IC通用驱动，本代码支持GT9147/GT917S/GT968/GT1151/GT9271等多种触摸屏IC
 *              这些触摸屏IC仅ID不同，具体代码基本不需要做任何修改可通过本代码直接驱动
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

#ifndef __GT9XXX_H
#define __GT9XXX_H

#include "sys.h"

/* GT9XXX引脚定义 */
#define GT9XXX_RST_GPIO_PORT            GPIOC
#define GT9XXX_RST_GPIO_PIN             GPIO_PIN_13
#define GT9XXX_RST_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC); }while(0)

#define GT9XXX_INT_GPIO_PORT            GPIOB
#define GT9XXX_INT_GPIO_PIN             GPIO_PIN_1
#define GT9XXX_INT_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

/* GT9XXX引脚IO操作 */
#define GT9XXX_RST(x)                   do{ x ?                                                         \
                                            GPIO_SetBit(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN) :    \
                                            GPIO_ResetBit(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN);   \
                                        }while(0)

#define GT9XXX_INT                      GPIO_ReadInputBit(GT9XXX_INT_GPIO_PORT, GT9XXX_INT_GPIO_PIN)

/* GT9XXX IIC读写地址定义 */
#define GT9XXX_CMD_WR                   0x28    /* 写地址 */
#define GT9XXX_CMD_RD                   0x29    /* 读地址 */

/* GT9XXX部分寄存器地址定义 */
#define GT9XXX_CTRL_REG                 0x8040  /* GT9XXX控制寄存器 */
#define GT9XXX_CFGS_REG                 0x8047  /* GT9XXX配置起始地址寄存器 */
#define GT9XXX_CHECK_REG                0x80FF  /* GT9XXX校验和寄存器 */
#define GT9XXX_PID_REG                  0x8140  /* GT9XXX产品ID寄存器 */
#define GT9XXX_GSTID_REG                0x814E  /* GT9XXX当前检测到的触摸情况 */
#define GT9XXX_TP1_REG                  0x8150  /* 第一个触摸点数据地址 */
#define GT9XXX_TP2_REG                  0x8158  /* 第二个触摸点数据地址 */
#define GT9XXX_TP3_REG                  0x8160  /* 第三个触摸点数据地址 */
#define GT9XXX_TP4_REG                  0x8168  /* 第四个触摸点数据地址 */
#define GT9XXX_TP5_REG                  0x8170  /* 第五个触摸点数据地址 */
#define GT9XXX_TP6_REG                  0x8178  /* 第六个触摸点数据地址 */
#define GT9XXX_TP7_REG                  0x8180  /* 第七个触摸点数据地址 */
#define GT9XXX_TP8_REG                  0x8188  /* 第八个触摸点数据地址 */
#define GT9XXX_TP9_REG                  0x8190  /* 第九个触摸点数据地址 */
#define GT9XXX_TP10_REG                 0x8198  /* 第十个触摸点数据地址 */

/* 函数声明 */
uint8_t gt9xxx_init(void);          /* 初始化GT9XXX */
uint8_t gt9xxx_scan(uint8_t mode);  /* 扫描GT9XXX触摸屏 */

#endif
