/**
 * \file
 * \brief zam6228Ä£¿éÇý¶¯Í·ÎÄ¼þ
 * 
 * \internal
 * \par modification history:
 * - 1.00 23-03-28, first implementation.
 * \endinternal
 */

#include "main.h"

#define ZAM6228_FUN_OK        0                     //ZAM6228º¯ÊýÏà¹Ø½Ó¿Ú²Ù×÷³É¹¦
#define ZAM6228_FUN_ERROR    -1                     //ZAM6228º¯ÊýÏà¹Ø½Ó¿Ú²Ù×÷Ê§°Ü
#define ZAM6228_VALUE_ERROR  -2                     //ZAM6228º¯ÊýµÄ´«Èë²ÎÊý´íÎó


/**
 * \broef ZAM6228Éè±¸Í¨µÀ
 */
#define ZAM6228_CHAN1         0                     //ZAM6228Éè±¸Í¨µÀ1
#define ZAM6228_CHAN2         1                     //ZAM6228Éè±¸Í¨µÀ2
#define ZAM6228_CHAN3         2                     //ZAM6228Éè±¸Í¨µÀ3
#define ZAM6228_CHAN4         3                     //ZAM6228Éè±¸Í¨µÀ4
#define ZAM6228_CHAN5         4                     //ZAM6228Éè±¸Í¨µÀ5
#define ZAM6228_CHAN6         5                     //ZAM6228Éè±¸Í¨µÀ6
#define ZAM6228_CHAN7         6                     //ZAM6228Éè±¸Í¨µÀ7
#define ZAM6228_CHAN8         7                     //ZAM6228Éè±¸Í¨µÀ8
/**
 * \broef ZAM6228Éè±¸Í¨µÀÊ¹ÄÜ
 */
#define ZAM6228_CHAN1_EN       1 << 0               //ZAM6228Í¨µÀ1Ê¹ÄÜ
#define ZAM6228_CHAN2_EN       1 << 1               //ZAM6228Í¨µÀ2Ê¹ÄÜ
#define ZAM6228_CHAN3_EN       1 << 2               //ZAM6228Í¨µÀ3Ê¹ÄÜ
#define ZAM6228_CHAN4_EN       1 << 3               //ZAM6228Í¨µÀ4Ê¹ÄÜ
#define ZAM6228_CHAN5_EN       1 << 4               //ZAM6228Í¨µÀ5Ê¹ÄÜ
#define ZAM6228_CHAN6_EN       1 << 5               //ZAM6228Í¨µÀ6Ê¹ÄÜ
#define ZAM6228_CHAN7_EN       1 << 6               //ZAM6228Í¨µÀ7Ê¹ÄÜ
#define ZAM6228_CHAN8_EN       1 << 7               //ZAM6228Í¨µÀ8Ê¹ÄÜ

 /**
 * \broef ×Ó¼Ä´æÆ÷µØÖ·
 */
#define ZAM6228_TEMP_ADDR    (0X00)                     //ÎÂ¶È×Ó¼Ä´æÆ÷µØÖ·
#define ZAM6228_TEMP_LEN     (0X18)                     //ÎÂ¶È×Ó¼Ä´æÆ÷³¤¶È

#define ZAM6228_CHAN_EN_ADDR (0X0C)                     //Í¨µÀÊ¹ÄÜ×Ó¼Ä´æÆ÷µØÖ·
#define ZAM6228_CHAN_EN_LEN  (0X01)                     //Í¨µÀÊ¹ÄÜ×Ó¼Ä´æÆ÷³¤¶È


/**
 * \broef ZAM6228Éè±¸½á¹¹Ìå
 */
//typedef struct zlg_zam6228_device {
//    I2C_InitTypeDef *hi2c;                /* i2cÉè±¸µÄ¾ä±ú */
//    uint16_t           zam6228_addr;         /* ZAM6228Ä£¿éµØÖ· */  
//}zlg_zam6228_device_t;



/**
 * \brief »ñÈ¡¼Ä´æÆ÷Í¨µÀµÄÎÂ¶ÈÖµ
 * 
 * 
 * \param[in] p_dev ZAM6228Éè±¸
 * \param[in] chan Í¨µÀ 
 * \param[in] memaddr ×Ó¼Ä´æÆ÷µØÖ·£¬
 * \param[out] p_data ´æ·ÅÎÂ¶ÈÊý¾ÝµÄ»º´æÇø
 * 
 * \retval ZAM6228_FUN_OK       ÅäÖÃ³É¹¦
 * \retval ZAM6228_FUN_ERROR    ÅäÖÃÊ§°Ü
 * \retval ZAM6228_VALUE_ERROR  ²ÎÊý´íÎó
 */
int zam6228_get_temp(void *p_dev, uint16_t memaddr, int8_t chan, float *p_data);



/**
 * \brief 
 * 
 * \param[in] p_dev   ZAM6228Éè±¸
 * \param[in] memaddr ×Ó¼Ä´æÆ÷µØÖ·£
 * \param[in] chan_en ÐèÒªÑ¡ÔñµÄÊ¹ÄÜÍ¨µÀ

 * 
 * \retval ZAM6228_FUN_OK    ÅäÖÃ³É¹¦
 * \retval ZAM6228_FUN_ERROR ÅäÖÃÊ§°Ü
 * \retval ZAM6228_VALUE_ERROR  ²ÎÊý´íÎó
 */
int  zam6228_set_chan_en(void *p_dev, uint16_t memaddr,int8_t chan_en) ;
int zam6228_demo_entry(void);



/** end of file **/
