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
 * \name ZAM6222 配置参数
 * @{
 */

/** \brief 通道1 */
#define ZAM6222X_CHAN1               (1)
/** \brief 通道2 */
#define ZAM6222X_CHAN2               (0)

/** \brief 测试方式相关宏定义，仅支持ZAM6222B模块设置 */
#define ZAM6222X_SD_TWO_WIRE         (0)               /** \brief 二线制 */
#define ZAM6222X_SD_THREE_WIRE       (1)               /** \brief 三线制 */

/** \brief 比较模式 */
#define ZAM6222X_TM_CMP              (1)
/** \brief 中断模式 */
#define ZAM6222X_TM_IRQ              (0)

/** \brief 输出极性，与模式结合判断温度报警检测状态  */
#define ZAM6222X_POL0                (0)
#define ZAM6222X_POL1                (1)

/** \brief ADC 10Hz采样 */
#define ZAM6222X_RATE_10HZ           (0)
/** \brief ADC 40Hz采样 */
#define ZAM6222X_RATE_40HZ           (1)
/** \brief ADC 1280Hz采样，仅适用于ZAM6222B */
#define ZAM6222X_RATE_1280HZ         (1)

/** \brief 触发 ALERT 信号的温度值测试个数1 */
#define ZAM6222X_FAULTS_1            (0)
/** \brief 触发 ALERT 信号的温度值测试个数2 */
#define ZAM6222X_FAULTS_2            (1)
/** \brief 触发 ALERT 信号的温度值测试个数4 */
#define ZAM6222X_FAULTS_4            (2)
/** \brief 触发 ALERT 信号的温度值测试个数6 */
#define ZAM6222X_FAULTS_6            (3)

/** @} */

/**
 * \name ZAM6222x 接口返回值
 * @{
 */

/** \brief 执行成功 */
#define ZAM6222X_OK                  (0)
/** \brief 参数错误 */
#define ZAM6222X_EINVAL              (1)
/** \brief 设备无应答 */
#define ZAM6222X_ENOACK              (2)
/** \brief 适配函数不存在 */
#define ZAM6222X_EHALNULL            (3)
/** \brief 通道号错误 */
#define ZAM6222X_ECHAN               (4)

/** @} */

/**
 * \brief ZAM6222x 初始化
 *
 * \param[in]  p_hal : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                     在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                     在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 *
 * \retval  ZAM6222X_OK   : 初始化成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_init (void *p_hal);

/**
 * \brief 读取 ZAM6222x 的采样温度
 *
 * \param[in]  p_hal  : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                      在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                      在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[out] p_val1 : 通道1采样温度，传入NULL表示忽略通道1
 * \param[out] p_val2 : 通道2采样温度，传入NULL表示忽略通道2
 *
 * \retval  ZAM6222X_OK   : 读取成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_value_read (void *p_hal, float *p_val1, float *p_val2);

/**
 * \brief 设置 ZAM6222x 温度报警检测阈值
 *
 * \param[in]  p_hal : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                     在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                     在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[in]  chan  : 通道号，ZAM6222_CHANx
 * \param[in]  upper : 阈值范围上限，不超过 1023.999878 ℃
 * \param[in]  lower : 阈值范围下限， 不低于 -1024 ℃
 *
 * \retval  ZAM6222X_OK   : 设置成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_threshold_set (void *p_hal, uint8_t chan, float upper, float lower);

/**
 * \brief 读取 ZAM6222x 温度报警检测阈值
 *
 * \param[in]  p_hal   : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                       在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                       在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[in]  chan    : 通道号，ZAM6222X_CHANx
 * \param[out] p_upper : 阈值范围上限
 * \param[out] p_lower : 阈值范围下限
 *
 * \retval  ZAM6222X_OK   : 读取成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_threshold_get (void    *p_hal,
                            uint8_t  chan,
                            float   *p_upper,
                            float   *p_lower);

/**
 * \brief 配置 ZAM6222x 温度报警检测模式和极性
 *
 * \param[in]  p_hal : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                     在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                     在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[in]  pol   : 极性, ZAM6222X_POLX
 * \param[in]  tm    : 模式, ZAM6222X_TM_XXX
 *
 * \retval  ZAM6222X_OK   : 配置成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_alert_cfg_set (void *p_hal, uint8_t pol, uint8_t tm);

/**
 * \brief 读取 ZAM6222x 温度报警检测模式和极性
 *
 * \param[in]  p_hal : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                     在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                     在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[out] p_pol : 极性, ZAM6222X_POLX
 * \param[out] p_tm  : 模式, ZAM6222X_TM_XXX
 *
 * \retval  ZAM6222X_OK   : 读取成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_alert_cfg_get (void *p_hal, uint8_t *p_pol, uint8_t *p_tm);

/**
 * \brief 设置 ZAM6222x ALERT 脚输出温度报警检测状态的通道号
 *
 * \param[in]  p_hal : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                     在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                     在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[in]  chan  : ALERT 输出哪个通道的温度报警检测状态, ZAM6222X_CHANx
 *
 * \retval  ZAM6222X_OK   : 设置成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_alert_out_set (void *p_hal, uint8_t chan);

/**
 * \brief 读取 ZAM6222X ALERT 脚输出温度报警检测状态的通道号
 *
 * \param[in]  p_hal  : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                      在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                      在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[out] p_chan : ALERT 输出哪个通道的温度报警检测状态, ZAM6222x_CHANx
 *
 * \retval  ZAM6222X_OK   : 读取成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_alert_out_get (void *p_hal, uint8_t *p_chan);

/**
 * \brief 设置 ZAM6222x 内部 ADC 采样速率
 *
 * \param[in]  p_hal : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                     在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                     在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[in]  rate  : 内部 ADC 采样速率, ZAM6222X_RATE_xxxx
 *
 * \retval  ZAM6222X_OK   : 设置成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_rate_set (void *p_hal, uint8_t rate);

/**
 * \brief 读取 ZAM6222x 内部 ADC 采样速率
 *
 * \param[in]  p_hal  : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                      在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                      在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[out] p_rate : 内部 ADC 采样速率, ZAM6222_RATE_xxxx
 *
 * \retval  ZAM6222_OK   : 读取成功
 * \retval -ZAM6222_E... : 其他错误
 */
int zam6222x_rate_get (void *p_hal, uint8_t *p_rate);

/**
 * \brief 读取 ZAM6222x 温度报警检测状态，由于在中断模式(ZAM6222_TM_IRQ)下读取采样
 *        值会清除状态标志，因此在中断模式下需要在读取数据前获取标志
 *
 * \param[in]  p_hal   : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                       在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                       在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[in]  chan    : 通道号，ZAM6222_CHANx
 * \param[out] p_alert : 输出温度报警检测状态
 *
 * \retval  ZAM6222_OK   : 读取成功
 * \retval -ZAM6222_E... : 其他错误
 */
int zam6222x_alert_get (void *p_hal, uint8_t chan, uint8_t *p_alert);

/**
 * \brief 设置 ZAM6222x 触发 ALERT 信号的温度值测试个数
 *
 * \param[in]  p_hal  : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                      在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                      在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[in]  faults : 触发 ALERT 信号的温度值测试个数, ZAM6222X_FAULTS_x
 *
 * \retval  ZAM6222X_OK   : 设置成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_alert_faults_set (void *p_hal, uint8_t faults);

/**
 * \brief 读取 ZAM6222x 触发 ALERT 信号的温度值测试个数
 *
 * \param[in]  p_hal    : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                        在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                        在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[out] p_faults : 触发 ALERT 信号的温度值测试个数, ZAM6222X_FAULTS_x
 *
 * \retval  ZAM6222X_OK   : 读取成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_alert_faults_get (void *p_hal, uint8_t *p_faults);

/**
 * \brief 设置 ZAM6222x 的测试方式
 *
 * 仅适用于ZAM6222B模块
 * 
 * \param[in]  p_hal    : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                        在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                        在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[in]  mode       模块测试方式，见相关宏定义ZAM6222X_SD_x
 *
 * \retval  ZAM6222X_OK   : 读取成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_meas_mode_set (void *p_hal, uint8_t mode);

/**
 * \brief 读取 ZAM6222X 的测试方式
 *
 * 仅适用于ZAM6222B模块
 * 
 * \param[in]  p_hal    : IIC 硬件适配参数(zam6222x_hal.h)，该参数协议栈内部不使用也不做处理，
 *                        在调用硬件适配接口时作为适配接口的第一个参数，应用层与适配层相互约定含义，
 *                        在使用单模块时可以直接传入设备地址: (void *)0x48 或 (void *)0x49.
 * \param[out] p_mode     模块测试方式，见相关宏定义ZAM6222X_SD_x
 *
 * \retval  ZAM6222X_OK   : 读取成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_meas_mode_get (void *p_hal, uint8_t *p_mode);

int zam6222x_temperature_entry (void *p_hal);
float  zam6228_temp(void *p_hal, float *p_val1);
#endif
