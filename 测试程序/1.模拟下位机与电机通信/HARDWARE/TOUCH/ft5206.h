/**
 ****************************************************************************************************
 * @file        ft5206.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       7寸电容触摸屏驱动代码
 * @note        支持FT5206/FT5426
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

#ifndef __FT5206_H
#define __FT5206_H

#include "sys.h"

/* FT5206引脚定义 */
#define FT5206_RST_GPIO_PORT            GPIOC
#define FT5206_RST_GPIO_PIN             GPIO_PIN_13
#define FT5206_RST_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC); }while(0)

#define FT5206_INT_GPIO_PORT            GPIOB
#define FT5206_INT_GPIO_PIN             GPIO_PIN_1
#define FT5206_INT_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

/* FT5206引脚IO操作 */
#define FT5206_RST(x)                   do{ x ?                                                         \
                                            GPIO_SetBit(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN) :    \
                                            GPIO_ResetBit(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN);   \
                                        }while(0)

#define FT5206_INT                      GPIO_ReadInputBit(FT5206_INT_GPIO_PORT, FT5206_INT_GPIO_PIN)

/* FT5206 IIC读写地址定义 */
#define FT5206_CMD_WR                   0x70    /* 写地址 */
#define FT5206_CMD_RD                   0x71    /* 读地址 */

/* FT5206部分寄存器地址定义 */
#define FT5206_DEVIDE_MODE              0x00    /* FT5206模式控制寄存器 */
#define FT5206_REG_NUM_FINGER           0x02    /* 触摸状态寄存器 */
#define FT5206_TP1_REG                  0x03    /* 第一个触摸点数据地址 */
#define FT5206_TP2_REG                  0x09    /* 第二个触摸点数据地址 */
#define FT5206_TP3_REG                  0x0F    /* 第三个触摸点数据地址 */
#define FT5206_TP4_REG                  0x15    /* 第四个触摸点数据地址 */
#define FT5206_TP5_REG                  0x1B    /* 第五个触摸点数据地址 */ 
#define FT5206_ID_G_LIB_VERSION         0xA1    /* 版本 */
#define FT5206_ID_G_MODE                0xA4    /* FT5206中断模式控制寄存器 */
#define FT5206_ID_G_THGROUP             0x80    /* 触摸有效值设置寄存器 */
#define FT5206_ID_G_PERIODACTIVE        0x88    /* 激活状态周期设置寄存器 */

/* 函数声明 */
uint8_t ft5206_init(void);          /* 初始化FT5206 */
uint8_t ft5206_scan(uint8_t mode);  /* 扫描FT5206读触摸屏 */

#endif
