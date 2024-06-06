#include "zam6222.h"
 uint8_t TMP1[10];
 extern float curTmp ;
 extern float temperature0,temperature1;
/** \brief Register data converted to integer */
#define __ZAM6222_REG_TO_INT(p_dat)     (((p_dat)[0] << 16) | \
                                         ((p_dat)[1] << 8)  | \
                                          (p_dat)[2])

/** \brief Integer converted to register data */
#define __ZAM6222_INT_TO_REG(p_dat, val) ((p_dat)[0] = ((val) >> 16) & 0xff, \
                                          (p_dat)[1] = ((val) >> 8) & 0xff,  \
                                          (p_dat)[2] =  (val & 0xff))

/** \brief Enable configuration index(0 or 1) */
#define __ZAM6222_CONFIG(config)         \
     ((config)[((((config)[0] >> 7) & 0x1) && (!(((config)[1] >> 7) & 0x1)))])

/******************************************************************************/
/**
 * \brief ZAM6222X IIC 初始化适配，函数由 zam6222x_init() 接口调用
 *
 * \param[in]  p_hal : 适配参数, zam6222x_init() 接口传入的第一个参数
 *
 * \retval  ZAM6222X_OK   : 初始化成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_hal_i2c_init (void *p_hal)
{
    uint8_t   ret = 0;
    
    ret = gpio_i2c_init();

    if (ret != 0) {
        return -1;
    }

    return 0;
}

/**
 * \brief ZAM6222X IIC 读适配，该函数由 ZAM6222x 驱动接口函数调用
 *
 * \param[in]  p_hal    : 适配参数，调用 zam6222x.h 接口时传入的第一个参数
 * \param[in]  sub_addr : 寄存器地址
 * \param[out] p_buf    : 读数据缓存区地址
 * \param[in]  nbytes   : 读取字节数
 *
 * \retval  ZAM6222X_OK   : 读取成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_hal_i2c_read (void     *p_hal,
                           uint32_t  sub_addr,
                           uint8_t  *p_buf,
                           uint32_t  nbytes)
{
    uint8_t   i2c_status = 0;
    uint32_t  dev_addr = (uint32_t)p_hal;


    i2c_status = gpio_i2c_read((dev_addr << 1),
                               sub_addr,
                               1,
                               p_buf,
                               nbytes);

    if (i2c_status != 0) {
        return -1;
    }

    return 0;
}

/**
 * \brief ZAM6222X IIC 写适配，该函数由 ZAM6222x 驱动接口函数调用
 *
 * \param[in]  p_hal    : 适配参数，调用 zam6222x.h 接口时传入的第一个参数
 * \param[in]  sub_addr : 寄存器地址
 * \param[in]  p_buf    : 写数据缓存区地址
 * \param[in]  nbytes   : 写字节数
 *
 * \retval  ZAM6222X_OK   : 读取成功
 * \retval -ZAM6222X_E... : 其他错误
 */
int zam6222x_hal_i2c_write (void          *p_hal,
                            uint32_t       sub_addr,
                            const uint8_t *p_buf,
                            uint32_t       nbytes)
{
    uint8_t   i2c_status = 0;
    uint32_t  dev_addr = (uint32_t)p_hal;


    i2c_status = gpio_i2c_write((dev_addr << 1),
                                sub_addr,
                                1,
                                p_buf,
                                nbytes);

    if (i2c_status != 0) {
        return -1;
    }

    return 0;
}
/**
 * \brief ZAM6222x 寄存器数据转 float 类型
 */
static void __zam6222_reg_to_float (uint8_t *p_reg, float *p_float)
{
    uint32_t integer = __ZAM6222_REG_TO_INT(p_reg);

    if (integer >= 8388608) {
        *p_float = (-(float)((16777216 - integer) / (float)(0x1 << 13)));
    } else {
        *p_float = ((float)(integer / (float)(0x1 << 13)));
    }
}

/**
 * \brief ZAM6222x float 数据转寄存器类型
 */
static void __zam6222x_float_to_reg (uint8_t *p_reg, float vfloat)
{
    uint32_t integer = ((uint32_t)((int)((vfloat) * (0x1 << 13)) & 0xffffff));

    __ZAM6222_INT_TO_REG(p_reg, integer);
}

/******************************************************************************/

/**
 * \brief ZAM6222x 初始化
 */
int zam6222x_init (void *p_hal)
{
    return zam6222x_hal_i2c_init(p_hal);
}

/**
 * \brief 读取 ZAM6222x 的采样温度
 */
int zam6222x_value_read (void *p_hal, float *p_val1, float *p_val2)
{
    int     error;
    uint8_t data[6];

    error = zam6222x_hal_i2c_read(p_hal, 0x0, &data[0], 0x6);
    if (error != ZAM6222X_OK) {
        return error;
    }

    if (p_val1) {
        __zam6222_reg_to_float(&data[0], p_val1);
    }

    if (p_val2) {
        __zam6222_reg_to_float(&data[3], p_val2);
    }

    return ZAM6222X_OK;
}

float  zam6228_temp(void *p_hal, float *p_val1)
{
	    int     error;
    uint8_t data[6];
 error = zam6222x_hal_i2c_read(p_hal, 0x0, &data[0], 0x6);
    if (error != ZAM6222X_OK) {
        return error;
    }

    if (p_val1) {
        __zam6222_reg_to_float(&data[0], p_val1);
    }
   
		return *p_val1;
}

/**
 * \brief 设置 ZAM6222x 温度报警检测阈值
 */
int zam6222x_threshold_set (void *p_hal, uint8_t chan, float upper, float lower)
{
    int     error;
    uint8_t high_data[6];
    uint8_t low_data[6];

    if (chan > ZAM6222X_CHAN2) {
        return -ZAM6222X_ECHAN;
    }

    if (upper > 1023.999878 || lower < -1024 || lower > upper) {
        return -ZAM6222X_EINVAL;
    }

    /* Read low temperature threshold */
    error = zam6222x_hal_i2c_read(p_hal, 0x2, &low_data[0], 0x6);
    if (error != ZAM6222X_OK) {
        return error;
    }

    /* Read high temperature threshold */
    error = zam6222x_hal_i2c_read(p_hal, 0x3, &high_data[0], 0x6);
    if (error != ZAM6222X_OK) {
        return error;
    }

    __zam6222x_float_to_reg(&low_data[chan * 3], lower);
    __zam6222x_float_to_reg(&high_data[chan * 3], upper);

    /* Write low temperature threshold */
    error = zam6222x_hal_i2c_write(p_hal, 0x2, &low_data[0], 0x6);
    if (error != ZAM6222X_OK) {
        return error;
    }

    /* Write high temperature threshold */
    error = zam6222x_hal_i2c_write(p_hal, 0x3, &high_data[0], 0x6);
    if (error != ZAM6222X_OK) {
        return error;
    }

    return ZAM6222X_OK;
}

/**
 * \brief 读取 ZAM6222x 模块温度报警检测阈值
 */
int zam6222x_threshold_get (void    *p_hal,
                           uint8_t  chan,
                           float   *p_upper,
                           float   *p_lower)
{
    int     error;
    uint8_t high_data[6];
    uint8_t low_data[6];

    if (chan > ZAM6222X_CHAN2) {
        return -ZAM6222X_ECHAN;
    }

    if (NULL == p_upper || NULL == p_lower) {
        return -ZAM6222X_EINVAL;
    }

    /* Read low temperature threshold */
    error = zam6222x_hal_i2c_read(p_hal, 0x2, &low_data[0], 0x6);
    if (error != ZAM6222X_OK) {
        return error;
    }
    
    /* Read high temperature threshold */
    error = zam6222x_hal_i2c_read(p_hal, 0x3, &high_data[0], 0x6);
    if (error != ZAM6222X_OK) {
        return error;
    }

    __zam6222_reg_to_float(&low_data[chan * 3], p_lower);
    __zam6222_reg_to_float(&high_data[chan * 3], p_upper);

    return ZAM6222X_OK;
}

/**
 * \brief 配置 ZAM6222x 温度报警检测模式和极性
 */
int zam6222x_alert_cfg_set (void *p_hal, uint8_t pol, uint8_t tm)
{
    int     error;
    uint8_t config[2];

    if (pol > ZAM6222X_POL1 || tm > ZAM6222X_TM_IRQ) {
        return -ZAM6222X_EINVAL;
    }

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    /* Update configuration */
    config[0] &= ~(0x3 << 1);
    config[0] |= pol << 2;
    config[0] |= tm  << 1;
    config[1] &= ~(0x3 << 1);
    config[1] |= pol << 2;
    config[1] |= tm  << 1;

    /* Write the latest configuration */
    return zam6222x_hal_i2c_write(p_hal, 0x1, &config[0], 0x2);
}

/**
 * \brief 读取 ZAM6222x 模块温度报警检测模式和极性
 */
int zam6222x_alert_cfg_get (void *p_hal, uint8_t *p_pol, uint8_t *p_tm)
{
    int     error;
    uint8_t config[2];

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    *p_pol = (__ZAM6222_CONFIG(config) >> 2) & 0x1;
    *p_tm  = (__ZAM6222_CONFIG(config) >> 1) & 0x1;

    return ZAM6222X_OK;
}

/**
 * \brief 设置 ZAM6222x ALERT 脚输出温度报警检测状态的通道号
 */
int zam6222x_alert_out_set (void *p_hal, uint8_t chan)
{
    int     error;
    uint8_t config[2];

    if (chan > ZAM6222X_CHAN2) {
        return -ZAM6222X_ECHAN;
    }

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    chan++; /* 0b01 0b10 */

    /* Update configuration */
    config[0] &= ~(0x1 << 7);
    config[1] &= ~(0x1 << 7);
    config[1] |= (chan & 0x1) << 7;
    config[0] |= ((chan >> 1) & 0x1) << 7;

    /* Write the latest configuration */
    return zam6222x_hal_i2c_write(p_hal, 0x1, &config[0], 0x2);
}

/**
 * \brief 读取 ZAM6222x ALERT 脚输出温度报警检测状态的通道号
 */
int zam6222x_alert_out_get (void *p_hal, uint8_t *p_chan)
{
    int     error;
    uint8_t config[2];

    if (p_chan == NULL) {
        return -ZAM6222X_EINVAL;
    }

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    if ((((config[0] >> 7) & 0x1) == 1) && (((config[1] >> 7) & 0x1) == 0)) {
        *p_chan = ZAM6222X_CHAN2;
    } else {
        *p_chan = ZAM6222X_CHAN1;
    }

    return ZAM6222X_OK;
}

/**
 * \brief 设置 ZAM6222x 内部 ADC 采样速率
 */
int zam6222x_rate_set (void *p_hal, uint8_t rate)
{
    int     error;
    uint8_t config[2];

    if (rate > ZAM6222X_RATE_40HZ) {
        return -ZAM6222X_ECHAN;
    }

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    /* Update configuration */
    config[0] &= ~(0x1 << 5);
    config[0] |= rate << 5;
    config[1] &= ~(0x1 << 5);
    config[1] |= rate << 5;

    /* Write the latest configuration */
    return zam6222x_hal_i2c_write(p_hal, 0x1, &config[0], 0x2);
}

/**
 * \brief 读取 ZAM6222x 内部 ADC 采样速率
 */
int zam6222x_rate_get (void *p_hal, uint8_t *p_rate)
{
    int     error;
    uint8_t config[2];

    if (p_rate == NULL) {
        return -ZAM6222X_EINVAL;
    }

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    *p_rate = (__ZAM6222_CONFIG(config) >> 5) & 0x1;

    return ZAM6222X_OK;
}

/**
 * \brief 读取 ZAM6222x 温度报警检测状态，由于在中断模式(ZAM6222X_TM_IRQ)下读取采样
 *        值会清除状态标志，因此在中断模式下需要在读取数据前获取标志
 */
int zam6222x_alert_get (void *p_hal, uint8_t chan, uint8_t *p_alert)
{
    int     error;
    uint8_t config[2];

    if (p_alert == NULL) {
        return -ZAM6222X_EINVAL;
    }

    if (chan > ZAM6222X_CHAN2) {
        return -ZAM6222X_ECHAN;
    }

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    *p_alert = (config[chan] >> 6) & 0x1;

    return ZAM6222X_OK;
}

/**
 * \brief 设置 ZAM6222x 触发 ALERT 信号的温度值测试个数
 */
int zam6222x_alert_faults_set (void *p_hal, uint8_t faults)
{
    int     error;
    uint8_t config[2];

    if (faults > ZAM6222X_FAULTS_6) {
        return -ZAM6222X_EINVAL;
    }

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    /* Update configuration */
    config[0] &= ~(0x3 << 3);
    config[0] |= faults << 3;
    config[1] &= ~(0x3 << 3);
    config[1] |= faults << 3;

    /* Write the latest configuration */
    return zam6222x_hal_i2c_write(p_hal, 0x1, &config[0], 0x2);
}

/**
 * \brief 读取 ZAM6222x 触发 ALERT 信号的温度值测试个数
 */
int zam6222x_alert_faults_get (void *p_hal, uint8_t *p_faults)
{
    int     error;
    uint8_t config[2];

    if (p_faults == NULL) {
        return -ZAM6222X_EINVAL;
    }

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    *p_faults = (__ZAM6222_CONFIG(config) >> 3) & 0x3;

    return ZAM6222X_OK;
}

/**
 * \brief 设置 ZAM6222B 的测试方式
 */
int zam6222x_meas_mode_set (void *p_hal, uint8_t mode)
{
    int error;
    uint8_t config[2];
    
    if (mode > ZAM6222X_SD_THREE_WIRE) {
        return -ZAM6222X_EINVAL;
    }
    
    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }
    
    /* Update configuration */
    config[0] &= ~(0x1);
    config[0] |= mode;
    config[1] &= ~(0x1);
    config[1] |= mode;
    
    /* Write the latest configuration */
    return zam6222x_hal_i2c_write(p_hal, 0x1, &config[0], 0x2);
}

/**
 * \brief 读取 ZAM6222X 的测试方式
 */
int zam6222x_meas_mode_get (void *p_hal, uint8_t *p_mode)
{
    int     error;
    uint8_t config[2];

    if (p_mode == NULL) {
        return -ZAM6222X_EINVAL;
    }

    /* Get the current configuration */
    error = zam6222x_hal_i2c_read(p_hal, 0x1, &config[0], 0x2);
    if (error != ZAM6222X_OK) {
        return error;
    }

    *p_mode = __ZAM6222_CONFIG(config) & 0x1;

    return ZAM6222X_OK;
}
 int zam6222x_cfg_init(void *p_hal)
{
    int      ret           = 0;
   
    /* 设置通道1上限值和下限值 */
    ret = zam6222x_threshold_set(p_hal, ZAM6222X_CHAN1, 1023.999878f, -0.000122f);
    if (ret != ZAM6222X_OK) {
        printf("Set the threshold of CH1 failed! ret = %d\r\n", ret);
        return ret;
    }
    
    /* 设置通道2上限值和下限值 */
    ret = zam6222x_threshold_set(p_hal, ZAM6222X_CHAN2, 1023.999878f, -0.000122f);
    if (ret != ZAM6222X_OK) {
        printf("Set the threshold of CH2 failed! ret = %d\r\n", ret);
        return ret;
    }
    
    
    /* 设置温度报警检测模式和极性 */
    ret = zam6222x_alert_cfg_set(p_hal, ZAM6222X_POL1, ZAM6222X_TM_CMP);
    if (ret != ZAM6222X_OK) {
        printf("Configure the alert mode and pol failed! ret = %d\r\n", ret);
        return ret;
    }
    
    /* 设置触发ALERT信号的温度值测试个数为6 */
    ret = zam6222x_alert_faults_set(p_hal, ZAM6222X_FAULTS_6);
    if (ret != ZAM6222X_OK) {
        printf("Set the number of temperature value that trigger the ALERT signal failed!\r\n");
        return ret;
    }
    
    /* 设置ALERT脚输出温度报警监测的通道为 通道1 */
    ret = zam6222x_alert_out_set(p_hal, ZAM6222X_CHAN1);
    if (ret != ZAM6222X_OK) {
        printf("Set alert out failed! ret = %d\r\n", ret);
        return ret;
    }
    

    ret = zam6222x_rate_set(p_hal, ZAM6222X_RATE_40HZ);
    if (ret != ZAM6222X_OK) {
        printf("Set the sampling rate of ADC inside the sensor failed! ret = %d\r\n", ret);
        return ret;
    }

    
    return ret;
}

/**
 * \brief entry
 */
int zam6222x_temperature_entry (void *p_hal)
{
	uint8_t  i=0;
    int   error;
//    float temperature0;
//    float temperature1;

    (void)zam6222x_init(p_hal);
	zam6222x_threshold_set(p_hal, ZAM6222X_CHAN1, 50.0f, -10.0f);
	 zam6222x_alert_cfg_set(p_hal, ZAM6222X_POL1, ZAM6222X_TM_CMP);
	zam6222x_alert_faults_set(p_hal, ZAM6222X_FAULTS_2);
	zam6222x_alert_out_set(p_hal, ZAM6222X_CHAN1);
	zam6222x_rate_set(p_hal, ZAM6222X_RATE_40HZ);
	
	
	
    /* 将zam6222x 模块的配置初始化为默认值 */
//    error = zam6222x_cfg_init(p_hal);
//    if (error != ZAM6222X_OK) {
//        return error;
//    }

    while (1) {

        /* 获取zam6222x 模块两个通道的温度值 */
        error = zam6222x_value_read(p_hal, &temperature0, &temperature1);
			  sprintf(TMP1,"%.2f",temperature0);
        if (error == ZAM6222X_OK) {
					RS485_TX_EN();
					RS485_SendString(TMP1);
					RS485_RX_EN();
					
//					curTmp = temperature0;
//					OLED_P8x16Num(30, 0, curTmp, 2);
//					OLED_P8x16Str(30, 0, curTmp); 
//						OLED_P8x16Str(100, 0, "~C"); 
//            printf("chanl1 temperature = %0.3f\nchanl2 temperature = %0.3f\n",
//                   temperature0,
////                   temperature1);


//			   printf("chanl1 temperature = %0.2f\n",temperature0);
        } else {
							RS485_TX_EN();
					RS485_SendString("Read failed!Return %d\r\n");
					RS485_RX_EN();

        }
        
        /* 延时 1000ms */
        delay_ms(5000);
    }
}

