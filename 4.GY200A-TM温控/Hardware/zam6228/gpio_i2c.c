/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief GPIO_I2C 驱动
 *
 * 使用GPIO模拟I2C时序
 *
 * \internal
 * \par Modification history
 * - 1.00 23-5-17  hjl, first implementation.
 * \endinternal
 */

#include "gpio_i2c.h"
#include "delay.h"
#include "sys.h"
#include <stdio.h>

/** \brief IO方向设置 */
#define __GPIO_I2C_SDA_IN()   {GPIOE->CRL&=0xFFFF0FFF;GPIOE->CRL|=4<<12;} /** \brief 浮空输入 */
#define __GPIO_I2C_SDA_OUT()  {GPIOE->CRL&=0xFFFF0FFF;GPIOE->CRL|=7<<12;} /** \brief 开漏输出 */


/** \brief IO操作函数 */
#define __GPIO_I2C_SCL         PEout(2)  //SCL
#define __GPIO_I2C_SDA         PEout(3)  //SDA
#define __GPIO_I2C_READ_SDA    PEin(3)   //输入SDA
#define __GPIO_I2C_READ_SCL()  GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) //输入SCL

#define __GPIO_I2C_ACK_SAMPLE_NUM    (250)
#if (__GPIO_I2C_ACK_SAMPLE_NUM < 1) || (__GPIO_I2C_ACK_SAMPLE_NUM > 255)
    #error "__GPIO_I2C_ACK_SAMPLE_NUM setup error!"
#endif

/*******************************************************************************
 Local functions
*******************************************************************************/

/** \brief 产生IIC起始信号 */
static void __gpio_i2c_start(void)
{
    __GPIO_I2C_SDA_OUT();
    __GPIO_I2C_SDA = 1;
    __GPIO_I2C_SCL = 1;
    delay_us(5);
    __GPIO_I2C_SDA = 0;
    delay_us(5);
    __GPIO_I2C_SCL = 0;
}

/** \brief 产生IIC停止信号 */
static void __gpio_i2c_stop(void)
{
    __GPIO_I2C_SDA_OUT();
    __GPIO_I2C_SCL = 0;
    __GPIO_I2C_SDA = 0;
    delay_us(5);
    __GPIO_I2C_SCL = 1;
    delay_us(5);
    __GPIO_I2C_SDA = 1;
}

/**
 * \brief 等待应答信号
 *
 * \retval  0 : 等待成功
 * \retval  1 : 等待失败
 */
static uint8_t __gpio_i2c_wait_ack(void)
{
    uint8_t value = 0;

    __GPIO_I2C_SDA_IN();
    delay_us(5);
    __GPIO_I2C_SCL = 1;
    delay_us(5);
    while (__GPIO_I2C_READ_SDA) {

        value++;
        if (value >= __GPIO_I2C_ACK_SAMPLE_NUM) {
            __gpio_i2c_stop();
            return 1;
        }
    }
    __GPIO_I2C_SCL = 0;

    return 0;
}

/** \brief 产生ACK信号 */
static void __gpio_i2c_send_ack(void)
{
    __GPIO_I2C_SDA_OUT();
    __GPIO_I2C_SDA = 0;
    delay_us(5);
    __GPIO_I2C_SCL = 1;
    delay_us(5);
    __GPIO_I2C_SCL = 0;
}

/** \brief 产生NACK信号 */
static void __gpio_i2c_send_nack(void)
{
    __GPIO_I2C_SDA_OUT();
    __GPIO_I2C_SDA = 1;
    delay_us(5);
    __GPIO_I2C_SCL = 1;
    delay_us(5);
}

/**
 *\brief I2C总线忙等待
 *
 * 等待I2C总线释放为空闲状态。若SCL线为高电平，I2C总线处于空闲状态。
 *
 *\param[in] timeout : 超时时间, 单位us
 *
 *\retval 0 : 等待成功
 *\retval 1 : 等待超时
 */
static uint8_t __gpio_i2c_busy_wait(uint32_t timeout)
{
    uint32_t wait_time = 0;
    
    while (__GPIO_I2C_READ_SCL() == 0) {
        /* 等待时间到或者timeout为0时，直接退出 */
        if (wait_time >= timeout) {
            return 1;
        } else {
            delay_us(1);
            wait_time++;
        }
    }
    
    return 0;
}

/** 
 * \brief I2C发送一个字节
 *
 * \param[in] data : 待发送数据
 */
static void __gpio_i2c_send_byte(const uint8_t data)
{
    uint8_t i = 0;

    __GPIO_I2C_SDA_OUT();
    __GPIO_I2C_SCL = 0;
    for (i = 0;i < 8;i++)
    {
        __GPIO_I2C_SDA = (!!((data << i) & 0x80));
        delay_us(5);
        __GPIO_I2C_SCL = 1;
        delay_us(5);
        __GPIO_I2C_SCL = 0;
    }
}

/**
 * \brief 读1个字节
 *
 * param[out] p_data : 读取到的数据
 * param[in]  ack    : ack=1时，发送ACK，ack=0时，发送nACK
 *
 * \retval 0 : 成功
 * \retval 1 : 失败
 */
static uint8_t __gpio_i2c_read_byte(uint8_t *p_data,
                                    uint8_t  ack)
{
    uint8_t i       = 0;
    uint8_t receive = 0;

    __GPIO_I2C_SDA_IN();
    delay_us(5);
    
    /**
     * \brief 主机尝试拉高SCL线。若没有从机占用总线，则SCL线此时为高电平；
     * 若存在从机占用总线，则SCL线此时为低电平，需要等待从机释放总线才能继续进行通信
     */
    __GPIO_I2C_SCL = 1;
    /* 等待从机释放总线 */
    if (__gpio_i2c_busy_wait(200)) {
        __gpio_i2c_stop();
        return 1;
    }
    delay_us(2);

    /* 正常获取数据 */
    receive <<= 1;
    if (__GPIO_I2C_READ_SDA) {
        receive++;
    }
    __GPIO_I2C_SCL = 0;
    
    for(i = 0;i < 7;i++)
    {
        receive <<= 1;
        delay_us(5);
        __GPIO_I2C_SCL = 1;
        delay_us(5);
        if(__GPIO_I2C_READ_SDA) {
            receive++;
        }
        __GPIO_I2C_SCL = 0;
    }
    if (!ack)
    {
        __gpio_i2c_send_nack();
    } else {
        __gpio_i2c_send_ack();
    }
    
    *p_data = receive;

    return 0;
}


/*******************************************************************************
 Global functions
*******************************************************************************/
uint8_t gpio_i2c_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE );
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;     /** \brief 开漏输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    __GPIO_I2C_SCL = 1;
    __GPIO_I2C_SDA = 1;
    
    return 0;
}


uint8_t gpio_i2c_read(uint8_t      dev_addr, 
                      uint32_t     sub_addr,
                      uint32_t     addr_len,
                      uint8_t     *p_buff,
                      uint32_t     nbytes)
{
    __gpio_i2c_start();

    __gpio_i2c_send_byte(dev_addr & (~0x01));
    if(__gpio_i2c_wait_ack()) {
        return 1;
    }
    
    __gpio_i2c_send_byte(sub_addr);
    if(__gpio_i2c_wait_ack()) {
        return 1;
    }

    __gpio_i2c_start();

    __gpio_i2c_send_byte(dev_addr | 0x01);
    if(__gpio_i2c_wait_ack()) {
        return 1;
    }
    
    while(nbytes) {
        if (nbytes > 1) {
            if (__gpio_i2c_read_byte(p_buff, 1)) {
                return 1;
            }
        } else {
            if (__gpio_i2c_read_byte(p_buff, 0)) {
                return 1;
            }
        }
        p_buff++;
        nbytes--;
    }

    __gpio_i2c_stop();
    
    return 0;
}

uint8_t gpio_i2c_write(uint8_t        dev_addr, 
                       uint32_t       sub_addr,
                       uint32_t       addr_len,
                       const uint8_t *p_buff,
                       uint32_t       nbytes)
{
    __gpio_i2c_start();

    __gpio_i2c_send_byte(dev_addr & (~0x01));
    if(__gpio_i2c_wait_ack()) {
        return 1;
    }

    __gpio_i2c_send_byte(sub_addr);
    if(__gpio_i2c_wait_ack()) {
        return 1;
    }
    
    while(nbytes--)
    {
        __gpio_i2c_send_byte(*p_buff);
        if(__gpio_i2c_wait_ack()) {
            return 1;
        }
        p_buff++;
    }

    __gpio_i2c_stop();

    return 0;
}

/* end of file */
