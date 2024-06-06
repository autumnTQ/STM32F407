/**
 * \file
 * \brief zam6228ģ������ͷ�ļ�
 * 
 * \internal
 * \par modification history:
 * - 1.00 23-03-28, first implementation.
 * \endinternal
 */

#include "main.h"

#define ZAM6228_FUN_OK        0                     //ZAM6228������ؽӿڲ����ɹ�
#define ZAM6228_FUN_ERROR    -1                     //ZAM6228������ؽӿڲ���ʧ��
#define ZAM6228_VALUE_ERROR  -2                     //ZAM6228�����Ĵ����������


/**
 * \broef ZAM6228�豸ͨ��
 */
#define ZAM6228_CHAN1         0                     //ZAM6228�豸ͨ��1
#define ZAM6228_CHAN2         1                     //ZAM6228�豸ͨ��2
#define ZAM6228_CHAN3         2                     //ZAM6228�豸ͨ��3
#define ZAM6228_CHAN4         3                     //ZAM6228�豸ͨ��4
#define ZAM6228_CHAN5         4                     //ZAM6228�豸ͨ��5
#define ZAM6228_CHAN6         5                     //ZAM6228�豸ͨ��6
#define ZAM6228_CHAN7         6                     //ZAM6228�豸ͨ��7
#define ZAM6228_CHAN8         7                     //ZAM6228�豸ͨ��8
/**
 * \broef ZAM6228�豸ͨ��ʹ��
 */
#define ZAM6228_CHAN1_EN       1 << 0               //ZAM6228ͨ��1ʹ��
#define ZAM6228_CHAN2_EN       1 << 1               //ZAM6228ͨ��2ʹ��
#define ZAM6228_CHAN3_EN       1 << 2               //ZAM6228ͨ��3ʹ��
#define ZAM6228_CHAN4_EN       1 << 3               //ZAM6228ͨ��4ʹ��
#define ZAM6228_CHAN5_EN       1 << 4               //ZAM6228ͨ��5ʹ��
#define ZAM6228_CHAN6_EN       1 << 5               //ZAM6228ͨ��6ʹ��
#define ZAM6228_CHAN7_EN       1 << 6               //ZAM6228ͨ��7ʹ��
#define ZAM6228_CHAN8_EN       1 << 7               //ZAM6228ͨ��8ʹ��

 /**
 * \broef �ӼĴ�����ַ
 */
#define ZAM6228_TEMP_ADDR    (0X00)                     //�¶��ӼĴ�����ַ
#define ZAM6228_TEMP_LEN     (0X18)                     //�¶��ӼĴ�������

#define ZAM6228_CHAN_EN_ADDR (0X0C)                     //ͨ��ʹ���ӼĴ�����ַ
#define ZAM6228_CHAN_EN_LEN  (0X01)                     //ͨ��ʹ���ӼĴ�������


/**
 * \broef ZAM6228�豸�ṹ��
 */
//typedef struct zlg_zam6228_device {
//    I2C_InitTypeDef *hi2c;                /* i2c�豸�ľ�� */
//    uint16_t           zam6228_addr;         /* ZAM6228ģ���ַ */  
//}zlg_zam6228_device_t;



/**
 * \brief ��ȡ�Ĵ���ͨ�����¶�ֵ
 * 
 * 
 * \param[in] p_dev ZAM6228�豸
 * \param[in] chan ͨ�� 
 * \param[in] memaddr �ӼĴ�����ַ��
 * \param[out] p_data ����¶����ݵĻ�����
 * 
 * \retval ZAM6228_FUN_OK       ���óɹ�
 * \retval ZAM6228_FUN_ERROR    ����ʧ��
 * \retval ZAM6228_VALUE_ERROR  ��������
 */
int zam6228_get_temp(void *p_dev, uint16_t memaddr, int8_t chan, float *p_data);



/**
 * \brief 
 * 
 * \param[in] p_dev   ZAM6228�豸
 * \param[in] memaddr �ӼĴ�����ַ�
 * \param[in] chan_en ��Ҫѡ���ʹ��ͨ��

 * 
 * \retval ZAM6228_FUN_OK    ���óɹ�
 * \retval ZAM6228_FUN_ERROR ����ʧ��
 * \retval ZAM6228_VALUE_ERROR  ��������
 */
int  zam6228_set_chan_en(void *p_dev, uint16_t memaddr,int8_t chan_en) ;
int zam6228_demo_entry(void);



/** end of file **/
