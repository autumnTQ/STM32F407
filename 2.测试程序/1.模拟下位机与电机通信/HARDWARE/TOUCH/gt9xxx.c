/**
 ****************************************************************************************************
 * @file        gt9xxx.c
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

#include "gt9xxx.h"
#include "ctiic.h"
#include "touch.h"
#include "lcd.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_gpio.h"
#include "./SYSTEM/delay/delay.h"
#include <string.h>

/* 默认支持触摸屏5点触摸
 * 除了GT9271支持10点触摸，其余只支持5点触摸
 */
static uint8_t g_gt_tnum = 5;

/**
 * @brief       从GT9XXX读出一次数据
 * @param       reg: 起始寄存器地址
 * @param       buf: 数据保存起始地址
 * @param       len: 待读出数据的长度
 * @retval      无
 */
static void gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);                        /* 发送写命令 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);                             /* 发送寄存器高8位地址 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);                           /* 发送寄存器低8位地址 */
    ct_iic_wait_ack();
    
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_RD);                        /* 发送读命令 */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)                               /* 循环读出数据 */
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);
    }
    
    ct_iic_stop();
}

/**
 * @brief       从GT9XXX写入一次数据
 * @param       reg: 起始寄存器地址
 * @param       buf: 待写入数据的起始地址
 * @param       len: 待写入数据的长度
 * @retval      写入结果
 *              0: 成功
 *              1: 失败
 */
static uint8_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ret = 0;
    
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);    /* 发送写命令 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);         /* 发送寄存器高8位地址 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);       /* 发送寄存器低8位地址 */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)           /* 循环写入数据 */
    {
        ct_iic_send_byte(buf[i]);
        ret = ct_iic_wait_ack();
        if (ret)
        {
            break;
        }
    }
    ct_iic_stop();
    
    return ret;
}

/**
 * @brief       初始化GT9XXX
 * @param       无
 * @retval      初始化结果
 *              0: 成功
 *              1: 失败
 */
uint8_t gt9xxx_init(void)
{
    GPIO_Config_T gpio_init_struct;
    uint8_t temp[5];
    
    /* 使能时钟 */
    GT9XXX_RST_GPIO_CLK_ENABLE();                           /* 使能GT9XXX RST引脚端口时钟 */
    GT9XXX_INT_GPIO_CLK_ENABLE();                           /* 使能GT9XXX INT引脚端口时钟 */
    
    /* 配置GT9XXX RST引脚 */
    gpio_init_struct.pin = GT9XXX_RST_GPIO_PIN;             /* GT9XXX RST引脚 */
    gpio_init_struct.mode = GPIO_MODE_OUT;                  /* 通用输出模式 */
    gpio_init_struct.speed = GPIO_SPEED_100MHz;             /* 高速 */
    gpio_init_struct.otype = GPIO_OTYPE_PP;                 /* 推挽输出 */
    gpio_init_struct.pupd = GPIO_PUPD_UP;                   /* 上拉 */
    GPIO_Config(GT9XXX_RST_GPIO_PORT, &gpio_init_struct);   /* 配置GT9XXX RST引脚 */
    
    /* 配置GT9XXX INT引脚 */
    gpio_init_struct.pin = GT9XXX_INT_GPIO_PIN;             /* GT9XXX INT引脚 */
    gpio_init_struct.mode = GPIO_MODE_IN;                   /* 输入模式 */
    gpio_init_struct.pupd = GPIO_PUPD_UP;                   /* 上拉 */
    GPIO_Config(GT9XXX_INT_GPIO_PORT, &gpio_init_struct);   /* 配置GT9XXX INT引脚 */
    
    ct_iic_init();                                          /* 初始化触摸屏IIC */
    
    /* 复位GT9XXX */
    GT9XXX_RST(0);                                          /* 使能GT9XXX复位 */
    delay_ms(10);
    GT9XXX_RST(1);                                          /* 释放GT9XXX复位 */
    delay_ms(10);
    
    /* 配置GT9XXX INT引脚 */
    gpio_init_struct.pin = GT9XXX_INT_GPIO_PIN;             /* GT9XXX INT引脚 */
    gpio_init_struct.mode = GPIO_MODE_IN;                   /* 输入模式 */
    gpio_init_struct.pupd = GPIO_PUPD_NOPULL;               /* 禁止上拉/下拉 */
    GPIO_Config(GT9XXX_INT_GPIO_PORT, &gpio_init_struct);   /* 配置GT9XXX INT引脚 */
    
    delay_ms(100);
    gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4);                 /* 读取GT9XXX PID */
    temp[4] = 0;
    if (strcmp((char *)temp, "9271") == 0)                  /* GT9271支持10点触摸 */
    {
        g_gt_tnum = 10;
    }
    
    temp[0] = 0x02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);                /* 软件复位GT9XXX */
    delay_ms(10);
    
    temp[0] = 0x00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);                /* 结束复位，进入读坐标状态 */
    
    return 0;
}

/* GT9XXX读取触摸坐标寄存器 */
static const uint16_t GT9XXX_TPX_TBL[10] = {
    GT9XXX_TP1_REG,
    GT9XXX_TP2_REG,
    GT9XXX_TP3_REG,
    GT9XXX_TP4_REG,
    GT9XXX_TP5_REG,
    GT9XXX_TP6_REG,
    GT9XXX_TP7_REG,
    GT9XXX_TP8_REG,
    GT9XXX_TP9_REG,
    GT9XXX_TP10_REG,
};

/**
 * @brief       扫描GT9XXX触摸屏
 * @note        采用查询方式
 * @param       mode: 电容屏未使用，用于兼容电阻屏
 * @retval      当前触摸状态
 *              0: 无触摸
 *              1: 有触摸
 */
uint8_t gt9xxx_scan(uint8_t mode)
{
    static uint8_t t = 0;
    uint8_t data;
    uint16_t temp;
    uint16_t tempsta;
    uint8_t i;
    uint8_t buf[4];
    uint8_t res = 0;
    
    t++;
    if (((t % 10) == 0) || (t < 10))
    {
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);                                  /* 读取触摸点的状态 */
        if ((mode & 0x80) && ((mode & 0x0F) <= g_gt_tnum))
        {
            data = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &data, 1);                              /* 清标志 */
        }
        
        if ((mode & 0x0F) && ((mode & 0x0F) <= g_gt_tnum))
        {
            temp = 0xFFFF << (mode & 0x0F);                                         /* 将点的个数转换为1的位数，匹配tp_dev.sta定义 */
            tempsta = tp_dev.sta;                                                   /* 保存当前的tp_dev.sta值 */
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            tp_dev.x[g_gt_tnum - 1] = tp_dev.x[0];                                  /* 保存触点0的数据，保存在最后一个上 */
            tp_dev.y[g_gt_tnum - 1] = tp_dev.y[0];
            for (i=0; i<g_gt_tnum; i++)
            {
                if (tp_dev.sta & (1 << i))                                          /* 触摸点有效 */
                {
                    gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);                       /* 读取XY坐标值 */
                    
                    if (lcddev.id == 0x5510)                                        /* 4.3寸800*480 MCU屏 */
                    {
                        if (tp_dev.touchtype & 0x01)                                /* 横屏 */
                        {
                            tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                            tp_dev.x[i] = 800 - (((uint16_t)buf[3] << 8) + buf[2]);
                        }
                        else                                                        /* 竖屏 */
                        {
                            tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                            tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                        }
                    }
                    else if (lcddev.id == 0x4342)                                   /* 4.3寸480*272 RGB屏 */
                    {
                        if (tp_dev.touchtype & 0x01)                                /* 横屏 */
                        {
                            tp_dev.x[i] = (((uint16_t)buf[1] << 8) + buf[0]);
                            tp_dev.y[i] = (((uint16_t)buf[3] << 8) + buf[2]);
                        }
                        else                                                        /* 竖屏 */
                        {
                            tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                            tp_dev.x[i] = 272 - (((uint16_t)buf[3] << 8) + buf[2]);
                        }
                    }
                    else if (lcddev.id == 0x1018)                                   /* 10.1寸1280*800 RGB屏 */
                    {
                        if (tp_dev.touchtype & 0x01)                                /* 横屏 */
                        {
                            tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                            tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];

                        }
                        else                                                        /* 竖屏 */
                        {
                            tp_dev.x[i] = 800 - (((uint16_t)buf[3] << 8) + buf[2]);
                            tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        }
                    }
                }
            }
            res = 1;
            
            if ((tp_dev.x[0] > lcddev.width) || (tp_dev.y[0] > lcddev.height))      /* 非法数据（坐标超出了） */
            {
                if ((mode & 0x0F) > 1)                                              /* 有其他点有数据，则复制第二个触点的数据到第一个触点. */
                {
                    tp_dev.x[0] = tp_dev.x[1];
                    tp_dev.y[0] = tp_dev.y[1];
                    t = 0;                                                          /* 触发一次，则会最少连续监测10次，从而提高命中率 */
                }
                else                                                                /* 非法数据，则忽略此次数据（还原原来的） */
                {
                    tp_dev.x[0] = tp_dev.x[g_gt_tnum - 1];
                    tp_dev.y[0] = tp_dev.y[g_gt_tnum - 1];
                    mode = 0x80;
                    tp_dev.sta = tempsta;                                           /* 恢复tp_dev.sta */
                }
            }
            else
            {
                t = 0;                                                              /* 触发一次，则会最少连续监测10次，从而提高命中率 */
            }
        }
    }
    
    if ((mode & 0x8F) == 0x80)                                                      /* 无触摸点按下 */
    {
        if (tp_dev.sta & TP_PRES_DOWN)                                              /* 之前是被按下的 */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;                                            /* 标记按键松开 */
        }
        else                                                                        /* 之前就没有被按下 */
        {
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
            tp_dev.sta &= 0xE000;                                                   /* 清除点有效标记 */
        }
    }
    
    if (t > 240)
    {
        t = 10;                                                                     /* 重新从10开始计数 */
    }
    
    return res;
}
