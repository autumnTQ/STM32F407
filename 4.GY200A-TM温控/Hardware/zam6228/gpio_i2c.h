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

#ifndef __GPIO_I2C_H
#define __GPIO_I2C_H

#include <stdint.h>
#include <stddef.h>


/**
 * \brief GPIO_I2C初始化
 *
 * \retval  0     : 成功
 * \retval 其他值 : 失败
 */
uint8_t gpio_i2c_init(void);


/**
 * \brief GPIO_I2C写
 *
 * \param[in]  dev_addr   : 从设备地址,需传入左移一位后的地址
 * \param[in]  sub_addr   : 寄存器地址
 * \param[in]  addr_len   : 寄存器地址长度
 * \param[in]  p_buff     : 发送数据缓存区
 * \param[in]  nbytes     : 发送数据字节数
 *
 * \retval 0 : 写成功
 * \retval 1 : 写失败
 */
uint8_t gpio_i2c_write(uint8_t        dev_addr, 
                       uint32_t       sub_addr,
                       uint32_t       addr_len,
                       const uint8_t *p_buff,
                       uint32_t       nbytes);

/**
 * \brief GPIO_I2C读
 *
 * \param[in]  dev_addr   : 从设备地址,需传入左移一位后的地址
 * \param[in]  sub_addr   : 寄存器地址
 * \param[in]  addr_len   : 寄存器地址长度
 * \param[in]  p_buff     : 接收数据缓存区
 * \param[in]  nbytes     : 接收数据字节数
 *
 * \retval 0 : 读取成功
 * \retval 1 : 读取失败
 */
uint8_t gpio_i2c_read(uint8_t      dev_addr, 
                      uint32_t     sub_addr,
                      uint32_t     addr_len,
                      uint8_t     *p_buff,
                      uint32_t     nbytes);


#endif /* __GPIO_I2C_H */

/* end of file */
