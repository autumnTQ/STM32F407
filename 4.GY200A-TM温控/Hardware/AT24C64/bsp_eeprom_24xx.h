/*
*********************************************************************************************************
*
*	模块名称 : 串行EEPROM 24xx02驱动模块
*	文件名称 : bsp_eeprom_24xx.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2012-10-12 armfly  ST固件库版本 V2.1.0
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_EEPROM_24XX_H
#define _BSP_EEPROM_24XX_H
#include "stm32f10x.h"
#include "bsp_i2c_gpio.h"

//EEPROM地址分配
#define DD_EEPROM_PID       0x0000  //PID信息组EEPROM首地址   第1页
#define DD_EEPROM_TMP       0x0060  //配置参数组EEPROM首地址  第4页
#define DD_EEPROM_ADDR      0x0080  //配置参数组EEPROM首地址  第5页
//0x0060  第4页    0x0080   第5页   P、I、D参数分别存放第1-3页

//#define AT24C02
#define AT24C64

#ifdef AT24C02
	#define EE_MODEL_NAME		"AT24C02"
	#define EE_DEV_ADDR			0xA0		/* 设备地址 */
	#define EE_PAGE_SIZE		8			/* 页面大小(字节) */
	#define EE_SIZE				256			/* 总容量(字节) */
	#define EE_ADDR_BYTES		1			/* 地址字节个数 */
#endif

#ifdef AT24C128
	#define EE_MODEL_NAME		"AT24C128"
	#define EE_DEV_ADDR			0xA0		/* 设备地址 */
	#define EE_PAGE_SIZE		64			/* 页面大小(字节) */
	#define EE_SIZE				(16*1024)	/* 总容量(字节) */
	#define EE_ADDR_BYTES		2			/* 地址字节个数 */
#endif
#ifdef AT24C64
	#define EE_MODEL_NAME		"AT24C64"
	#define EE_DEV_ADDR			0xA0		/* 设备地址 */
	#define EE_PAGE_SIZE		32			/* 页面大小(字节) */
	#define EE_SIZE				(16*512)	/* 总容量(字节) */
	#define EE_ADDR_BYTES		2			/* 地址字节个数 */
#endif
uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
