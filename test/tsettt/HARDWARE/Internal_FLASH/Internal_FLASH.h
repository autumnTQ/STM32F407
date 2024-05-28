#ifndef __INTERNAL_FLASH_H
#define __INTERNAL_FLASH_H

#include "stm32f4xx.h"
#include "usart.h"
#include "common.h"

#define ADDR_FLASH_SECTOR_10 ((uint32_t)0x080C0000)
#define ADDR_FLASH_SECTOR_11 ((uint32_t)0x080E0000)

FLASH_Status Internal_FLASH_Write(uint32_t FLASH_Sector, uint32_t FLASH_Place, char * data, int dataLength);
void Internal_FLASH_Read(uint32_t FLASH_Place, char* Read_Data, int dataLength);

#endif
