#include "Config.h"

#define SPI1_CS_ENABLE GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI1_CS_DISABLE GPIO_SetBits(GPIOA, GPIO_Pin_4)

uint16_t leat_driver_current = 0; // 上次驱动电流
uint16_t leat_xi_fen = 0;		  // 上次细分
// long XTARGET = 0;
uint16_t STATUS = 0;
extern uint32_t motor_step;
char Spi_Status; // PAGE23:SPI返回状态位
long ReadPosition;
long PositionData = 0; // 定义目标位置值
long ENC_DATA;
/*******************************************************************************
 * 函数名       :
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void TMC5160_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// CLK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Set CS pin high to disable device
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI1_CS_DISABLE; //

	SPI_Cmd(SPI1, DISABLE);												// 失能SPI1
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	// 设置SPI位双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						// 主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					// 8位数据针
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;							// 空闲时钟高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						// 时钟第二个边沿采集型号
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							// 软件控制NSS片选信号
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // 32分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					// 先传输高位
	SPI_InitStructure.SPI_CRCPolynomial = 7;							// 默认
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
}
/*******************************************************************************
 * 函数名       : 数据读写
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
uint16_t tmc5160_SPI2_ReadWriteByte(uint16_t byte)
{
	uint16_t retry = 0;
	while ((SPI1->SR & 1 << 1) == 0) // 发送缓冲器非空
	{
		if (++retry > 200)
			return 0; // 等待一段时间退出
	}
	SPI1->DR = byte; // 发送数据

	retry = 0;
	while ((SPI1->SR & 1 << 0) == 0) // 接收缓冲器为空
	{
		if (++retry > 200)
			return 0; // 当代一段时间返回
	}
	return SPI1->DR; // 返回接收缓冲器内容
}

/*******************************************************************************
 * 函数名       : 写寄存器值
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
uint32_t TMC5160_SPIWriteInt(uint8_t add, long write_data)
{
	uint8_t TMC_send_buff[5], TMC_recv_buff[5];
	uint32_t recv_data = 0;
	SPI1_CS_ENABLE; // 片选有效
	TMC_send_buff[0] = add | 0x80;
	TMC_send_buff[1] = (write_data >> 24) & 0x000000ff;
	TMC_send_buff[2] = (write_data >> 16) & 0x000000ff;
	TMC_send_buff[3] = (write_data >> 8) & 0x000000ff;
	TMC_send_buff[4] = (write_data & 0x000000ff);

	TMC_recv_buff[0] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[0]);
	TMC_recv_buff[1] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[1]);
	TMC_recv_buff[2] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[2]);
	TMC_recv_buff[3] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[3]);
	TMC_recv_buff[4] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[4]);
	SPI1_CS_DISABLE; // 片选无效

	recv_data = TMC_recv_buff[0];
	return (recv_data);
}

/*******************************************************************************
 * 函数名       : 读寄存器值
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
uint32_t TMC5160_SPIReadInt(uint8_t add, long write_data)
{
	uint8_t TMC_send_buff[5], TMC_recv_buff[5];
	uint32_t recv_data = 0;
	SPI1_CS_ENABLE; // 片选有效
	TMC_send_buff[0] = add | 0x00;
	TMC_send_buff[1] = (write_data >> 24) & 0x000000ff;
	TMC_send_buff[2] = (write_data >> 16) & 0x000000ff;
	TMC_send_buff[3] = (write_data >> 8) & 0x000000ff;
	TMC_send_buff[4] = (write_data & 0x000000ff);

	TMC_recv_buff[0] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[0]);
	TMC_recv_buff[1] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[1]);
	TMC_recv_buff[2] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[2]);
	TMC_recv_buff[3] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[3]);
	TMC_recv_buff[4] = tmc5160_SPI2_ReadWriteByte(TMC_send_buff[4]);
	SPI1_CS_DISABLE; // 片选无效

	STATUS = TMC_recv_buff[0]; // 状态位

	recv_data = TMC_recv_buff[1];
	recv_data <<= 8;
	recv_data |= TMC_recv_buff[2];
	recv_data <<= 8;
	recv_data |= TMC_recv_buff[3];
	recv_data <<= 8;
	recv_data |= TMC_recv_buff[4];

	return (recv_data);
}

/*******************************************************************************
 * 函数名       :
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void TMC5160_Regest_Init(void)
{
	//---------------------------
	// 使能 stealthChop 电压 PWM 模式 mode (取决于速度阈值
	TMC5160_SPIWriteInt(0x00, 0x00000004); // writing value 0x0000000C = 12 = 0.0 to address 0 = 0x00(GCONF)
	TMC5160_SPIWriteInt(0x03, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 1 = 0x03(SLAVECONF)
	TMC5160_SPIWriteInt(0x05, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 2 = 0x05(X_COMPARE)
	TMC5160_SPIWriteInt(0x06, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 3 = 0x06(OTP_PROG)
	TMC5160_SPIWriteInt(0x08, 0x0000000E); // writing value 0x00000011 = 17 = 0.0 to address 4 = 0x08(FACTORY_CONF)
	TMC5160_SPIWriteInt(0x09, 0x00010606); // writing value 0x00010606 = 67078 = 0.0 to address 5 = 0x09(SHORT_CONF)
	TMC5160_SPIWriteInt(0x0A, 0x00080400); // writing value 0x00080400 = 525312 = 0.0 to address 6 = 0x0A(DRV_CONF)
	TMC5160_SPIWriteInt(0x0B, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 7 = 0x0B(GLOBAL_SCALER)
	// 速度相关的驱动控制寄存器
	TMC5160_SPIWriteInt(0x10, 0x00000005); // writing value 0x00070707 = 460551 = 0.0 to address 8 = 0x10(IHOLD_IRUN)
	TMC5160_SPIWriteInt(0x11, 0x0000000A); // writing value 0x0000000A = 10 = 0.0 to address 9 = 0x11(TPOWERDOWN)
	TMC5160_SPIWriteInt(0x13, 0x00000010); // writing value 0x00000041 = 65 = 0.0 to address 10 = 0x13(TPWMTHRS)
	TMC5160_SPIWriteInt(0x14, 0x00000010); // writing value 0x00004189 = 16777 = 0.0 to address 11 = 0x14(TCOOLTHRS)
	TMC5160_SPIWriteInt(0x15, 0x00000010); // writing value 0x00000000 = 0 = 0.0 to address 12 = 0x15(THIGH)
	// 斜波发生器运动寄存器
	TMC5160_SPIWriteInt(0x20, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 13 = 0x20(RAMPMODE)
	TMC5160_SPIWriteInt(0x21, 0x00000000); // writing value 0xFFCC12F0 = 0 = 0.0 to address 14 = 0x21(XACTUAL)
	TMC5160_SPIWriteInt(0x23, 0x00000100); // writing value 0x00000000 = 0 = 0.0 to address 15 = 0x23(VSTART)

	TMC5160_SPIWriteInt(0x24, 0x00001000); // writing value 0x00001F07 = 7943 = 0.0 to address 16 = 0x24(A1)
	TMC5160_SPIWriteInt(0x25, 0x00000000); // writing value 0x0000C350 = 50000 = 0.0 to address 17 = 0x25(V1)
	TMC5160_SPIWriteInt(0x26, 0x00004000); // writing value 0x00004DF1 = 19953 = 0.0 to address 18 = 0x26(AMAX)
	TMC5160_SPIWriteInt(0x27, 0x00010000); // writing value 0x0006D3A0 = 447392 = 0.0 to address 19 = 0x27(VMAX)
	TMC5160_SPIWriteInt(0x28, 0x00004000); // writing value 0x00009B83 = 39811 = 0.0 to address 20 = 0x28(DMAX)
	TMC5160_SPIWriteInt(0x2A, 0x00001000); // writing value 0x00007B87 = 31623 = 0.0 to address 21 = 0x2A(D1)

	TMC5160_SPIWriteInt(0x2B, 0x0000000A); // writing value 0x0000000A = 10 = 0.0 to address 22 = 0x2B(VSTOP)
	TMC5160_SPIWriteInt(0x2C, 0x00000100); // writing value 0x00000000 = 0 = 0.0 to address 23 = 0x2C(TZEROWAIT)
	TMC5160_SPIWriteInt(0x2D, 0x00000000); // writing value 0xFFCC12F0 = 0 = 0.0 to address 24 = 0x2D(XTARGET)

	TMC5160_SPIWriteInt(0x33, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 25 = 0x33(VDCMIN)
	TMC5160_SPIWriteInt(0x34, 0x00000000); // writing value 0x00000400 = 1024 = 0.0 to address 26 = 0x34(SW_MODE)
	// 编码器寄存器
	TMC5160_SPIWriteInt(0x38, 0x00000400); // writing value 0x00000000 = 0 = 0.0 to address 27 = 0x38(ENCMODE)
	// TMC5160_SPIWriteInt(0x38, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 27 = 0x38(ENCMODE)
	TMC5160_SPIWriteInt(0x39, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 28 = 0x39(X_ENC)
	TMC5160_SPIWriteInt(0x3A, 0x00011770); // writing value 0x00010000 = 65536 = 0.0 to address 29 = 0x3A(ENC_CONST)
	TMC5160_SPIWriteInt(0x3D, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 30 = 0x3D(ENC_DEVIATION)
	// 电机驱动寄存器-电机微步控制寄存器
	TMC5160_SPIWriteInt(0x60, 0xAAAAB554); // writing value 0xAAAAB554 = 0 = 0.0 to address 31 = 0x60(MSLUT[0])
	TMC5160_SPIWriteInt(0x61, 0x4A9554AA); // writing value 0x4A9554AA = 1251300522 = 0.0 to address 32 = 0x61(MSLUT[1])
	TMC5160_SPIWriteInt(0x62, 0x24492929); // writing value 0x24492929 = 608774441 = 0.0 to address 33 = 0x62(MSLUT[2])
	TMC5160_SPIWriteInt(0x63, 0x10104222); // writing value 0x10104222 = 269500962 = 0.0 to address 34 = 0x63(MSLUT[3])
	TMC5160_SPIWriteInt(0x64, 0xFBFFFFFF); // writing value 0xFBFFFFFF = 0 = 0.0 to address 35 = 0x64(MSLUT[4])
	TMC5160_SPIWriteInt(0x65, 0xB5BB777D); // writing value 0xB5BB777D = 0 = 0.0 to address 36 = 0x65(MSLUT[5])
	TMC5160_SPIWriteInt(0x66, 0x49295556); // writing value 0x49295556 = 1227445590 = 0.0 to address 37 = 0x66(MSLUT[6])
	TMC5160_SPIWriteInt(0x67, 0x00404222); // writing value 0x00404222 = 4211234 = 0.0 to address 38 = 0x67(MSLUT[7])
	TMC5160_SPIWriteInt(0x68, 0xFFFF8056); // writing value 0xFFFF8056 = 0 = 0.0 to address 39 = 0x68(MSLUTSEL)
	TMC5160_SPIWriteInt(0x69, 0x00F70000); // writing value 0x00F70000 = 16187392 = 0.0 to address 40 = 0x69(MSLUTSTART)
	// 电机驱动寄存器-驱动寄存器组
	TMC5160_SPIWriteInt(0x6C, 0x04410153); // writing value 0x00410153 = 4260179 = 0.0 to address 41 = 0x6C(CHOPCONF)
	TMC5160_SPIWriteInt(0x6D, 0x00000000); // writing value 0x00030000 = 196608 = 0.0 to address 42 = 0x6D(COOLCONF)
	TMC5160_SPIWriteInt(0x6E, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 43 = 0x6E(DCCTRL)
	TMC5160_SPIWriteInt(0x70, 0xC40C001E); // writing value 0xC40C001E = 0 = 0.0 to address 44 = 0x70(PWMCONF)

	//----------------------------
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	// GPIO_SetBits(GPIOB,GPIO_Pin_2);//
}
/*******************************************************************************
 * 函数名       :
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void TMC5160_current_set(void)
{
	uint16_t IRUN_current = 0;	// 运行电流
	uint16_t IHOLD_current = 0; // 静止状态下电机电流
	uint32_t current = 0;

	IRUN_current = sys_param_1[9];
	IHOLD_current = sys_param_1[10];

	if (leat_driver_current != IRUN_current)
	{
		leat_driver_current = IRUN_current;
		current = 0x00060000; // IHOLE延时

		if (IRUN_current >= 32)
			IRUN_current = 32;

		current |= (IRUN_current << 8);
		current |= IHOLD_current;

		TMC5160_SPIWriteInt(0x10, current); // 降电流时钟周期，电机运行电流8-12，停止状态保持电路 0-4
	}
}
/*******************************************************************************
 * 函数名       :
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void TMC5160_xi_fen_set(void)
{
	uint16_t xi_fen = 0; // 细分
	uint32_t xi_F = 0;

	xi_fen = sys_param_1[3]; // 细分
	if (leat_xi_fen != xi_fen)
	{
		leat_xi_fen = xi_fen;
		xi_F = 0x00410153;

		switch (xi_fen)
		{
		case 256:
		{
			xi_F |= 0X00000000;
		}
		break;
		case 128:
		{
			xi_F |= 0X01000000;
		}
		break;
		case 64:
		{
			xi_F |= 0X02000000;
		}
		break;
		case 32:
		{
			xi_F |= 0X03000000;
		}
		break;
		case 16:
		{
			xi_F |= 0X04000000;
		}
		break;
		case 8:
		{
			xi_F |= 0X05000000;
		}
		break;
		case 4:
		{
			xi_F |= 0X06000000;
		}
		break;
		case 2:
		{
			xi_F |= 0X07000000;
		}
		break;
		case 1:
		{
			xi_F |= 0X08000000;
		}
		break;
		case 0:
		{
			xi_F |= 0X00000000;
		}
		break;
		}
		TMC5160_SPIWriteInt(0x6C, xi_F); // 载波器配置,细分（24-27）
	}
}
/*******************************************************************************
 * 函数名       :
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void TMC5160_JiaSu_Set(void) // T型加速设置
{
	static uint32_t leat_AMAX = 0; // 速度
	static uint32_t leat_VMAX = 0; // 速度
	static uint32_t leat_DMAX = 0; // 速度

	long VMAX = sys_param_1[5]; // 运行速度
	long AMAX = sys_param_1[6]; // 加速度
	long DMAX = sys_param_1[7]; // 减速度

	if (leat_AMAX != AMAX)
	{
		leat_AMAX = AMAX;
		TMC5160_SPIWriteInt(0x26, AMAX); // 加速度
		TMC5160_SPIWriteInt(0x28, DMAX); // 减速度
	}
	if (leat_VMAX != VMAX)
	{
		leat_VMAX = VMAX;
		TMC5160_SPIWriteInt(0x27, VMAX); // 运行速度
	}
}
/*******************************************************************************
 * 函数名       :
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void TMC5160_ENCMODE_Set(void) // 编码器设置
{
	float e = 0.0;
	uint32_t ZhengSu = 0, XiaoSu = 0; // 分离出的正数，小数部分
	uint16_t BianMaQi = 4096 * 4;	  // 编码盘1000线  经过AB倍频 4倍频根据电机驱动芯片所制定
	uint16_t BuShu = 200;			  // 360/1.8=200; 一圈多少步
	int32_t value = 0;

	switch (sys_param_1[3]) // 细分设置
	{
	case 256:
	{
		e = 256 * BuShu;
		e /= BianMaQi;
		break;
	}
	case 128:
	{
		e = 128 * BuShu;
		e /= BianMaQi;
		break;
	}
	case 64:
	{
		e = 64 * BuShu;
		e /= BianMaQi;
		break;
	}
	case 32:
	{
		e = 32 * BuShu;
		e /= BianMaQi;
		break;
	}
	case 16:
	{
		e = 16 * BuShu;
		e /= BianMaQi;
		break;
	}
	case 8:
	{
		e = 8 * BuShu;
		e /= BianMaQi;
		break;
	}
	case 4:
	{
		e = 4 * BuShu;
		e /= BianMaQi;
		break;
	}
	case 5:
	{
		e = 3 * BuShu;
		e /= BianMaQi;
		break;
	}
	case 1:
	{
		e = 1 * BuShu;
		e /= BianMaQi;
		break;
	}
	default:
	{
		e = 256 * BuShu;
		e /= BianMaQi;
		break;
	}
	}

	e *= 10000;
	ZhengSu = e / 10000;			// 整数
	XiaoSu = e - (ZhengSu * 10000); // 小数
	e = XiaoSu;
	e /= 10000;

	value = (ZhengSu << 16) + e * 10000;
	// value=(ZhengSu<<16)+e*65536;
	TMC5160_SPIWriteInt(0x3A, value); // 设置编码器因子

	// TMC5160_SPIWriteInt(0x38, 0x00000604); // 配置编码器寄存器 writing value 0x00000000 = 0 = 0.0 to address 27 = 0x38(ENCMODE)
	TMC5160_SPIWriteInt(0x39, 0); // 编码器计数值清理
}
/*******************************************************************************
 * 函数名       :
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void TMC5160_XTARGET_Set(void) // 运行到目标位置设置
{
	static long leat_sys_param26 = 0, leat_sys_param27 = 0;
	static long leat_XTARGET = 0;

	if (leat_sys_param26 != sys_param_1[26] | leat_sys_param27 != sys_param_1[27])
	{
		leat_sys_param26 = sys_param_1[26];
		leat_sys_param27 = sys_param_1[27];

		XTARGET = leat_sys_param27 << 16;		  // 脉冲
		XTARGET |= leat_sys_param26 & 0x0000ffff; // 脉冲

		TMC5160_SPIWriteInt(0x2D, XTARGET); // 目标位置绝对位置

		sys_param_1[26] = XTARGET & 0x0000ffff;
		sys_param_1[27] = (XTARGET & 0xffff0000) >> 16;
	}
	else if (leat_XTARGET != XTARGET)
	{
		leat_XTARGET = XTARGET;

		TMC5160_SPIWriteInt(0x2D, XTARGET); // 目标位置绝对位置

		sys_param_1[26] = XTARGET & 0x0000ffff;
		sys_param_1[27] = (XTARGET & 0xffff0000) >> 16;
	}
}
/*******************************************************************************
 * 函数名       :
 * 函数功能		 :
 * 输入       :
 * 输出       :
 *******************************************************************************/
void TMC5160_DRV_STATUS_Return(void) //
{
	// uint32_t DRV_STATUS=0;
	int32_t value = 0, value1 = 0, value2 = 0;
	float s = 0;
	// static uint8_t leat_sys_param_24=0;
	static float f = 3.584;

	value = TMC5160_SPIReadInt(0x39, 0x00000000); // 读编码器 实际编码器位置
	s = value;

	sys_param_1[28] = value & 0x0000ffff;
	sys_param_1[29] = (value & 0xffff0000) >> 16;
	s /= f;
	//	value1=s;
	//	sys_param_1[30]=value1&0x0000ffff;
	//	sys_param_1[31]=(value1&0xffff0000)>>16;
	//
	//	if(leat_sys_param_24!=sys_param_1[24])
	//	{
	//
	//		leat_sys_param_24=sys_param_1[24];
	//
	//		value2=XTARGET;
	//
	//		f=value/value2;
	//	}
}

void TMC5160_Regest_Init_Test(void)
{
	// 设置细分-整步
	TMC5160_SPIWriteInt(0x6C, 0x000100C3); // PAGE46:CHOPCONF: TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadcycle)
	TMC5160_SPIWriteInt(0x6D, 0x0100E313); // PAGE48:
	TMC5160_SPIWriteInt(0x0A, 0x00180710); // PAGE30:死区时间10：200nS，BBMCLKS：7，驱动电流19、18BIT：10，滤波时间21、20BIT：01：200nS
	TMC5160_SPIWriteInt(0x10, 0x00061406); // PAGE33:IHOLD_IRUN: IHOLD=06(默认10), IRUN=20 (31max.current), IHOLDDELAY=6
	TMC5160_SPIWriteInt(0x11, 0x0000000A); // PAGE33:TPOWERDOWN=10:电机静止到电流减小之间的延时
	TMC5160_SPIWriteInt(0x00, 0x00000004); // PAGE27:EN_PWM_MODE=1，使能STEALTHCHOP
	TMC5160_SPIWriteInt(0x70, 0x000C0000); // PAGE43:PWMCONF--STEALTHCHOP:
	TMC5160_SPIWriteInt(0x13, 0x000001F4); // PAGE33:TPWM_THRS=500,对应切换速度35000=ca.30RPM

	// 加减速设置
	TMC5160_SPIWriteInt(0x24, 1000);	// PAGE35:A1=1000 第一阶段加速度
	TMC5160_SPIWriteInt(0x25, 50000);	// PAGE35:V1=50000加速度阀值速度V1
	TMC5160_SPIWriteInt(0x26, 500);		// PAGE35:AMAX=500大于V1的加速度
	TMC5160_SPIWriteInt(0x27, 2000000); // PAGE35:VMAX=200000
	TMC5160_SPIWriteInt(0x28, 700);		// PAGE35:DMAX=700大于V1的减速度
	TMC5160_SPIWriteInt(0x2A, 1400);	// PAGE35:D1=1400小于V1的减速度
	TMC5160_SPIWriteInt(0x2B, 10);		// PAGE35:VSTOP=10停止速度，接近于0
	// 位置模式
	TMC5160_SPIWriteInt(0x20, 0); // PAGE35:RAMPMODE=0位置模式，使用所有A、V、D参数
	// 编码器配置
	TMC5160_SPIWriteInt(0x38, 0x00000404);			// PAGE40：ENCMODE（RW）编码器配置和N通道使用：
	TMC5160_SPIWriteInt(0x39, 0);					// PAGE40：XENC（RW）实际编码器位，异码
	TMC5160_SPIWriteInt(0x3A, (51 * 65536 + 2000)); // PAGE40：ENC_CONST（W）累加常数，伴览货
}

// 主程序部分
void mmymian(long PositionData)
{
	TMC5160_SPIWriteInt(0x26, 9000);	// PAGE35：AMAX=500大寸V1的加加速度
	TMC5160_SPIWriteInt(0x27, 5000000); // PAGE35:VMAX=200000
	TMC5160_SPIWriteInt(0x28, 9000);	// PAGE35：DMAX=700大于V1的减速度
	TMC5160_SPIWriteInt(0x2A, 90000);	// PAGE35：VSTOP=10停止速度，接近于0//PAGE35:D1=1400小于V1的减速度
	TMC5160_SPIWriteInt(0x2B, 500);

	PositionData = 51200 * 5; // 顺时针转5图
	Move(PositionData);		  // PAGE36：XTARGET=-51200（逆时针旋转1
	for (int i = 0; i < 5; i++)
	{
		// IIENC_DATA = ReadData(0x39) / 204.8;
		ENC_DATA = TMC5160_SPIReadInt(0x39, 0x00000000) / 4.0; // PAGE40:X_ENC（RW）实际编码器位置：编码器因子51.2*4倍频=204.8，为了补
		Delay_MS(10);										   // 恢复高速参数
	}

	TMC5160_SPIWriteInt(0xA4 & 0x7F, 10000);   // PAGE35:A1-1000第一阶段加速度
	TMC5160_SPIWriteInt(0xA5 & 0x7F, 500000);  // PAGE35：V1=50000加速度阀值速度v1
	TMC5160_SPIWriteInt(0xA6 & 0x7F, 5000);	   // PAGE35：AMAX=500大于V1的加速度
	TMC5160_SPIWriteInt(0xA7 & 0x7F, 2000000); // PAGE35:VMAX=200000
	TMC5160_SPIWriteInt(0xA8 & 0x7F, 7000);	   // PAGE35:DMAX=700大于V1的减速度
	TMC5160_SPIWriteInt(0xAA & 0x7F, 14000);   // PAGE35：D1-1400小于V1的减速度
	TMC5160_SPIWriteInt(0xAB & 0x7F, 100);	   // PAGE35：VSTOP=10停止速度，接近于0
	Delay_MS(1000);

	Move(PositionData - ENC_DATA); // 位置补偿
	Delay_MS(1000);
}

void Move(long PosiTionData) // 移动数据
{
	uint16_t i;
	TMC5160_SPIWriteInt(0xAD & 0x7F, PosiTionData);		 // PAGE36:XTARGET=-51200(逆时针旋转1圈(1圈：200*256微步))
	TMC5160_SPIWriteInt(0xA1 & 0x7F, 0);				 // PAGE35:启动电机，参考位置清0
	TMC5160_SPIWriteInt(0x21 & 0x7F, 0);				 // PAGE35:	查询XACTUAL，多查询1次防止出错
	ReadPosition = TMC5160_SPIReadInt(0x21, 0x00000000); // PAGE35:	查询XACTUAL，下一个读操作返回XACTUAL，实际电机位置，该值通常只应在归零时修改。在位置模式下修改，修改内容将启动一个运动

	for (i = 0; i < 3000; i++)
	{
		Delay_MS(1);
		ReadPosition = TMC5160_SPIReadInt(0x21, 0x00000000);

		if ((Spi_Status & 0x28) == 0x28)
		{
			i = 3000;
			//			if(ReadPosition==0x00000000)
			//			{
			//				i=3000;
			//			}
			//			else;
		}
		else
			;
	}
}

// 归零测试代码
void zero_target(void)
{
	uint8_t enc_status = 0;
	uint32_t xlatch = 0;

	TMC5160_SPIWriteInt(0x34, 0x00000400); // writing value 0x00000400 = 1024 = 0.0 to address 26 = 0x34(SW_MODE)

	TMC5160_SPIWriteInt(0x38, 0x00000604);

	//  控制电机运行
	TMC5160_SPIWriteInt(0x20, 0x00000001); // 速度模式运行

	while (TMC5160_SPIReadInt(0x3B, 0x00000000) == 0x02)
	{
	};

	enc_status = TMC5160_SPIReadInt(0x3B, 0x00000000); // 读取编码器状态信息

	if (enc_status == 0x02) // 检测到事件发生
	{
		TMC5160_SPIWriteInt(0x3B, 0x00000002); // 清除状态位
	}

	TMC5160_SPIReadInt(0x36, 0x00000000);
	xlatch = TMC5160_SPIReadInt(0x36, 0x00000000);
}
