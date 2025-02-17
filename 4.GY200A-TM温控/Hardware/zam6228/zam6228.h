/**
 * \file
 * \brief zam6228模块驱动头文件
 * 
 * \internal
 * \par modification history:
 * - 1.00 23-03-28, first implementation.
 * \endinternal
 */

#include "main.h"

#define ZAM6228_FUN_OK        0                     //ZAM6228函数相关接口操作成功
#define ZAM6228_FUN_ERROR    -1                     //ZAM6228函数相关接口操作失败
#define ZAM6228_VALUE_ERROR  -2                     //ZAM6228函数的传入参数错误


/**
 * \broef ZAM6228设备通道
 */
#define ZAM6228_CHAN1         0                     //ZAM6228设备通道1
#define ZAM6228_CHAN2         1                     //ZAM6228设备通道2
#define ZAM6228_CHAN3         2                     //ZAM6228设备通道3
#define ZAM6228_CHAN4         3                     //ZAM6228设备通道4
#define ZAM6228_CHAN5         4                     //ZAM6228设备通道5
#define ZAM6228_CHAN6         5                     //ZAM6228设备通道6
#define ZAM6228_CHAN7         6                     //ZAM6228设备通道7
#define ZAM6228_CHAN8         7                     //ZAM6228设备通道8
/**
 * \broef ZAM6228设备通道使能
 */
#define ZAM6228_CHAN1_EN       1 << 0               //ZAM6228通道1使能
#define ZAM6228_CHAN2_EN       1 << 1               //ZAM6228通道2使能
#define ZAM6228_CHAN3_EN       1 << 2               //ZAM6228通道3使能
#define ZAM6228_CHAN4_EN       1 << 3               //ZAM6228通道4使能
#define ZAM6228_CHAN5_EN       1 << 4               //ZAM6228通道5使能
#define ZAM6228_CHAN6_EN       1 << 5               //ZAM6228通道6使能
#define ZAM6228_CHAN7_EN       1 << 6               //ZAM6228通道7使能
#define ZAM6228_CHAN8_EN       1 << 7               //ZAM6228通道8使能

 /**
 * \broef 子寄存器地址
 */
#define ZAM6228_TEMP_ADDR    (0X00)                     //温度子寄存器地址
#define ZAM6228_TEMP_LEN     (0X18)                     //温度子寄存器长度

#define ZAM6228_CHAN_EN_ADDR (0X0C)                     //通道使能子寄存器地址
#define ZAM6228_CHAN_EN_LEN  (0X01)                     //通道使能子寄存器长度


/**
 * \broef ZAM6228设备结构体
 */
//typedef struct zlg_zam6228_device {
//    I2C_InitTypeDef *hi2c;                /* i2c设备的句柄 */
//    uint16_t           zam6228_addr;         /* ZAM6228模块地址 */  
//}zlg_zam6228_device_t;



/**
 * \brief 获取寄存器通道的温度值
 * 
 * 
 * \param[in] p_dev ZAM6228设备
 * \param[in] chan 通道 
 * \param[in] memaddr 子寄存器地址，
 * \param[out] p_data 存放温度数据的缓存区
 * 
 * \retval ZAM6228_FUN_OK       配置成功
 * \retval ZAM6228_FUN_ERROR    配置失败
 * \retval ZAM6228_VALUE_ERROR  参数错误
 */
int zam6228_get_temp(void *p_dev, uint16_t memaddr, int8_t chan, float *p_data);



/**
 * \brief 
 * 
 * \param[in] p_dev   ZAM6228设备
 * \param[in] memaddr 子寄存器地址�
 * \param[in] chan_en 需要选择的使能通道

 * 
 * \retval ZAM6228_FUN_OK    配置成功
 * \retval ZAM6228_FUN_ERROR 配置失败
 * \retval ZAM6228_VALUE_ERROR  参数错误
 */
int  zam6228_set_chan_en(void *p_dev, uint16_t memaddr,int8_t chan_en) ;
int zam6228_demo_entry(void);



/** end of file **/
