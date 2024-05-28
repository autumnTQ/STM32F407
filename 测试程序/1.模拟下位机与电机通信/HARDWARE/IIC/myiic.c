/**
 ****************************************************************************************************
 * @file        myiic.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       IIC驱动代码
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

#include "myiic.h"
#include "delay.h"
#include "apm32f4xx_rcm.h"
#include "stm32f4xx_gpio.h"

/**
 * @brief       IIC延时函数
 * @note        用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static void iic_delay(void)
{
    delay_us(2);    /* 2us的延时，读写速度在250KHz以内 */
}

/**
 * @brief       初始化IIC
 * @param       无
 * @retval      无
 */
void iic_init(void)
{
    GPIO_Config_T gpio_init_struct;
    
    /* 使能时钟 */
    IIC_SCL_GPIO_CLK_ENABLE();                          /* 使能IIC SCL引脚端口时钟 */
    IIC_SDA_GPIO_CLK_ENABLE();                          /* 使能IIC SDA引脚端口时钟 */
    
    /* 配置IIC SCL引脚 */
    gpio_init_struct.pin = IIC_SCL_GPIO_PIN;            /* IIC SCL引脚 */
    gpio_init_struct.mode = GPIO_MODE_OUT;              /* 通用输出模式 */
    gpio_init_struct.speed = GPIO_SPEED_100MHz;         /* 高速 */
    gpio_init_struct.otype = GPIO_OTYPE_PP;             /* 推挽输出 */
    gpio_init_struct.pupd = GPIO_PUPD_UP;               /* 上拉 */
    GPIO_Config(IIC_SCL_GPIO_PORT, &gpio_init_struct);  /* 配置IIC SCL引脚 */
    
    /* 配置IIC SDA引脚 */
    gpio_init_struct.pin = IIC_SDA_GPIO_PIN;            /* IIC SDA引脚 */
    gpio_init_struct.mode = GPIO_MODE_OUT;              /* 通用输出模式 */
    gpio_init_struct.speed = GPIO_SPEED_100MHz;         /* 高速 */
    gpio_init_struct.otype = GPIO_OTYPE_OD;             /* 开漏输出 */
    gpio_init_struct.pupd = GPIO_PUPD_UP;               /* 上拉 */
    GPIO_Config(IIC_SDA_GPIO_PORT, &gpio_init_struct);  /* 配置IIC SDA引脚 */
    
    iic_stop();                                         /* 停止IIC总线上所有设备 */
}

/**
 * @brief       产生IIC起始信号
 * @note        IIC起始信号: 当SCL为高时，SDA从高变成低
 * @param       无
 * @retval      无
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
 * @brief       产生IIC停止信号
 * @note        IIC停止信号: 当SCL为高时，SDA从低变成高
 * @param       无
 * @retval      无
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
 * @brief       等待应答信号
 * @param       无
 * @retval      等待应答结果
 *              1: 接收应答失败
 *              0: 接收应答成功
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    IIC_SDA(1);             /* 主机释放SDA线（此时外部器件可以拉低SDA线） */
    iic_delay();
    IIC_SCL(1);             /* SCL=1，此时从机可以返回ACK */
    iic_delay();
    
    while (IIC_SDA_READ)    /* 等待应答 */
    {
        waittime++;
        if (waittime > 250)
        {
            iic_stop();
            rack = 1;
            break;
        }
    }
    
    IIC_SCL(0);             /* SCL=0，结束ACK检查 */
    iic_delay();
    
    return rack;
}

/**
 * @brief       产生ACK应答
 * @param       无
 * @retval      无
 */
void iic_ack(void)
{
    IIC_SDA(0);     /* SCL 0 -> 1时SDA=0，表示应答 */
    iic_delay();
    IIC_SCL(1);     /* 产生一个时钟 */
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1);     /* 主机释放SDA线 */
    iic_delay();
}

/**
 * @brief       不产生ACK应答
 * @param       无
 * @retval      无
 */
void iic_nack(void)
{
    IIC_SDA(1);     /* SCL 0 -> 1时SDA=1，表示不应答 */
    iic_delay();
    IIC_SCL(1);     /* 产生一个时钟 */
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief       IIC发送一个字节
 * @param       data: 待发送的数据
 * @retval      无
 */
void iic_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        IIC_SDA((data & 0x80) >> 7);    /* 先发送高位 */
        iic_delay();
        IIC_SCL(1);
        iic_delay();
        IIC_SCL(0);
        data <<= 1;                     /* 左移1位，为下一次发送做准备 */
    }
    IIC_SDA(1);                         /* 发送完成，主机释放SDA线 */
}

/**
 * @brief       IIC读取一个字节
 * @param       ack:  
 *   @arg       0: 发送ACK
 *   @arg       1: 发送NACK
 * @retval      接收到的数据
 */
uint8_t iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t receive = 0;
    
    for (i=0; i<8; i++)     /* 遍历接收8比特数据 */
    {
        receive <<= 1;      /* 先接收到高位，所以先收到的数据位要左移 */
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
        iic_nack();         /* 发送NACK */
    }
    else
    {
        iic_ack();          /* 发送ACK */
    }
    
    return receive;
}
