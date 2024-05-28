#include "usart.h"
#include "malloc.h"
#include "sys.h"
#include "usart.h"	
#include "string.h"
#include "common.h"

#include "FreeRTOS.h"
#include "task.h"

#define FIX_LENGTH   8    //除校验和 和 数据位之后的固定协议的内容长度

#define FIX_LENGTH_1   7    //除校验和 和 数据位之后的固定协议的内容长度

/* crc校验 */
unsigned short CrcVerifice(unsigned char * pStr, unsigned int iLen)
{
	unsigned short iCrc = 0xFFFF;
	while (iLen--)
	{
		iCrc ^= *pStr++;
		for (int i = 0; i < 8; ++i)
		{
			if (iCrc & 1)
			{
				iCrc = (iCrc >> 1) ^ 0xA001;
			}
			else
			{
				iCrc = (iCrc >> 1);
			}
		}
	}
	return iCrc;
}

unsigned char SerialSendData(unsigned char *fixProtocol, char * data, int dataLength)
{
	taskENTER_CRITICAL();
	
	int totalLength = FIX_LENGTH + dataLength + 2;//完整帧长度 =  固定长度+数据长度+校验和
	unsigned char* szSendData = (unsigned char*)mymalloc(SRAMIN,totalLength);
	mymemcpy(szSendData, fixProtocol, FIX_LENGTH);
	mymemcpy(szSendData + FIX_LENGTH, data, dataLength);

	union HEXSHORT unLength;
	unLength.num = totalLength - 3;

	szSendData[1] = unLength.bnum[1];
	szSendData[2] = unLength.bnum[0];
	
	
	unsigned short iCrc = CrcVerifice(szSendData, totalLength - 2);
	szSendData[totalLength - 2] = iCrc & 0Xff;
	szSendData[totalLength - 1] = (iCrc >> 8) & 0xff;
	
	
	for(int i = 0;i<totalLength;i++)
	{
		USART_SendData(UART4,szSendData[i]);
		while( USART_GetFlagStatus(UART4,USART_FLAG_TC) != SET);//发送完了，标志位置1，此时判断“不等于1”==0 就可以
	}
	myfree(SRAMIN,szSendData);		//释放内存
	
	taskEXIT_CRITICAL();
	return 1;
}


//用于测试发送固定数据
unsigned char SerialSendData_PC(unsigned char *fixProtocol, int dataLength)
{
	taskENTER_CRITICAL();
	
	int totalLength = dataLength + 2;//完整帧长度 =  固定长度+数据长度+校验和
	unsigned char* szSendData = (unsigned char*)mymalloc(SRAMIN,totalLength);
	mymemcpy(szSendData, fixProtocol, dataLength);
	
	unsigned short iCrc = CrcVerifice(szSendData, totalLength - 2);
	szSendData[totalLength - 2] = iCrc & 0Xff;
	szSendData[totalLength - 1] = (iCrc >> 8) & 0xff;
	
	
	for(int i = 0;i<totalLength;i++)
	{
		USART_SendData(UART4,szSendData[i]);
		while( USART_GetFlagStatus(UART4,USART_FLAG_TC) != SET);//发送完了，标志位置1，此时判断“不等于1”==0 就可以
	}
	myfree(SRAMIN,szSendData);		//释放内存
	
	taskEXIT_CRITICAL();
	return 1;
}

bool  SerialSendData2Motor(ProtocolType type, unsigned char *fixProtocol, char * data, int dataLength)
{
	taskENTER_CRITICAL();
	
	int fixLength;

	if (type == ProtocolType_1)
	{
		fixLength = FIX_LENGTH_1 - 1;
	}
	else
	{
		fixLength = FIX_LENGTH_1;    //数值为7
	}

	int totalLength = fixLength + dataLength + 2;//完整帧长度 =  固定长度+数据长度+校验和
	unsigned char* szSendData = (unsigned char*)mymalloc(SRAMIN,totalLength);
	mymemcpy(szSendData, fixProtocol, FIX_LENGTH_1);
	mymemcpy(szSendData + FIX_LENGTH_1, data, dataLength);
	//szSendData[0] = 0x01;
	//校验和
	unsigned short iCrc = CrcVerifice(szSendData, totalLength - 2);
	szSendData[totalLength - 2] = iCrc & 0Xff;
	szSendData[totalLength - 1] = (iCrc >> 8) & 0xff;
	
	
	for(int i = 0;i<totalLength;i++)
	{
		
		//vTaskDelay(1); 
		while( USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);//发送完了，标志位置1，此时判断“不等于1”==0 就可以
		USART_SendData(USART2,szSendData[i]);
	}
	
	myfree(SRAMIN,szSendData);		//释放内存
	
	taskEXIT_CRITICAL();
	
	return true;
}

bool SerialSendData2Pc(ProtocolType type, unsigned char *fixProtocol, char * data, int dataLength)
{
	taskENTER_CRITICAL();
	
	int fixLength;
	if (type == ProtocolType_1)
	{
		fixLength = FIX_LENGTH_1 - 1;
	}
	else
	{
		fixLength = FIX_LENGTH_1;
	}
	int totalLength = fixLength + dataLength + 2;//完整帧长度 =  固定长度+数据长度+校验和
	unsigned char* szSendData = (unsigned char*)mymalloc(SRAMIN,totalLength);
	mymemcpy(szSendData, fixProtocol, FIX_LENGTH_1);
	mymemcpy(szSendData + FIX_LENGTH_1, data, dataLength);
	szSendData[0] = 0x01;
	//校验和
	unsigned short iCrc = CrcVerifice(szSendData, totalLength - 2);
	szSendData[totalLength - 2] = iCrc & 0Xff;
	szSendData[totalLength - 1] = (iCrc >> 8) & 0xff;

	for(int i = 0;i<totalLength;i++)
	{
		
		while( USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);//发送完了，标志位置1，此时判断“不等于1”==0 就可以
		USART_SendData(USART1,szSendData[i]);
	}
	myfree(SRAMIN,szSendData);		//释放内存
	
	taskEXIT_CRITICAL();
	return true;
}

//串口3发送数据给夹爪
bool  SerialSendData2Grip(ProtocolType type, unsigned char *fixProtocol)
{
	taskENTER_CRITICAL();
		
	for(int i = 0;i<9;i++)
	{
		while( USART_GetFlagStatus(USART3,USART_FLAG_TC) != SET);//发送完了，标志位置1，此时判断“不等于1”==0 就可以
		USART_SendData(USART3,fixProtocol[i]);
	}
		
	taskEXIT_CRITICAL();
	
	return true;
}

//串口2发送数据给推杆
bool  SerialSendData2Push(ProtocolType type, unsigned char *fixProtocol, char * data, int dataLength)
{
	taskENTER_CRITICAL();
	
	int fixLength = 4;

	int totalLength = fixLength + dataLength + 2;//完整帧长度 =  固定长度+数据长度+校验和
    
	unsigned char* szSendData = (unsigned char*)mymalloc(SRAMIN,totalLength);
    
	if (type == ProtocolType_1)
	{
		mymemcpy(szSendData, fixProtocol, fixLength);
		mymemcpy(szSendData + fixLength, data, dataLength);
	}
	else if(type == ProtocolType_2)
	{
		mymemcpy(szSendData, fixProtocol, 6);
		mymemcpy(szSendData + 6, data, dataLength);
	}

	//校验和
	unsigned short iCrc = CrcVerifice(szSendData, totalLength - 2);
	szSendData[totalLength - 2] = iCrc & 0Xff;
	szSendData[totalLength - 1] = (iCrc >> 8) & 0xff;
	
	
	for(int i = 0;i<totalLength;i++)
	{
		//vTaskDelay(1); 
		while( USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);         //发送完了，标志位置1，此时判断“不等于1”==0 就可以
		
        USART_SendData(USART2, szSendData[i]);
	}
	
	myfree(SRAMIN,szSendData);		//释放内存
	
	taskEXIT_CRITICAL();
	
	return true;
}

//串口2发送数据给温度控制板
bool  SerialSendData2Temp(ProtocolType type, unsigned char *fixProtocol, char * data, int dataLength)
{
	taskENTER_CRITICAL();
	
	int fixLength = 4;

	int totalLength = fixLength + dataLength + 2;//完整帧长度 =  固定长度+数据长度+校验和
    
	unsigned char* szSendData = (unsigned char*)mymalloc(SRAMIN,totalLength);
    
	if (type == ProtocolType_1)
	{
		mymemcpy(szSendData, fixProtocol, fixLength);
		mymemcpy(szSendData + fixLength, data, dataLength);
	}
	else if(type == ProtocolType_2)
	{
		mymemcpy(szSendData, fixProtocol, 6);
		mymemcpy(szSendData + 6, data, dataLength);
	}

	//校验和
	unsigned short iCrc = CrcVerifice(szSendData, totalLength - 2);
	szSendData[totalLength - 2] = iCrc & 0Xff;
	szSendData[totalLength - 1] = (iCrc >> 8) & 0xff;
	
	
	for(int i = 0;i<totalLength;i++)
	{
		//vTaskDelay(1); 
		while( USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);         //发送完了，标志位置1，此时判断“不等于1”==0 就可以
		
        USART_SendData(USART2, szSendData[i]);
	}
	
	myfree(SRAMIN,szSendData);		//释放内存
	
	taskEXIT_CRITICAL();
	
	return true;
}
