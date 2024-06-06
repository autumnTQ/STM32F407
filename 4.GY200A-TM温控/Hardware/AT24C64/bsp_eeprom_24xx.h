/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx02����ģ��
*	�ļ����� : bsp_eeprom_24xx.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2012-10-12 armfly  ST�̼���汾 V2.1.0
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_EEPROM_24XX_H
#define _BSP_EEPROM_24XX_H
#include "stm32f10x.h"
#include "bsp_i2c_gpio.h"

//EEPROM��ַ����
#define DD_EEPROM_PID       0x0000  //PID��Ϣ��EEPROM�׵�ַ   ��1ҳ
#define DD_EEPROM_TMP       0x0060  //���ò�����EEPROM�׵�ַ  ��4ҳ
#define DD_EEPROM_ADDR      0x0080  //���ò�����EEPROM�׵�ַ  ��5ҳ
//0x0060  ��4ҳ    0x0080   ��5ҳ   P��I��D�����ֱ��ŵ�1-3ҳ

//#define AT24C02
#define AT24C64

#ifdef AT24C02
	#define EE_MODEL_NAME		"AT24C02"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		8			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				256			/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
#endif

#ifdef AT24C128
	#define EE_MODEL_NAME		"AT24C128"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		64			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				(16*1024)	/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */
#endif
#ifdef AT24C64
	#define EE_MODEL_NAME		"AT24C64"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		32			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				(16*512)	/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */
#endif
uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
