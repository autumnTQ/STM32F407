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

#ifndef __GPIO_I2C_H
#define __GPIO_I2C_H

#include <stdint.h>
#include <stddef.h>


/**
 * \brief GPIO_I2C��ʼ��
 *
 * \retval  0     : �ɹ�
 * \retval ����ֵ : ʧ��
 */
uint8_t gpio_i2c_init(void);


/**
 * \brief GPIO_I2Cд
 *
 * \param[in]  dev_addr   : ���豸��ַ,�贫������һλ��ĵ�ַ
 * \param[in]  sub_addr   : �Ĵ�����ַ
 * \param[in]  addr_len   : �Ĵ�����ַ����
 * \param[in]  p_buff     : �������ݻ�����
 * \param[in]  nbytes     : ���������ֽ���
 *
 * \retval 0 : д�ɹ�
 * \retval 1 : дʧ��
 */
uint8_t gpio_i2c_write(uint8_t        dev_addr, 
                       uint32_t       sub_addr,
                       uint32_t       addr_len,
                       const uint8_t *p_buff,
                       uint32_t       nbytes);

/**
 * \brief GPIO_I2C��
 *
 * \param[in]  dev_addr   : ���豸��ַ,�贫������һλ��ĵ�ַ
 * \param[in]  sub_addr   : �Ĵ�����ַ
 * \param[in]  addr_len   : �Ĵ�����ַ����
 * \param[in]  p_buff     : �������ݻ�����
 * \param[in]  nbytes     : ���������ֽ���
 *
 * \retval 0 : ��ȡ�ɹ�
 * \retval 1 : ��ȡʧ��
 */
uint8_t gpio_i2c_read(uint8_t      dev_addr, 
                      uint32_t     sub_addr,
                      uint32_t     addr_len,
                      uint8_t     *p_buff,
                      uint32_t     nbytes);


#endif /* __GPIO_I2C_H */

/* end of file */
