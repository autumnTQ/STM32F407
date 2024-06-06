/**
 * \file
 * \brief zam6228模块驱动源文件
 * 
 * \internal
 * \par modification history:
 * - 1.00 23-03-28, first implementation.
 * \endinternal
 */

#include "zam6228.h"
#include "main.h"
float      temp_date[8] = {0};  //存放温度数据的缓存区


/**
 * \brief zam6228读数据
 */
static int __zam6228_read_data(void  *p_dev, uint16_t memaddr, uint16_t memsize, uint8_t *p_data, uint16_t size)
{

	 uint8_t   i2c_status = 0;
    uint32_t  dev_addr = (uint32_t)p_dev;


    i2c_status = gpio_i2c_read((dev_addr << 1),
                               memaddr,
                               1,
                               p_data,
                               size);

    if (i2c_status != 0) {
        return -1;
    }

    return 0;
	
}

/**
 * \brief zam6228写数据
 */
static int __zam6228_write_data(void  *p_dev, uint16_t memaddr, uint16_t memsize, uint8_t *p_data, uint16_t size)
{

	  uint8_t   i2c_status = 0;
    uint32_t  dev_addr = (uint32_t)p_dev;


    i2c_status = gpio_i2c_write((dev_addr << 1),
                                memaddr,
                                1,
                                p_data,
                                size);

    if (i2c_status != 0) {
        return -1;
    }

    return 0;
	
	
}

/**
 * \brief 将读取到的寄存器值转为温度值
 */
static float __zam6228_reg_to_temp(uint8_t *p_data)
{
    uint32_t temp = 0;
    temp |= p_data[0] << 16;
    temp |= p_data[1] << 8;
    temp |= p_data[2];
    
    //当前测量温度为负温度值
    if(temp >= 8388608){
        return (-(float)((16777216 - temp)/(float)(0x1<<13)));
    } else {
        return ((float)(temp/(float)(0x1<<13)));
    }
}

/**
 * \brief 获取寄存器通道的温度值
 */
int zam6228_get_temp(void *p_dev, uint16_t memaddr, int8_t chan, float *p_data)
{
    int ret = 0;
    float    temp_buff[8] = {0};
    uint8_t  rx_buff[24] = {0};

    if ( p_dev == NULL || p_data == NULL) {
        return ZAM6228_VALUE_ERROR;
    }
    if (chan < 0 || chan > 7) {
        return ZAM6228_VALUE_ERROR;
    }
    
    ret = __zam6228_read_data(p_dev, memaddr, 1, &rx_buff[0], ZAM6228_TEMP_LEN);
    if (ret < 0) {
        return ZAM6228_FUN_ERROR;
    }
    
    switch (chan) {
        case 0:
            temp_buff[0] = __zam6228_reg_to_temp(&rx_buff[0]);
            *p_data = temp_buff[0];
            break;
        case 1:
            temp_buff[1] = __zam6228_reg_to_temp(&rx_buff[3]);
            *p_data = temp_buff[1];
            break;
        case 2:
            temp_buff[2] = __zam6228_reg_to_temp(&rx_buff[6]);
            *p_data = temp_buff[2];
            break;
        case 3:
            temp_buff[3] = __zam6228_reg_to_temp(&rx_buff[9]);
            *p_data = temp_buff[3];
            break;
        case 4:
            temp_buff[4] = __zam6228_reg_to_temp(&rx_buff[12]);
            *p_data = temp_buff[4];
            break;
        case 5:
            temp_buff[5] = __zam6228_reg_to_temp(&rx_buff[15]);
            *p_data = temp_buff[5];
            break;
        case 6:
            temp_buff[6] = __zam6228_reg_to_temp(&rx_buff[18]);
            *p_data = temp_buff[6];
            break;
        case 7:
            temp_buff[7] = __zam6228_reg_to_temp(&rx_buff[21]);
            *p_data = temp_buff[7];
            break;
        default:
            break;
    }

    return ZAM6228_FUN_OK;
}

/**
 * \brief 设置通道使能
 */
int zam6228_set_chan_en(void  *p_dev, uint16_t memaddr,int8_t chan_en)
{
    uint8_t __chan_en = chan_en;
    int     ret;
    
    if (p_dev == NULL) {
        return ZAM6228_VALUE_ERROR;
    }
    
    ret = __zam6228_write_data(p_dev, memaddr, 1, &__chan_en, ZAM6228_CHAN_EN_LEN);
    if (ret < 0) {
        return ZAM6228_FUN_ERROR;
    }
    
    return ZAM6228_FUN_OK;
}

//int  zam6228_demo_entry(void)
//{
//    char       info[256] = {0};
//    int        ret = 0;
// 
//    uint8_t    __chan_en;
//    int        i;

//    /* 初始化一个zam6228设备 */

//   
//    /* 配置模块使能通道，这里默认配置8通道使能*/
//    __chan_en =   ZAM6228_CHAN1_EN | ZAM6228_CHAN2_EN | ZAM6228_CHAN3_EN | ZAM6228_CHAN4_EN | ZAM6228_CHAN5_EN | ZAM6228_CHAN6_EN | ZAM6228_CHAN7_EN | ZAM6228_CHAN8_EN;
//    
//    ret = zam6228_set_chan_en((void *)0x48, ZAM6228_CHAN_EN_ADDR,__chan_en) ;
//    if(ret < 0){
//       return -1;
//    }

//    
//    while (1) {

//        /* 获取通道1采集到的温度数据 */
//        ret = zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN1, &temp_date[0]);
//        if(ret < 0){
//            return -1;
//        }
//        /* 获取通道2采集到的温度数据 */
//        ret = zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN2, &temp_date[1]);
//        if(ret < 0){
//            return -1;
//        }
//        /* 获取通道3采集到的温度数据 */
//        ret = zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN3, &temp_date[2]);
//        if(ret < 0){
//            return -1;
//        }
//        /* 获取通道4采集到的温度数据 */
//        ret = zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN4, &temp_date[3]);
//        if(ret < 0){
//            return -1;
//        }
//        /* 获取通道5采集到的温度数据 */
//        ret = zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN5, &temp_date[4]);
//        if(ret < 0){
//            return -1;
//        }
//        /* 获取通道6采集到的温度数据 */
//        ret = zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN6, &temp_date[5]);
//        if(ret < 0){
//            return -1;
//        }
//        /* 获取通道7采集到的温度数据 */
//        ret = zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN7, &temp_date[6]);
//        if(ret < 0){
//            return -1;
//        }
//        /* 获取通道8采集到的温度数据 */
//        ret = zam6228_get_temp((void *)0x48, ZAM6228_TEMP_ADDR, ZAM6228_CHAN8, &temp_date[7]);
//        if(ret < 0){
//            return -1;
//        }
//        
//        for (i = 0;i<8;i++) {
//            // 输出温度值 精度为0.001 
////            sprintf(info,"chanl%d temperature = %0.3f\n",i+1,temp_date[i]);
//					usart3_mode(TX);
//					printf("chanl%d temperature = %0.3f\n",i+1,temp_date[i]);
//					delay_ms(2000);					
//        }   
//        delay_ms(2000);

//			}
//					return 0;
// 
//}




/** end of file **/
