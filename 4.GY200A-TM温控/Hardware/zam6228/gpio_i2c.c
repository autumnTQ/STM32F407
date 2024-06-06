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
 * \brief GPIO_I2C ����
 *
 * ʹ��GPIOģ��I2Cʱ��
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

/** \brief IO�������� */
#define __GPIO_I2C_SDA_IN()   {GPIOE->CRL&=0xFFFF0FFF;GPIOE->CRL|=4<<12;} /** \brief �������� */
#define __GPIO_I2C_SDA_OUT()  {GPIOE->CRL&=0xFFFF0FFF;GPIOE->CRL|=7<<12;} /** \brief ��©��� */


/** \brief IO�������� */
#define __GPIO_I2C_SCL         PEout(2)  //SCL
#define __GPIO_I2C_SDA         PEout(3)  //SDA
#define __GPIO_I2C_READ_SDA    PEin(3)   //����SDA
#define __GPIO_I2C_READ_SCL()  GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) //����SCL

#define __GPIO_I2C_ACK_SAMPLE_NUM    (250)
#if (__GPIO_I2C_ACK_SAMPLE_NUM < 1) || (__GPIO_I2C_ACK_SAMPLE_NUM > 255)
    #error "__GPIO_I2C_ACK_SAMPLE_NUM setup error!"
#endif

/*******************************************************************************
 Local functions
*******************************************************************************/

/** \brief ����IIC��ʼ�ź� */
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

/** \brief ����IICֹͣ�ź� */
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
 * \brief �ȴ�Ӧ���ź�
 *
 * \retval  0 : �ȴ��ɹ�
 * \retval  1 : �ȴ�ʧ��
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

/** \brief ����ACK�ź� */
static void __gpio_i2c_send_ack(void)
{
    __GPIO_I2C_SDA_OUT();
    __GPIO_I2C_SDA = 0;
    delay_us(5);
    __GPIO_I2C_SCL = 1;
    delay_us(5);
    __GPIO_I2C_SCL = 0;
}

/** \brief ����NACK�ź� */
static void __gpio_i2c_send_nack(void)
{
    __GPIO_I2C_SDA_OUT();
    __GPIO_I2C_SDA = 1;
    delay_us(5);
    __GPIO_I2C_SCL = 1;
    delay_us(5);
}

/**
 *\brief I2C����æ�ȴ�
 *
 * �ȴ�I2C�����ͷ�Ϊ����״̬����SCL��Ϊ�ߵ�ƽ��I2C���ߴ��ڿ���״̬��
 *
 *\param[in] timeout : ��ʱʱ��, ��λus
 *
 *\retval 0 : �ȴ��ɹ�
 *\retval 1 : �ȴ���ʱ
 */
static uint8_t __gpio_i2c_busy_wait(uint32_t timeout)
{
    uint32_t wait_time = 0;
    
    while (__GPIO_I2C_READ_SCL() == 0) {
        /* �ȴ�ʱ�䵽����timeoutΪ0ʱ��ֱ���˳� */
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
 * \brief I2C����һ���ֽ�
 *
 * \param[in] data : ����������
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
 * \brief ��1���ֽ�
 *
 * param[out] p_data : ��ȡ��������
 * param[in]  ack    : ack=1ʱ������ACK��ack=0ʱ������nACK
 *
 * \retval 0 : �ɹ�
 * \retval 1 : ʧ��
 */
static uint8_t __gpio_i2c_read_byte(uint8_t *p_data,
                                    uint8_t  ack)
{
    uint8_t i       = 0;
    uint8_t receive = 0;

    __GPIO_I2C_SDA_IN();
    delay_us(5);
    
    /**
     * \brief ������������SCL�ߡ���û�дӻ�ռ�����ߣ���SCL�ߴ�ʱΪ�ߵ�ƽ��
     * �����ڴӻ�ռ�����ߣ���SCL�ߴ�ʱΪ�͵�ƽ����Ҫ�ȴ��ӻ��ͷ����߲��ܼ�������ͨ��
     */
    __GPIO_I2C_SCL = 1;
    /* �ȴ��ӻ��ͷ����� */
    if (__gpio_i2c_busy_wait(200)) {
        __gpio_i2c_stop();
        return 1;
    }
    delay_us(2);

    /* ������ȡ���� */
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
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;     /** \brief ��©��� */
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
