/**
 ****************************************************************************************************
 * @file        touch.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       ��������������
 * @note        ֧�ֵ��衢����ʽ������
 *              ����������֧��ADS7843/7846/UH7843/7846/XPT2046/TSC2046/GT9147/GT9271/FT5206��
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

#ifndef __TOUCH_H
#define __TOUCH_H

#include "sys.h"

/* ���败�������Ŷ��� */
#define T_PEN_GPIO_PORT             GPIOB
#define T_PEN_GPIO_PIN              GPIO_PIN_1
#define T_PEN_GPIO_CLK_ENABLE()     do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

#define T_CS_GPIO_PORT              GPIOC
#define T_CS_GPIO_PIN               GPIO_PIN_13
#define T_CS_GPIO_CLK_ENABLE()      do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC); }while(0)

#define T_MISO_GPIO_PORT            GPIOB
#define T_MISO_GPIO_PIN             GPIO_PIN_2
#define T_MISO_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

#define T_MOSI_GPIO_PORT            GPIOF
#define T_MOSI_GPIO_PIN             GPIO_PIN_11
#define T_MOSI_GPIO_CLK_ENABLE()    do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOF); }while(0)

#define T_CLK_GPIO_PORT             GPIOB
#define T_CLK_GPIO_PIN              GPIO_PIN_0
#define T_CLK_GPIO_CLK_ENABLE()     do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB); }while(0)

/* ���败��������IO���� */
#define T_MOSI(x)                   do{ x ?                                                 \
                                        GPIO_SetBit(T_MOSI_GPIO_PORT, T_MOSI_GPIO_PIN) :    \
                                        GPIO_ResetBit(T_MOSI_GPIO_PORT, T_MOSI_GPIO_PIN);   \
                                    }while(0)

#define T_CLK(x)                    do{ x ?                                                 \
                                        GPIO_SetBit(T_CLK_GPIO_PORT, T_CLK_GPIO_PIN) :      \
                                        GPIO_ResetBit(T_CLK_GPIO_PORT, T_CLK_GPIO_PIN);     \
                                    }while(0)

#define T_CS(x)                     do{ x ?                                                 \
                                        GPIO_SetBit(T_CS_GPIO_PORT, T_CS_GPIO_PIN) :        \
                                        GPIO_ResetBit(T_CS_GPIO_PORT, T_CS_GPIO_PIN);       \
                                    }while(0)

#define T_PEN                       GPIO_ReadInputBit(T_PEN_GPIO_PORT, T_PEN_GPIO_PIN)
#define T_MISO                      GPIO_ReadInputBit(T_MISO_GPIO_PORT, T_MISO_GPIO_PIN)

/* ������ض��� */
#define TP_PRES_DOWN                0x8000  /* ���������� */
#define TP_CATH_PRES                0x4000  /* �а��������� */
#define CT_MAX_TOUCH                10      /* ������֧�ֵĵ���,�̶�Ϊ5�� */

/* ���������ƽṹ�� */
typedef struct
{
    uint8_t (*init)(void);      /* ��ʼ�������������� */
    uint8_t (*scan)(uint8_t);   /* ɨ�败������0����Ļɨ�裻1���������� */
    void (*adjust)(void);       /* ������У׼ */
    uint16_t x[CT_MAX_TOUCH];   /* ��ǰ���� */
    uint16_t y[CT_MAX_TOUCH];   /* �����������10�����꣬����������x[0]��y[0]����˴�ɨ��ʱ����������
                                 * ��x[9]��y[9]�洢��һ�ΰ���ʱ������
                                 */
    uint16_t sta;               /* �ʵ�״̬
                                 * b15��1�����£�0���ɿ�
                                 * b14��0��û�а������£�1���а�������
                                 * b13~b10������
                                 * b9~b0�����ݴ��������µĵ�����0��δ���£�1�����£�
                                 */
    /* 5��У׼������У׼����������������ҪУ׼�� */
    float xfac;                 /* 5��У׼��x����������� */
    float yfac;                 /* 5��У׼��y����������� */
    short xc;                   /* ����X��������ֵ��ADֵ�� */
    short yc;                   /* ����Y��������ֵ��ADֵ�� */
    uint8_t touchtype;          /* �����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�
                                 * b0��0���������ʺ�����ΪX���꣬����ΪY�����TP��
                                 *     1���������ʺ�����ΪY���꣬����ΪX�����TP��
                                 * b1~6������
                                 * b7��0��������
                                 *     1��������
                                 */
} _m_tp_dev;
extern _m_tp_dev tp_dev;

/* �������� */
void tp_draw_big_point(uint16_t x, uint16_t y, uint16_t color); /* ��һ����� */
void tp_save_adjust_data(void);                                 /* ����У׼���� */
uint8_t tp_scan(uint8_t mode);                                  /* ������ɨ�� */
void tp_adjust(void);                                           /* ������У׼ */
uint8_t tp_init(void);                                          /* ��ʼ�������� */

#endif
