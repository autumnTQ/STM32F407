#include "Config.h"

/*
 *@attention
 *SCK -- PA5
 *MISO -- PA6
 *MOSI -- PA7
 *CS-- PA4
 */
u16 ReadAngle_init = 0;

u32 TLE5012B_Aangle = 0; // 角度

extern uint16_t WriteValue(uint16_t u16RegValue, uint16_t u16RegValue_1);

/*******************************************************************************
 * 函数名       :
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void SPI5012B_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	// CLK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Set CS pin high to disable device
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI_CS_DISABLE;

	SPI_Cmd(SPI2, DISABLE);												// 失能SPI2
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	// 设置SPI位双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						// 主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;					// 16位数据针
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							// 空闲时钟低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						// 时钟第二个边沿采集型号
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							// 软件控制NSS片选信号
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; // 32分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					// 先传输高位
	SPI_InitStructure.SPI_CRCPolynomial = 7;							// 默认
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_Cmd(SPI2, ENABLE);
	Delay_US(500);
}

/*******************************************************************************
 * 函数名       : 数据读写
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
uint16_t SPIx_ReadWriteByte(uint16_t byte)
{
	uint16_t retry = 0;
	while ((SPI2->SR & 1 << 1) == 0) // 发送缓冲器非空
	{
		if (++retry > 200)
			return 0; // 等待一段时间退出
	}
	SPI2->DR = byte; // 发送数据

	retry = 0;
	while ((SPI2->SR & 1 << 0) == 0) // 接收缓冲器为空
	{
		if (++retry > 200)
			return 0; // 当代一段时间返回
	}
	return SPI2->DR; // 返回接收缓冲器内容
}
/*******************************************************************************
 * 函数名     : 读角速度
 * 函数功能	 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
uint16_t ReadSpeed(void)
{
	u16 a_value = 0;
	u32 a;
	float f = 0;
	a_value = ReadValue(READ_SPEED_VALUE);
	a_value &= 0x7FFF; // 去掉状态位

	f = a_value;
	f = f * 360 / 32768;
	a = (u16)(f / 2);
	return (a);
}
/*******************************************************************************
 * 函数名       : 读寄存器值
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
uint16_t ReadValue(uint16_t u16RegValue)
{
	uint16_t u16Data;

	SPI_CS_ENABLE;     // 控制MOSI--PB15的输出类型为开漏或推挽。发送命令时设置MOSI为推挽输出，接收数据时设置MOSI为开漏，以实现正常通信。
					  
					   //   MCU向TLE5012B发送命令时 MCU的数据接收引脚MISO引脚拉高 。当TLE5012B向MCU传输数据时，MCU的接收命令时MOSI引脚拉高         
	SPIx_ReadWriteByte(u16RegValue);
	SPI_TX_OFF;
    Delay_US(100);
	u16Data = SPIx_ReadWriteByte(0xffff);
	SPI_CS_DISABLE;
	SPI_TX_ON;

	return (u16Data);
}
/*******************************************************************************
 * 函数名       : 写寄存器值
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
uint16_t WriteValue(uint16_t u16RegValue, uint16_t u16RegValue_1)
{
	uint16_t u16Data;

	SPI_CS_ENABLE;

	SPIx_ReadWriteByte(u16RegValue);
	SPIx_ReadWriteByte(u16RegValue_1);

	SPI_TX_OFF;
    
    Delay_US(100);
	u16Data = SPIx_ReadWriteByte(0xffff);

	SPI_CS_DISABLE;
	SPI_TX_ON;
	return (u16Data);
}
/*******************************************************************************
 * 函数名       : 读角度0-359
 * 函数功能	  :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void ReadAngle(void)
{
	static u16 n = 0;
	static u32 last_value = 0; // 上次的角度
	u16 a_value = 0;
	u32 a;
	float f = 0.0, f1 = 0.0;

	//-------采集角度--------------------
	a_value = ReadValue(READ_ANGLE_VALUE);
//	a_value = ReadValue(0x8421);
	a_value &= 0x7FFF; // 去掉状态位
	a_value <<= 1;	   //
	f = a_value;
	f = f * 360 / 0x10000;
	a = (u16)(f * 100);

	TLE5012B_Aangle = a;
}
