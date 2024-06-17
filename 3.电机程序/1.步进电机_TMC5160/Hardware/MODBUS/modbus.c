
/* 包含的头文件---------------------------------------------------------------*/
#include "modbus.h"

uint8_t testCoil = 1;  // 用于测试 位地址1
uint16_t testRegister; // 用于测试 字址址16

uint8_t localAddr = 1;	   // 单片机控制板的地址
uint8_t sendCount = 0;	   // 发送字节个数
uint8_t receCount = 0;	   // 接收到的字节个数
uint8_t sendPosi = 0;	   // 发送位置
uint8_t UARTU1_RX_bit = 0; // UARTU接收数据查询标志位
uint8_t UARTU1_TX_bit = 0; // UARTU接收数据查询标志位

uint32_t dwTickCount, dwIntTick;	// 时钟
uint8_t sendBuf[256], receBuf[256]; // 发送接收缓冲区
uint8_t checkoutError;				// ==2 偶校验错
uint16_t receTimeOut;				// 接收超时
uint8_t c10ms;						// 10ms 计时

unsigned char Bit_BianLiang[32]; // 32个位变量

/*******************************************************************************
 * @函数名称
 * @函数说明   主函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void USART1_Init(uint32_t btl)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		 // 端口号
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		 // 端口号	TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 // 复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			  // 端口号  RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	  // 传送速度
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_StructInit(&USART_InitStructure); // 将结构体设置为缺省状态

	USART_InitStructure.USART_BaudRate = btl;										// 波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 一针数据宽度8BITS
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 帧结尾一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 发送接收使能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流控制失能
	USART_Init(USART1, &USART_InitStructure);										// 设置串口1

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART_ITConfig(USART1, USART_IT_TC, ENABLE);   // 打开串口1的传输完成中断
	// USART_ITConfig(USART1, USART_IT_TXE, DISABLE); // 打开串口1的发送中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 打开串口1接收中断
	USART_Cmd(USART1, ENABLE);					   // 打开串口1
}

/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void b485Send(uint8_t n)
{
	if (n == 1)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	}
}

/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void beginSend(void) // 发送数据
{
	uint16_t t;
	b485Send(1);  // 设为发送
	sendPosi = 0; // 发送位置
	t = sendBuf[sendPosi];
	USART_SendData(USART1, t); // 发送一个字节
}

// 字地址 0 - 255 (只取低8位)
// 位地址 0 - 255 (只取低8位)
/* CRC 高位字节值表 */
const uint8_t auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};
/* CRC低位字节值表*/
const uint8_t auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen) // 计算CRC16
{
	uint8_t uchCRCHi = 0xFF; /* 高CRC字节初始化 */
	uint8_t uchCRCLo = 0xFF; /* 低CRC 字节初始化 */
	uint32_t uIndex;		 /* CRC循环中的索引 */
	while (usDataLen--)		 /* 传输消息缓冲区 */
	{
		uIndex = uchCRCHi ^ *puchMsg++; /* 计算CRC */
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}

/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void readCoil(void) // 读线圈
{
#if 0
	uint8_t addr;      //地址
	uint8_t tempAddr;
	uint8_t byteCount; //字节数
	uint8_t bitCount;  //位数
	uint16_t crcData;  //crc效验数据
	uint8_t position;
	uint8_t i,k;
	//uint8_t  result;
	uint16_t tempData;
	uint8_t  exit = 0;
	//addr = (receBuf[2]<<8) + receBuf[3];	
	//tempAddr = addr & 0xfff;
	addr = receBuf[3];//接收线圈数据的开始地址
	tempAddr = addr;

	//bitCount = (receBuf[4]<<8) + receBuf[5];	//读取的位个数
	bitCount = receBuf[5];//线圈个数

	byteCount = bitCount / 8;					//字节个数

	if(bitCount%8 != 0)//保证读到的位是8 的整数倍
		byteCount++;	
		
	for(k=0;k<byteCount;k++)
	{//字节位置
		position = k + 3;//取发送数据遇地址
		sendBuf[position] = 0;
		for(i=0;i<8;i++)
		{
			getCoilVal(tempAddr,&tempData);//获取线圈状态
			sendBuf[position] |= tempData << i;
			tempAddr++;
			if(tempAddr >= addr+bitCount)
			{	//读完
				exit = 1;
				break;
			}	
		}
		if(exit == 1)
			break;
	}
	
	sendBuf[0] = localAddr;
	sendBuf[1] = 0x01;	
	sendBuf[2] = byteCount;
	byteCount += 3;
	crcData = crc16(sendBuf,byteCount);	
	sendBuf[byteCount] = crcData >> 8;
	byteCount++;
	sendBuf[byteCount] = crcData & 0xff;
	sendCount = byteCount + 1;
	
	beginSend();
#endif
} // void readCoil(void)

/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void readRegisters(void) // 读寄存器
{
	uint16_t addr;
	uint16_t tempAddr;
	uint16_t crcData;
	uint16_t readCount;
	uint16_t byteCount;
	uint16_t i;
	uint16_t tempData = 0;
	addr = (receBuf[2] << 8) + receBuf[3]; // 起始地址
	tempAddr = addr;

	readCount = (receBuf[4] << 8) + receBuf[5]; // 要读的个数
	// readCount = receBuf[5];
	if (receBuf[5] < 1024) // 目前只有1024个数据
	{
		byteCount = readCount * 2;
		for (i = 0; i < byteCount; i += 2, tempAddr++)
		{
			getRegisterVal(tempAddr, &tempData);
			sendBuf[i + 3] = tempData >> 8;
			sendBuf[i + 4] = tempData & 0xff;
		}

		sendBuf[0] = localAddr; // 器件地址
		sendBuf[1] = 3;			// 功能吗
		sendBuf[2] = byteCount; // 数据个数
		byteCount += 3;
		crcData = crc16(sendBuf, byteCount);
		sendBuf[byteCount] = crcData >> 8;
		byteCount++;
		sendBuf[byteCount] = crcData & 0xff;

		sendCount = byteCount + 1;
		beginSend();
	}
	else
	{
		receCount = 0; // 清接收地址偏移寄存器
		checkoutError = 0;
	}
} // void readRegisters(void)

/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void forceSingleCoil(void) // 强制单个线圈
{
#if 0
	uint8_t addr;
	uint8_t tempAddr;
	uint16_t tempData;
	uint8_t  onOff;
	uint8_t i;
	
	//addr = (receBuf[2]<<8) + receBuf[3];	
	//tempAddr = addr & 0xfff;
	addr = receBuf[3];
	tempAddr = addr;

	//onOff = (receBuf[4]<<8) + receBuf[5];	
	onOff = receBuf[4];
	
	//if(onOff == 0xff00)
	if(onOff == 0xff)
	{	//设为ON
		tempData = 1;
	} 
	//else if(onOff == 0x0000)
	else if(onOff == 0x00)
	{	//设为OFF
		tempData = 0;
	}

	setCoilVal(tempAddr,tempData);	
	
	for(i=0;i<receCount;i++)
	{
		sendBuf[i] = receBuf[i];
	}
	sendCount = receCount;
	beginSend();
#endif
} // void forceSingleCoil(void)
/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void presetSingleRegister(void) // 设置单个寄存器
{
	uint16_t addr = 0;
	uint16_t addr_L = 0;
	uint16_t addr_H = 0;
	uint16_t tempAddr = 0;
	uint8_t byteCount = byteCount;
	uint16_t crcData = 0;
	uint16_t tempData = 0;

	addr_H = receBuf[2]; // 地址高位
	addr_L = receBuf[3]; // 地址低位
	addr = addr_H;
	addr <<= 8;
	addr |= addr_L;
	tempAddr = addr;

	tempData = receBuf[4];
	tempData <<= 8;
	tempData |= receBuf[5];
	setRegisterVal(tempAddr, tempData);

	sendBuf[0] = localAddr;
	sendBuf[1] = 6;
	sendBuf[2] = addr_H;
	sendBuf[3] = addr_L;
	sendBuf[4] = receBuf[4];
	sendBuf[5] = receBuf[5];
	crcData = crc16(sendBuf, 6);
	sendBuf[6] = crcData >> 8;
	sendBuf[7] = crcData & 0xff;
	sendCount = 8;
	UARTU1_TX_bit = 1; // UARTU发送数据查询标志位
	beginSend();
} //

/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void presetMultipleRegisters(void) // 设置多个寄存器
{
	uint16_t addr;
	uint16_t tempAddr;
	uint16_t byteCount = byteCount;
	uint16_t setCount;
	uint16_t crcData;
	uint16_t tempData;
	// uint8_t  finsh;	//为1时完成 为0时出错
	uint16_t i;

	addr = (receBuf[2] << 8) + receBuf[3]; // 起始地址
	tempAddr = addr;

	setCount = (receBuf[4] << 8) + receBuf[5]; // 元件个数
	byteCount = receBuf[6];					   // 字节数
	for (i = 0; i < setCount; i++, tempAddr++)
	{
		tempData = (receBuf[i * 2 + 7] << 8) + receBuf[i * 2 + 8];
		setRegisterVal(tempAddr, tempData); // 设置寄存器值
	}
	sendBuf[0] = localAddr;
	sendBuf[1] = 16;
	sendBuf[2] = addr >> 8;
	sendBuf[3] = addr & 0xff;
	sendBuf[4] = setCount >> 8;
	sendBuf[5] = setCount & 0xff;
	crcData = crc16(sendBuf, 6);
	sendBuf[6] = crcData >> 8;
	sendBuf[7] = crcData & 0xff;
	sendCount = 8;
	beginSend();
} // void presetMultipleRegisters(void)
/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void checkComm0Modbus(void) // 查询UART数据
{
	uint16_t crcData;
	uint16_t tempData;
	localAddr = UART_addr; // modbus_rtu通讯地址
	if (receCount > 4)	   // 接收数据大于4个字节开始解析通讯协议
	{
		switch (receBuf[1]) // modbus功能
		{
		case 1: // 读取线圈状态(读取点 16位以内)
		case 3: // 读取保持寄存器(一个或多个)
		case 5: // 强制单个线圈
		case 6:
		{ // 设置单个寄存器
			if (receCount >= 8)
			{ // 接收完成一组数据
				// 应该关闭接收中断

				if (receBuf[0] == localAddr && checkoutError == 0)
				{
					crcData = crc16(receBuf, 6);
					if (crcData == receBuf[7] + (receBuf[6] << 8))
					{ // 校验正确
						if (receBuf[1] == 1)
						{ // 读取线圈状态(读取点 16位以内)
							readCoil();
						}
						else if (receBuf[1] == 3)
						{ // 读取保持寄存器(一个或多个)
							readRegisters();
						}
						else if (receBuf[1] == 5)
						{ // 强制单个线圈
							forceSingleCoil();
						}
						else if (receBuf[1] == 6)
						{ // 写单个寄存器
							presetSingleRegister();
						}
					}
				}
				receCount = 0; // 初始化接收数据数组地址
				checkoutError = 0;
			}
			break;
		}
		case 15:
		{ // 设置多个线圈
			tempData = receBuf[6];
			tempData += 9; // 数据个数
			if (receCount >= tempData)
			{
				if (receBuf[0] == localAddr && checkoutError == 0)
				{
					crcData = crc16(receBuf, tempData - 2);
					if (crcData == (receBuf[tempData - 2] << 8) + receBuf[tempData - 1])
					{
						// forceMultipleCoils();
					}
					receCount = 0; // 初始化接收数据数组地址
					checkoutError = 0;
				}
				else
				{
					receCount = 0; // 初始化接收数据数组地址
					checkoutError = 0;
				}
			}
			break;
		}
		case 16:
		{ // 设置多个寄存器
			tempData = (receBuf[4] << 8) + receBuf[5];
			tempData = tempData * 2; // 数据个数
			tempData += 9;
			if (receCount >= tempData)
			{
				if (receBuf[0] == localAddr && checkoutError == 0)
				{
					crcData = crc16(receBuf, tempData - 2);
					if (crcData == (receBuf[tempData - 2] << 8) + receBuf[tempData - 1])
					{
						presetMultipleRegisters();
					}
					receCount = 0; // 初始化接收数据数组地址
					checkoutError = 0;
				}
				else
				{
					receCount = 0; // 初始化接收数据数组地址
					checkoutError = 0;
				}
			}
			break;
		}
		default:
		{				   // 不是所支持的功能吗，初始化接收
			receCount = 0; // 初始化接收数据数组地址
			break;
		}
		}
	}
} // void checkComm0(void)
/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
// 取线圈状态 返回0表示成功
uint16_t getCoilVal(uint16_t addr, uint16_t *tempData)
{
	uint16_t result = 0;
	uint16_t tempAddr;

	tempAddr = addr & 0xfff;
	// 只取低8位地址

	switch (tempAddr & 0xff)
	{
	case 0:
		*tempData = Bit_BianLiang[tempAddr];
		break;
	case 1:
		*tempData = Bit_BianLiang[tempAddr];
		break;
	case 2:
		*tempData = Bit_BianLiang[tempAddr];
		break;
	case 3:
		*tempData = Bit_BianLiang[tempAddr];
		break;
	case 4:
		*tempData = Bit_BianLiang[tempAddr];
		break;
	case 5:
		*tempData = Bit_BianLiang[tempAddr];
		break;
	default:
		break;
	}

	return result;

} // uint16_t getCoilVal(uint16_t addr,uint16_t *data)
/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
// 设定线圈状态 返回0表示成功
uint16_t setCoilVal(uint16_t addr, uint16_t tempData)
{
	uint16_t result = 0;
	uint16_t tempAddr;
	tempAddr = addr & 0xfff;
	switch (tempAddr & 0xff)
	{
	case 0:
		Bit_BianLiang[tempAddr] = tempData;
		break;
	case 1:
		Bit_BianLiang[tempAddr] = tempData;
		break;
	case 2:
		Bit_BianLiang[tempAddr] = tempData;
		break;
	case 3:
		Bit_BianLiang[tempAddr] = tempData;
		break;
	case 4:
		Bit_BianLiang[tempAddr] = tempData;
		break;
	default:
		break;
	}
	return result;
} // uint16_t setCoilVal(uint16_t addr,uint16_t data)
/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
// 取寄存器值 返回0表示成功
uint16_t getRegisterVal(uint16_t addr, uint16_t *tempData)
{
	uint16_t i = 0;
	uint16_t result = 0;
	uint16_t tempAddr;
	tempAddr = addr;
	if (tempAddr < 100)
	{
		i = tempAddr;
		*tempData = sys_param_1[i];
	}
	//	 if((tempAddr>=100)&(tempAddr<=299))
	//	 {
	//		i=tempAddr-100;
	//		*tempData=sys_param_2[i] ;
	//	 }
	//	 if((tempAddr>=300)&(tempAddr<=499))
	//	 {
	//		i=tempAddr-300;
	//		*tempData=sys_param_3[i] ;
	//	 }
	return result;
} // uint16_t getRegisterVal(uint16_t addr,uint16_t &data)
/*******************************************************************************
 * @函数名称
 * @函数说明   函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
// 设置寄存器值 返回0表示成功
uint16_t setRegisterVal(uint16_t addr, uint16_t tempData)
{
	uint16_t i = 0;
	uint16_t result = 0;
	uint16_t tempAddr;
	tempAddr = addr;

	if (tempAddr < 100)
	{
		i = tempAddr;
		sys_param_1[i] = tempData;
		if (tempAddr < 20 | tempAddr > 39)
		{
			save_parm_bit = 1; // 保存参数变量置1=保存参数
		}
	}
	//	  if((tempAddr>=100)&(tempAddr<=299))
	//	  {
	//			i=tempAddr-100;
	//			sys_param_2[i] =tempData;
	//			save_parm_bit=1;//保存参数变量置1=保存参数
	//	  }
	//	  if((tempAddr>=300)&(tempAddr<=499))
	//	  {
	//			i=tempAddr-300;
	//			sys_param_3[i] =tempData;
	//	  }

	return result;
}

/*******************************************************************************
 * @函数名称	USART1_Handler
 * @函数说明   串口1中断
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *******************************************************************************/
void USART1_IRQHandler(void)
{
	FlagStatus Status, Status1, Status2, Status3, Status4, Status5, Status6;
	u16 RxData = 0;
	u16 TimeOut = 0;
	u16 BUFSIZE = 256; // 接收数据字节最大
	u16 t = 0;

	Status = USART_GetITStatus(USART1, USART_IT_TXE);	// 发送数据寄存器空中断
	Status1 = USART_GetITStatus(USART1, USART_IT_TC);	// 发送数据完成中断
	Status2 = USART_GetITStatus(USART1, USART_IT_RXNE); // 接收数据寄存器非空中断
	Status4 = USART_GetITStatus(USART1, USART_IT_IDLE); // 总线空闲标志
	if (Status == SET)									// 发送寄存器空中断
	{
		USART_ClearFlag(USART1, USART_FLAG_TXE);
		USART_ClearITPendingBit(USART1, USART_IT_TXE);
	}
	//--------------------------------------//
	if (Status1 == SET) // 发送完成中断
	{
		USART_ClearFlag(USART1, USART_FLAG_TC);
		USART_ClearITPendingBit(USART1, USART_IT_TC);

		// if (sendPosi < sendCount - 1)
		// {
		// 	sendPosi++;
		// 	t = sendBuf[sendPosi];
		// 	USART_SendData(USART1, t); // 发送一个字节
		// }
		// else
		// {
		// 	receCount = 0; // 清接收地址偏移寄存器
		// 	checkoutError = 0;
		// 	b485Send(0); // 发送完成后将485设为接收
		// }
	}
	//-----------------------------------//
	if (Status2 == SET) // 接收数据寄存器非空中断
	{

		uint8_t Res;
		if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			Res = USART_ReceiveData(USART1); // 读取接收到的数据

			//USART_SendData(USART1, Res);
			protocol_data_recv(&Res, 1);
		}

		// MODBUS接收中断
		// USART_ClearFlag(USART1, USART_FLAG_RXNE);
		// USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		// RxData = USART_ReceiveData(USART1); //

		// protocol_data_recv((uint8_t *)&RxData, 1); // 野火调试PID参数

		// switch (baudRate_Selection) // 根据波特率选择超时时间
		// {
		// case 0:
		// {				  // 4800
		// 	TimeOut = 80; // ms
		// 	break;
		// }
		// case 1:
		// {				  // 9600
		// 	TimeOut = 40; // ms
		// 	break;
		// }
		// case 2:
		// {				  // 19200
		// 	TimeOut = 20; // ms
		// 	break;
		// }
		// case 3:
		// {				 // 115200
		// 	TimeOut = 4; // ms
		// 	break;
		// }
		// default:
		// {
		// 	TimeOut = 80; // ms
		// 	break;
		// }
		// }
		// if (receTimeOut > TimeOut) // 接收超时1个单位是 100us
		// {
		// 	receCount = 0;	 /* buffer overflow */
		// 	receTimeOut = 0; //
		// 	TIM_Cmd(TIM2, DISABLE);
		// 	TIM_SetCounter(TIM2, 0x0000);
		// 	TIM_Cmd(TIM2, ENABLE);
		// }
		// else
		// {
		// 	receTimeOut = 0; //
		// }
		// receBuf[receCount] = RxData;
		// receCount++;
		// if (receCount == BUFSIZE) // 接收最大数据个数
		// {
		// 	receCount = 0; /* buffer overflow */
		// }
		// UARTU1_RX_bit = 1; // UARTU接收数据查询标志位
		// 				   // checkComm0Modbus();	//查询UART数据
	}
	//---------------------------------------//
	if (Status4 == SET) // 总线空闲标志
	{
		USART_ClearFlag(USART1, USART_FLAG_IDLE);
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	}
}
