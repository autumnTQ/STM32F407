#ifndef __ZAM6222_H
#define __ZAM6222_H

//#ifdef __cplusplus
//extern "C" {
//#endif

#include "stm32f10x.h"
#include "gpio_i2c.h"
#include "delay.h"
#include "stdio.h"
#include "bsp_485.h"
#include "iic_ssd1306.h"
//#include "bsp/485/bsp_usartx_485.h"
//#include "bsp/i2c/bsp_i2c_OLED.h"
/**
 * \addtogroup zam6222x_if
 * \copydoc zam6222x.h
 * @{
 */

/**
 * \name ZAM6222 ���ò���
 * @{
 */

/** \brief ͨ��1 */
#define ZAM6222X_CHAN1               (1)
/** \brief ͨ��2 */
#define ZAM6222X_CHAN2               (0)

/** \brief ���Է�ʽ��غ궨�壬��֧��ZAM6222Bģ������ */
#define ZAM6222X_SD_TWO_WIRE         (0)               /** \brief ������ */
#define ZAM6222X_SD_THREE_WIRE       (1)               /** \brief ������ */

/** \brief �Ƚ�ģʽ */
#define ZAM6222X_TM_CMP              (1)
/** \brief �ж�ģʽ */
#define ZAM6222X_TM_IRQ              (0)

/** \brief ������ԣ���ģʽ����ж��¶ȱ������״̬  */
#define ZAM6222X_POL0                (0)
#define ZAM6222X_POL1                (1)

/** \brief ADC 10Hz���� */
#define ZAM6222X_RATE_10HZ           (0)
/** \brief ADC 40Hz���� */
#define ZAM6222X_RATE_40HZ           (1)
/** \brief ADC 1280Hz��������������ZAM6222B */
#define ZAM6222X_RATE_1280HZ         (1)

/** \brief ���� ALERT �źŵ��¶�ֵ���Ը���1 */
#define ZAM6222X_FAULTS_1            (0)
/** \brief ���� ALERT �źŵ��¶�ֵ���Ը���2 */
#define ZAM6222X_FAULTS_2            (1)
/** \brief ���� ALERT �źŵ��¶�ֵ���Ը���4 */
#define ZAM6222X_FAULTS_4            (2)
/** \brief ���� ALERT �źŵ��¶�ֵ���Ը���6 */
#define ZAM6222X_FAULTS_6            (3)

/** @} */

/**
 * \name ZAM6222x �ӿڷ���ֵ
 * @{
 */

/** \brief ִ�гɹ� */
#define ZAM6222X_OK                  (0)
/** \brief �������� */
#define ZAM6222X_EINVAL              (1)
/** \brief �豸��Ӧ�� */
#define ZAM6222X_ENOACK              (2)
/** \brief ���亯�������� */
#define ZAM6222X_EHALNULL            (3)
/** \brief ͨ���Ŵ��� */
#define ZAM6222X_ECHAN               (4)

/** @} */

/**
 * \brief ZAM6222x ��ʼ��
 *
 * \param[in]  p_hal : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                     �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                     ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 *
 * \retval  ZAM6222X_OK   : ��ʼ���ɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_init (void *p_hal);

/**
 * \brief ��ȡ ZAM6222x �Ĳ����¶�
 *
 * \param[in]  p_hal  : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                      �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                      ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[out] p_val1 : ͨ��1�����¶ȣ�����NULL��ʾ����ͨ��1
 * \param[out] p_val2 : ͨ��2�����¶ȣ�����NULL��ʾ����ͨ��2
 *
 * \retval  ZAM6222X_OK   : ��ȡ�ɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_value_read (void *p_hal, float *p_val1, float *p_val2);

/**
 * \brief ���� ZAM6222x �¶ȱ��������ֵ
 *
 * \param[in]  p_hal : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                     �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                     ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[in]  chan  : ͨ���ţ�ZAM6222_CHANx
 * \param[in]  upper : ��ֵ��Χ���ޣ������� 1023.999878 ��
 * \param[in]  lower : ��ֵ��Χ���ޣ� ������ -1024 ��
 *
 * \retval  ZAM6222X_OK   : ���óɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_threshold_set (void *p_hal, uint8_t chan, float upper, float lower);

/**
 * \brief ��ȡ ZAM6222x �¶ȱ��������ֵ
 *
 * \param[in]  p_hal   : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                       �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                       ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[in]  chan    : ͨ���ţ�ZAM6222X_CHANx
 * \param[out] p_upper : ��ֵ��Χ����
 * \param[out] p_lower : ��ֵ��Χ����
 *
 * \retval  ZAM6222X_OK   : ��ȡ�ɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_threshold_get (void    *p_hal,
                            uint8_t  chan,
                            float   *p_upper,
                            float   *p_lower);

/**
 * \brief ���� ZAM6222x �¶ȱ������ģʽ�ͼ���
 *
 * \param[in]  p_hal : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                     �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                     ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[in]  pol   : ����, ZAM6222X_POLX
 * \param[in]  tm    : ģʽ, ZAM6222X_TM_XXX
 *
 * \retval  ZAM6222X_OK   : ���óɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_alert_cfg_set (void *p_hal, uint8_t pol, uint8_t tm);

/**
 * \brief ��ȡ ZAM6222x �¶ȱ������ģʽ�ͼ���
 *
 * \param[in]  p_hal : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                     �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                     ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[out] p_pol : ����, ZAM6222X_POLX
 * \param[out] p_tm  : ģʽ, ZAM6222X_TM_XXX
 *
 * \retval  ZAM6222X_OK   : ��ȡ�ɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_alert_cfg_get (void *p_hal, uint8_t *p_pol, uint8_t *p_tm);

/**
 * \brief ���� ZAM6222x ALERT ������¶ȱ������״̬��ͨ����
 *
 * \param[in]  p_hal : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                     �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                     ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[in]  chan  : ALERT ����ĸ�ͨ�����¶ȱ������״̬, ZAM6222X_CHANx
 *
 * \retval  ZAM6222X_OK   : ���óɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_alert_out_set (void *p_hal, uint8_t chan);

/**
 * \brief ��ȡ ZAM6222X ALERT ������¶ȱ������״̬��ͨ����
 *
 * \param[in]  p_hal  : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                      �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                      ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[out] p_chan : ALERT ����ĸ�ͨ�����¶ȱ������״̬, ZAM6222x_CHANx
 *
 * \retval  ZAM6222X_OK   : ��ȡ�ɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_alert_out_get (void *p_hal, uint8_t *p_chan);

/**
 * \brief ���� ZAM6222x �ڲ� ADC ��������
 *
 * \param[in]  p_hal : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                     �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                     ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[in]  rate  : �ڲ� ADC ��������, ZAM6222X_RATE_xxxx
 *
 * \retval  ZAM6222X_OK   : ���óɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_rate_set (void *p_hal, uint8_t rate);

/**
 * \brief ��ȡ ZAM6222x �ڲ� ADC ��������
 *
 * \param[in]  p_hal  : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                      �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                      ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[out] p_rate : �ڲ� ADC ��������, ZAM6222_RATE_xxxx
 *
 * \retval  ZAM6222_OK   : ��ȡ�ɹ�
 * \retval -ZAM6222_E... : ��������
 */
int zam6222x_rate_get (void *p_hal, uint8_t *p_rate);

/**
 * \brief ��ȡ ZAM6222x �¶ȱ������״̬���������ж�ģʽ(ZAM6222_TM_IRQ)�¶�ȡ����
 *        ֵ�����״̬��־��������ж�ģʽ����Ҫ�ڶ�ȡ����ǰ��ȡ��־
 *
 * \param[in]  p_hal   : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                       �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                       ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[in]  chan    : ͨ���ţ�ZAM6222_CHANx
 * \param[out] p_alert : ����¶ȱ������״̬
 *
 * \retval  ZAM6222_OK   : ��ȡ�ɹ�
 * \retval -ZAM6222_E... : ��������
 */
int zam6222x_alert_get (void *p_hal, uint8_t chan, uint8_t *p_alert);

/**
 * \brief ���� ZAM6222x ���� ALERT �źŵ��¶�ֵ���Ը���
 *
 * \param[in]  p_hal  : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                      �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                      ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[in]  faults : ���� ALERT �źŵ��¶�ֵ���Ը���, ZAM6222X_FAULTS_x
 *
 * \retval  ZAM6222X_OK   : ���óɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_alert_faults_set (void *p_hal, uint8_t faults);

/**
 * \brief ��ȡ ZAM6222x ���� ALERT �źŵ��¶�ֵ���Ը���
 *
 * \param[in]  p_hal    : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                        �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                        ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[out] p_faults : ���� ALERT �źŵ��¶�ֵ���Ը���, ZAM6222X_FAULTS_x
 *
 * \retval  ZAM6222X_OK   : ��ȡ�ɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_alert_faults_get (void *p_hal, uint8_t *p_faults);

/**
 * \brief ���� ZAM6222x �Ĳ��Է�ʽ
 *
 * ��������ZAM6222Bģ��
 * 
 * \param[in]  p_hal    : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                        �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                        ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[in]  mode       ģ����Է�ʽ������غ궨��ZAM6222X_SD_x
 *
 * \retval  ZAM6222X_OK   : ��ȡ�ɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_meas_mode_set (void *p_hal, uint8_t mode);

/**
 * \brief ��ȡ ZAM6222X �Ĳ��Է�ʽ
 *
 * ��������ZAM6222Bģ��
 * 
 * \param[in]  p_hal    : IIC Ӳ���������(zam6222x_hal.h)���ò���Э��ջ�ڲ���ʹ��Ҳ��������
 *                        �ڵ���Ӳ������ӿ�ʱ��Ϊ����ӿڵĵ�һ��������Ӧ�ò���������໥Լ�����壬
 *                        ��ʹ�õ�ģ��ʱ����ֱ�Ӵ����豸��ַ: (void *)0x48 �� (void *)0x49.
 * \param[out] p_mode     ģ����Է�ʽ������غ궨��ZAM6222X_SD_x
 *
 * \retval  ZAM6222X_OK   : ��ȡ�ɹ�
 * \retval -ZAM6222X_E... : ��������
 */
int zam6222x_meas_mode_get (void *p_hal, uint8_t *p_mode);

int zam6222x_temperature_entry (void *p_hal);
float  zam6228_temp(void *p_hal, float *p_val1);
#endif
