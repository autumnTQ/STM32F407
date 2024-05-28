
#ifndef _TLE5012B_H_
#define _TLE5012B_H_

#include "stm32f10x.h"

// 把PB15(MOSI)复用开漏输出
#define SPI_TX_OFF                  \
     {                              \
          GPIOB->CRH &= 0x0FFFFFFF; \
          GPIOB->CRH |= 0xF0000000; \
     }
// 把PB15(MOSI)复用推挽输出
#define SPI_TX_ON                   \
     {                              \
          GPIOB->CRH &= 0x0FFFFFFF; \
          GPIOB->CRH |= 0xB0000000; \
     }

// 片选引脚
#define SPI_CS_ENABLE GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define SPI_CS_DISABLE GPIO_SetBits(GPIOB, GPIO_Pin_12)

/* SPI command for TLE5012 */
#define READ_STATUS 0x8001      // 8000  1_0000_0_000000_0001
#define READ_ANGLE_VALUE 0x8021 // 8021  1_0000_0_000010_0001   读取角度值
#define READ_SPEED_VALUE 0x8031 // 8031
#define READ_AREV_VALUE 0x8041
#define READ_MOD_3 0x8091       // 8009  1_0000_0_001001_0001
#define WRITE_MOD1_VALUE 0x5060 // 0_1010_0_000110_0001
#define MOD1_VALUE 0x0001
#define WRITE_MOD2_VALUE 0x5080 // 0_1010_0_001000_0001
#define MOD2_VALUE 0x0801
#define WRITE_MOD3_VALUE 0x5091 // 0_1010_0_001001_0001
#define MOD3_VALUE 0x0000
#define WRITE_MOD4_VALUE 0x50E0 // 0_1010_0_001110_0001
#define MOD4_VALUE 0x0098       // 9bit 512
#define WRITE_IFAB_VALUE 0x50B1
#define IFAB_VALUE 0x000D
  
/* Functionality mode */
#define REFERESH_ANGLE 0

extern u32 TLE5012B_Aangle; // 角度

void SPI5012B_Init(void);
void SPI_SendData16(uint16_t SendData);
uint16_t SPI_ReadData16(void);
void ReadAngle(void);
uint16_t ReadSpeed(void);
uint16_t ReadValue(uint16_t u16Value);
uint16_t SPIx_ReadWriteByte(uint16_t byte);
uint16_t TlE5012W_Reg(uint16_t Reg_CMD, uint16_t Reg_Data);

uint16_t WriteValue(uint16_t u16RegValue, uint16_t u16RegValue_1);


#endif
