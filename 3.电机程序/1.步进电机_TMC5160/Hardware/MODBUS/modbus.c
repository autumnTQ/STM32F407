
/* ������ͷ�ļ�---------------------------------------------------------------*/
#include "modbus.h"

uint8_t testCoil = 1;  // ���ڲ��� λ��ַ1
uint16_t testRegister; // ���ڲ��� ��ַַ16

uint8_t localAddr = 1;	   // ��Ƭ�����ư�ĵ�ַ
uint8_t sendCount = 0;	   // �����ֽڸ���
uint8_t receCount = 0;	   // ���յ����ֽڸ���
uint8_t sendPosi = 0;	   // ����λ��
uint8_t UARTU1_RX_bit = 0; // UARTU�������ݲ�ѯ��־λ
uint8_t UARTU1_TX_bit = 0; // UARTU�������ݲ�ѯ��־λ

uint32_t dwTickCount, dwIntTick;	// ʱ��
uint8_t sendBuf[256], receBuf[256]; // ���ͽ��ջ�����
uint8_t checkoutError;				// ==2 żУ���
uint16_t receTimeOut;				// ���ճ�ʱ
uint8_t c10ms;						// 10ms ��ʱ

unsigned char Bit_BianLiang[32]; // 32��λ����

/*******************************************************************************
 * @��������
 * @����˵��   ������
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void USART1_Init(uint32_t btl)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		 // �˿ں�
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		 // �˿ں�	TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 // �����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			  // �˿ں�  RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	  // �����ٶ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_StructInit(&USART_InitStructure); // ���ṹ������Ϊȱʡ״̬

	USART_InitStructure.USART_BaudRate = btl;										// ������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// һ�����ݿ��8BITS
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// ֡��βһ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// ���ͽ���ʹ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // Ӳ��������ʧ��
	USART_Init(USART1, &USART_InitStructure);										// ���ô���1

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART_ITConfig(USART1, USART_IT_TC, ENABLE);   // �򿪴���1�Ĵ�������ж�
	// USART_ITConfig(USART1, USART_IT_TXE, DISABLE); // �򿪴���1�ķ����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // �򿪴���1�����ж�
	USART_Cmd(USART1, ENABLE);					   // �򿪴���1
}

/*******************************************************************************
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
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
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void beginSend(void) // ��������
{
	uint16_t t;
	b485Send(1);  // ��Ϊ����
	sendPosi = 0; // ����λ��
	t = sendBuf[sendPosi];
	USART_SendData(USART1, t); // ����һ���ֽ�
}

// �ֵ�ַ 0 - 255 (ֻȡ��8λ)
// λ��ַ 0 - 255 (ֻȡ��8λ)
/* CRC ��λ�ֽ�ֵ�� */
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
/* CRC��λ�ֽ�ֵ��*/
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
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen) // ����CRC16
{
	uint8_t uchCRCHi = 0xFF; /* ��CRC�ֽڳ�ʼ�� */
	uint8_t uchCRCLo = 0xFF; /* ��CRC �ֽڳ�ʼ�� */
	uint32_t uIndex;		 /* CRCѭ���е����� */
	while (usDataLen--)		 /* ������Ϣ������ */
	{
		uIndex = uchCRCHi ^ *puchMsg++; /* ����CRC */
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}

/*******************************************************************************
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void readCoil(void) // ����Ȧ
{
#if 0
	uint8_t addr;      //��ַ
	uint8_t tempAddr;
	uint8_t byteCount; //�ֽ���
	uint8_t bitCount;  //λ��
	uint16_t crcData;  //crcЧ������
	uint8_t position;
	uint8_t i,k;
	//uint8_t  result;
	uint16_t tempData;
	uint8_t  exit = 0;
	//addr = (receBuf[2]<<8) + receBuf[3];	
	//tempAddr = addr & 0xfff;
	addr = receBuf[3];//������Ȧ���ݵĿ�ʼ��ַ
	tempAddr = addr;

	//bitCount = (receBuf[4]<<8) + receBuf[5];	//��ȡ��λ����
	bitCount = receBuf[5];//��Ȧ����

	byteCount = bitCount / 8;					//�ֽڸ���

	if(bitCount%8 != 0)//��֤������λ��8 ��������
		byteCount++;	
		
	for(k=0;k<byteCount;k++)
	{//�ֽ�λ��
		position = k + 3;//ȡ������������ַ
		sendBuf[position] = 0;
		for(i=0;i<8;i++)
		{
			getCoilVal(tempAddr,&tempData);//��ȡ��Ȧ״̬
			sendBuf[position] |= tempData << i;
			tempAddr++;
			if(tempAddr >= addr+bitCount)
			{	//����
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
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void readRegisters(void) // ���Ĵ���
{
	uint16_t addr;
	uint16_t tempAddr;
	uint16_t crcData;
	uint16_t readCount;
	uint16_t byteCount;
	uint16_t i;
	uint16_t tempData = 0;
	addr = (receBuf[2] << 8) + receBuf[3]; // ��ʼ��ַ
	tempAddr = addr;

	readCount = (receBuf[4] << 8) + receBuf[5]; // Ҫ���ĸ���
	// readCount = receBuf[5];
	if (receBuf[5] < 1024) // Ŀǰֻ��1024������
	{
		byteCount = readCount * 2;
		for (i = 0; i < byteCount; i += 2, tempAddr++)
		{
			getRegisterVal(tempAddr, &tempData);
			sendBuf[i + 3] = tempData >> 8;
			sendBuf[i + 4] = tempData & 0xff;
		}

		sendBuf[0] = localAddr; // ������ַ
		sendBuf[1] = 3;			// ������
		sendBuf[2] = byteCount; // ���ݸ���
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
		receCount = 0; // ����յ�ַƫ�ƼĴ���
		checkoutError = 0;
	}
} // void readRegisters(void)

/*******************************************************************************
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void forceSingleCoil(void) // ǿ�Ƶ�����Ȧ
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
	{	//��ΪON
		tempData = 1;
	} 
	//else if(onOff == 0x0000)
	else if(onOff == 0x00)
	{	//��ΪOFF
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
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void presetSingleRegister(void) // ���õ����Ĵ���
{
	uint16_t addr = 0;
	uint16_t addr_L = 0;
	uint16_t addr_H = 0;
	uint16_t tempAddr = 0;
	uint8_t byteCount = byteCount;
	uint16_t crcData = 0;
	uint16_t tempData = 0;

	addr_H = receBuf[2]; // ��ַ��λ
	addr_L = receBuf[3]; // ��ַ��λ
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
	UARTU1_TX_bit = 1; // UARTU�������ݲ�ѯ��־λ
	beginSend();
} //

/*******************************************************************************
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void presetMultipleRegisters(void) // ���ö���Ĵ���
{
	uint16_t addr;
	uint16_t tempAddr;
	uint16_t byteCount = byteCount;
	uint16_t setCount;
	uint16_t crcData;
	uint16_t tempData;
	// uint8_t  finsh;	//Ϊ1ʱ��� Ϊ0ʱ����
	uint16_t i;

	addr = (receBuf[2] << 8) + receBuf[3]; // ��ʼ��ַ
	tempAddr = addr;

	setCount = (receBuf[4] << 8) + receBuf[5]; // Ԫ������
	byteCount = receBuf[6];					   // �ֽ���
	for (i = 0; i < setCount; i++, tempAddr++)
	{
		tempData = (receBuf[i * 2 + 7] << 8) + receBuf[i * 2 + 8];
		setRegisterVal(tempAddr, tempData); // ���üĴ���ֵ
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
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
void checkComm0Modbus(void) // ��ѯUART����
{
	uint16_t crcData;
	uint16_t tempData;
	localAddr = UART_addr; // modbus_rtuͨѶ��ַ
	if (receCount > 4)	   // �������ݴ���4���ֽڿ�ʼ����ͨѶЭ��
	{
		switch (receBuf[1]) // modbus����
		{
		case 1: // ��ȡ��Ȧ״̬(��ȡ�� 16λ����)
		case 3: // ��ȡ���ּĴ���(һ������)
		case 5: // ǿ�Ƶ�����Ȧ
		case 6:
		{ // ���õ����Ĵ���
			if (receCount >= 8)
			{ // �������һ������
				// Ӧ�ùرս����ж�

				if (receBuf[0] == localAddr && checkoutError == 0)
				{
					crcData = crc16(receBuf, 6);
					if (crcData == receBuf[7] + (receBuf[6] << 8))
					{ // У����ȷ
						if (receBuf[1] == 1)
						{ // ��ȡ��Ȧ״̬(��ȡ�� 16λ����)
							readCoil();
						}
						else if (receBuf[1] == 3)
						{ // ��ȡ���ּĴ���(һ������)
							readRegisters();
						}
						else if (receBuf[1] == 5)
						{ // ǿ�Ƶ�����Ȧ
							forceSingleCoil();
						}
						else if (receBuf[1] == 6)
						{ // д�����Ĵ���
							presetSingleRegister();
						}
					}
				}
				receCount = 0; // ��ʼ���������������ַ
				checkoutError = 0;
			}
			break;
		}
		case 15:
		{ // ���ö����Ȧ
			tempData = receBuf[6];
			tempData += 9; // ���ݸ���
			if (receCount >= tempData)
			{
				if (receBuf[0] == localAddr && checkoutError == 0)
				{
					crcData = crc16(receBuf, tempData - 2);
					if (crcData == (receBuf[tempData - 2] << 8) + receBuf[tempData - 1])
					{
						// forceMultipleCoils();
					}
					receCount = 0; // ��ʼ���������������ַ
					checkoutError = 0;
				}
				else
				{
					receCount = 0; // ��ʼ���������������ַ
					checkoutError = 0;
				}
			}
			break;
		}
		case 16:
		{ // ���ö���Ĵ���
			tempData = (receBuf[4] << 8) + receBuf[5];
			tempData = tempData * 2; // ���ݸ���
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
					receCount = 0; // ��ʼ���������������ַ
					checkoutError = 0;
				}
				else
				{
					receCount = 0; // ��ʼ���������������ַ
					checkoutError = 0;
				}
			}
			break;
		}
		default:
		{				   // ������֧�ֵĹ����𣬳�ʼ������
			receCount = 0; // ��ʼ���������������ַ
			break;
		}
		}
	}
} // void checkComm0(void)
/*******************************************************************************
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
// ȡ��Ȧ״̬ ����0��ʾ�ɹ�
uint16_t getCoilVal(uint16_t addr, uint16_t *tempData)
{
	uint16_t result = 0;
	uint16_t tempAddr;

	tempAddr = addr & 0xfff;
	// ֻȡ��8λ��ַ

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
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
// �趨��Ȧ״̬ ����0��ʾ�ɹ�
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
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
// ȡ�Ĵ���ֵ ����0��ʾ�ɹ�
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
 * @��������
 * @����˵��   ����
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *****************************************************************************/
// ���üĴ���ֵ ����0��ʾ�ɹ�
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
			save_parm_bit = 1; // �������������1=�������
		}
	}
	//	  if((tempAddr>=100)&(tempAddr<=299))
	//	  {
	//			i=tempAddr-100;
	//			sys_param_2[i] =tempData;
	//			save_parm_bit=1;//�������������1=�������
	//	  }
	//	  if((tempAddr>=300)&(tempAddr<=499))
	//	  {
	//			i=tempAddr-300;
	//			sys_param_3[i] =tempData;
	//	  }

	return result;
}

/*******************************************************************************
 * @��������	USART1_Handler
 * @����˵��   ����1�ж�
 * @�������   ��
 * @�������   ��
 * @���ز���   ��
 *******************************************************************************/
void USART1_IRQHandler(void)
{
	FlagStatus Status, Status1, Status2, Status3, Status4, Status5, Status6;
	u16 RxData = 0;
	u16 TimeOut = 0;
	u16 BUFSIZE = 256; // ���������ֽ����
	u16 t = 0;

	Status = USART_GetITStatus(USART1, USART_IT_TXE);	// �������ݼĴ������ж�
	Status1 = USART_GetITStatus(USART1, USART_IT_TC);	// ������������ж�
	Status2 = USART_GetITStatus(USART1, USART_IT_RXNE); // �������ݼĴ����ǿ��ж�
	Status4 = USART_GetITStatus(USART1, USART_IT_IDLE); // ���߿��б�־
	if (Status == SET)									// ���ͼĴ������ж�
	{
		USART_ClearFlag(USART1, USART_FLAG_TXE);
		USART_ClearITPendingBit(USART1, USART_IT_TXE);
	}
	//--------------------------------------//
	if (Status1 == SET) // ��������ж�
	{
		USART_ClearFlag(USART1, USART_FLAG_TC);
		USART_ClearITPendingBit(USART1, USART_IT_TC);

		// if (sendPosi < sendCount - 1)
		// {
		// 	sendPosi++;
		// 	t = sendBuf[sendPosi];
		// 	USART_SendData(USART1, t); // ����һ���ֽ�
		// }
		// else
		// {
		// 	receCount = 0; // ����յ�ַƫ�ƼĴ���
		// 	checkoutError = 0;
		// 	b485Send(0); // ������ɺ�485��Ϊ����
		// }
	}
	//-----------------------------------//
	if (Status2 == SET) // �������ݼĴ����ǿ��ж�
	{

		uint8_t Res;
		if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
			Res = USART_ReceiveData(USART1); // ��ȡ���յ�������

			//USART_SendData(USART1, Res);
			protocol_data_recv(&Res, 1);
		}

		// MODBUS�����ж�
		// USART_ClearFlag(USART1, USART_FLAG_RXNE);
		// USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		// RxData = USART_ReceiveData(USART1); //

		// protocol_data_recv((uint8_t *)&RxData, 1); // Ұ�����PID����

		// switch (baudRate_Selection) // ���ݲ�����ѡ��ʱʱ��
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
		// if (receTimeOut > TimeOut) // ���ճ�ʱ1����λ�� 100us
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
		// if (receCount == BUFSIZE) // ����������ݸ���
		// {
		// 	receCount = 0; /* buffer overflow */
		// }
		// UARTU1_RX_bit = 1; // UARTU�������ݲ�ѯ��־λ
		// 				   // checkComm0Modbus();	//��ѯUART����
	}
	//---------------------------------------//
	if (Status4 == SET) // ���߿��б�־
	{
		USART_ClearFlag(USART1, USART_FLAG_IDLE);
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	}
}
