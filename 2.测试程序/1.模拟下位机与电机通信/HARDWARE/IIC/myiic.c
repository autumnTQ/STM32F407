/**
 ****************************************************************************************************
 * @file        myiic.c
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

#include "myiic.h"
#include "delay.h"
#include "apm32f4xx_rcm.h"
#include "stm32f4xx_gpio.h"

/**
 * @brief       IIC��ʱ����
 * @note        ���ڿ���IIC��д�ٶ�
 * @param       ��
 * @retval      ��
 */
static void iic_delay(void)
{
    delay_us(2);    /* 2us����ʱ����д�ٶ���250KHz���� */
}

/**
 * @brief       ��ʼ��IIC
 * @param       ��
 * @retval      ��
 */
void iic_init(void)
{
    GPIO_Config_T gpio_init_struct;
    
    /* ʹ��ʱ�� */
    IIC_SCL_GPIO_CLK_ENABLE();                          /* ʹ��IIC SCL���Ŷ˿�ʱ�� */
    IIC_SDA_GPIO_CLK_ENABLE();                          /* ʹ��IIC SDA���Ŷ˿�ʱ�� */
    
    /* ����IIC SCL���� */
    gpio_init_struct.pin = IIC_SCL_GPIO_PIN;            /* IIC SCL���� */
    gpio_init_struct.mode = GPIO_MODE_OUT;              /* ͨ�����ģʽ */
    gpio_init_struct.speed = GPIO_SPEED_100MHz;         /* ���� */
    gpio_init_struct.otype = GPIO_OTYPE_PP;             /* ������� */
    gpio_init_struct.pupd = GPIO_PUPD_UP;               /* ���� */
    GPIO_Config(IIC_SCL_GPIO_PORT, &gpio_init_struct);  /* ����IIC SCL���� */
    
    /* ����IIC SDA���� */
    gpio_init_struct.pin = IIC_SDA_GPIO_PIN;            /* IIC SDA���� */
    gpio_init_struct.mode = GPIO_MODE_OUT;              /* ͨ�����ģʽ */
    gpio_init_struct.speed = GPIO_SPEED_100MHz;         /* ���� */
    gpio_init_struct.otype = GPIO_OTYPE_OD;             /* ��©��� */
    gpio_init_struct.pupd = GPIO_PUPD_UP;               /* ���� */
    GPIO_Config(IIC_SDA_GPIO_PORT, &gpio_init_struct);  /* ����IIC SDA���� */
    
    iic_stop();                                         /* ֹͣIIC�����������豸 */
}

/**
 * @brief       ����IIC��ʼ�ź�
 * @note        IIC��ʼ�ź�: ��SCLΪ��ʱ��SDA�Ӹ߱�ɵ�
 * @param       ��
 * @retval      ��
 */
void iic_start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief       ����IICֹͣ�ź�
 * @note        IICֹͣ�ź�: ��SCLΪ��ʱ��SDA�ӵͱ�ɸ�
 * @param       ��
 * @retval      ��
 */
void iic_stop(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1);
    iic_delay();
}

/**
 * @brief       �ȴ�Ӧ���ź�
 * @param       ��
 * @retval      �ȴ�Ӧ����
 *              1: ����Ӧ��ʧ��
 *              0: ����Ӧ��ɹ�
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    IIC_SDA(1);             /* �����ͷ�SDA�ߣ���ʱ�ⲿ������������SDA�ߣ� */
    iic_delay();
    IIC_SCL(1);             /* SCL=1����ʱ�ӻ����Է���ACK */
    iic_delay();
    
    while (IIC_SDA_READ)    /* �ȴ�Ӧ�� */
    {
        waittime++;
        if (waittime > 250)
        {
            iic_stop();
            rack = 1;
            break;
        }
    }
    
    IIC_SCL(0);             /* SCL=0������ACK��� */
    iic_delay();
    
    return rack;
}

/**
 * @brief       ����ACKӦ��
 * @param       ��
 * @retval      ��
 */
void iic_ack(void)
{
    IIC_SDA(0);     /* SCL 0 -> 1ʱSDA=0����ʾӦ�� */
    iic_delay();
    IIC_SCL(1);     /* ����һ��ʱ�� */
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1);     /* �����ͷ�SDA�� */
    iic_delay();
}

/**
 * @brief       ������ACKӦ��
 * @param       ��
 * @retval      ��
 */
void iic_nack(void)
{
    IIC_SDA(1);     /* SCL 0 -> 1ʱSDA=1����ʾ��Ӧ�� */
    iic_delay();
    IIC_SCL(1);     /* ����һ��ʱ�� */
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief       IIC����һ���ֽ�
 * @param       data: �����͵�����
 * @retval      ��
 */
void iic_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        IIC_SDA((data & 0x80) >> 7);    /* �ȷ��͸�λ */
        iic_delay();
        IIC_SCL(1);
        iic_delay();
        IIC_SCL(0);
        data <<= 1;                     /* ����1λ��Ϊ��һ�η�����׼�� */
    }
    IIC_SDA(1);                         /* ������ɣ������ͷ�SDA�� */
}

/**
 * @brief       IIC��ȡһ���ֽ�
 * @param       ack:  
 *   @arg       0: ����ACK
 *   @arg       1: ����NACK
 * @retval      ���յ�������
 */
uint8_t iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t receive = 0;
    
    for (i=0; i<8; i++)     /* ��������8�������� */
    {
        receive <<= 1;      /* �Ƚ��յ���λ���������յ�������λҪ���� */
        IIC_SCL(1);
        iic_delay();
        
        if (IIC_SDA_READ)
        {
            receive++;
        }
        
        IIC_SCL(0);
        iic_delay();
    }
    
    if (!ack)
    {
        iic_nack();         /* ����NACK */
    }
    else
    {
        iic_ack();          /* ����ACK */
    }
    
    return receive;
}
