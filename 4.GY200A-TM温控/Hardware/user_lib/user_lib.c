/*
*********************************************************************************************************
*
*	模块名称 : 字符串操作\数值转换
*	文件名称 :user_lib.c
*	版    本 : V1.3a
*	说    明 : 提供一些常用的sting、mem操作函数
*   修改记录 :
*               V1.0  2013-12-05  首版
*               V1.1  2014-06-20  增加大小端整数转换函数
*				V1.2  2015-04-06  增加 BEBufToUint32()和 LEBufToUint32()
*				V1.3  2015-10-09  增加 BcdToChar(), HexToAscll()和 AsciiToUint32()
*				V1.3a 2015-10-09  解决 HexToAscll() 函数末尾补0的BUG
*
*********************************************************************************************************
*/

#include "user_lib.h"


/*
*********************************************************************************************************
*	函 数 名: str_len
*	功能说明: 计算字符串长度.0是结束符
*	形    参: _str : 缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
int str_len(char *_str)
{
	int len = 0;

	while (*_str++) len++;
	return len;
}

/*
*********************************************************************************************************
*	函 数 名: str_cpy
*	功能说明: 复制字符串
*	形    参: tar : 目标缓冲区
*			 src : 源缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void str_cpy(char *_tar, char *_src)
{
	do
	{
		*_tar++ = *_src;
	}
	while (*_src++);
}

/*
*********************************************************************************************************
*	函 数 名: str_cmp
*	功能说明: 字符串比较
*	形    参: s1 : 字符串1
*			  s2 : 字符串2
*	返 回 值: 0 表示相等 非0表示不等
*********************************************************************************************************
*/
int str_cmp(char * s1, char * s2)
{
	while ((*s1!=0) && (*s2!=0) && (*s1==*s2))
	{
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

/*
*********************************************************************************************************
*	函 数 名: str_copy
*	功能说明: 复制字符串
*	形    参: tar : 目标缓冲区
*			 src : 源缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void mem_set(char *_tar, char _data, int _len)
{
	while (_len--)
	{
		*_tar++ = _data;
	}
}

/*
*********************************************************************************************************
*	函 数 名: int_to_ascii
*	功能说明: 将整数转换为ASCII数组。支持负数。
*	形    参: _Number : 整数
*			 _pBuf : 目标缓冲区, 存放转换后的结果。以0结束的字符串。
*			 _len : ASCII字符个数, 字符串长度
*	返 回 值: 无
*********************************************************************************************************
*/
void int_to_str(int _iNumber, char *_pBuf, unsigned char _len)
{
	unsigned char i;
	int iTemp;

	if (_iNumber < 0)	/* 负数 */
	{
		iTemp = -_iNumber;	/* 转为正数 */
	}
	else
	{
		iTemp = _iNumber;
	}

	mem_set(_pBuf, ' ',_len);

	/* 将整数转换为ASCII字符串 */
	for (i = 0; i < _len; i++)
	{
		_pBuf[_len - 1 - i] = (iTemp % 10) + '0';
		iTemp = iTemp / 10;
		if (iTemp == 0)
		{
			break;
		}
	}
	_pBuf[_len] = 0;

	if (_iNumber < 0)	/* 负数 */
	{
		for (i = 0; i < _len; i++)
		{
			if ((_pBuf[i] == ' ') && (_pBuf[i + 1] != ' '))
			{
				_pBuf[i] = '-';
				break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: str_to_int
*	功能说明: 将ASCII码字符串转换成整数。 遇到小数点自动越过。
*	形    参: _pStr :待转换的ASCII码串. 可以以逗号，#或0结束。 2014-06-20 修改为非0-9的字符。
*	返 回 值: 二进制整数值
*********************************************************************************************************
*/
int str_to_int(char *_pStr)
{
	unsigned char flag;
	char *p;
	int ulInt;
	unsigned char  i;
	unsigned char  ucTemp;

	p = _pStr;
	if (*p == '-')
	{
		flag = 1;	/* 负数 */
		p++;
	}
	else
	{
		flag = 0;
	}

	ulInt = 0;
	for (i = 0; i < 15; i++)
	{
		ucTemp = *p;
		if (ucTemp == '.')	/* 遇到小数点，自动跳过1个字节 */
		{
			p++;
			ucTemp = *p;
		}
		if ((ucTemp >= '0') && (ucTemp <= '9'))
		{
			ulInt = ulInt * 10 + (ucTemp - '0');
			p++;
		}
		else
		{
			break;
		}
	}

	if (flag == 1)
	{
		return -ulInt;
	}
	return ulInt;
}

/*
*********************************************************************************************************
*	函 数 名: BEBufToUint16
*	功能说明: 将2字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint16_t BEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[0] << 8) | _pBuf[1]);
}

/*
*********************************************************************************************************
*	函 数 名: LEBufToUint16
*	功能说明: 将2字节数组(小端Little Endian，低字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*********************************************************************************************************
*/
uint16_t LEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[1] << 8) | _pBuf[0]);
}


/*
*********************************************************************************************************
*	函 数 名: BEBufToUint32
*	功能说明: 将4字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint32_t BEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[0] << 24) | ((uint32_t)_pBuf[1] << 16) | ((uint32_t)_pBuf[2] << 8) | _pBuf[3]);
}

/*
*********************************************************************************************************
*	函 数 名: LEBufToUint32
*	功能说明: 将4字节数组(小端Little Endian，低字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*********************************************************************************************************
*/
uint32_t LEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[3] << 24) | ((uint32_t)_pBuf[2] << 16) | ((uint32_t)_pBuf[1] << 8) | _pBuf[0]);
}



/*
*********************************************************************************************************
*	函 数 名: CaculTwoPoint
*	功能说明: 根据2点直线方程，计算Y值
*	形    参:  2个点的坐标和x输入量
*	返 回 值: x对应的y值
*********************************************************************************************************
*/
int32_t  CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x)
{
	return y1 + ((int64_t)(y2 - y1) * (x - x1)) / (x2 - x1);
}


/*
*********************************************************************************************************
*	函 数 名: BcdToChar
*	功能说明: 将BCD码转为ASCII字符。 比如 0x0A ==> 'A'
*	形    参: _bcd   ：输入的二进制数。必须小于16
*	返 回 值: 转换结果
*********************************************************************************************************
*/
char BcdToChar(uint8_t _bcd)
{
	if (_bcd < 10)
	{
		return _bcd + '0';
	}
	else if (_bcd < 16)
	{
		return _bcd + 'A';
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: HexToAscll
*	功能说明: 将二进制数组转换为16进制格式的ASCII字符串。每个2个ASCII字符后保留1个空格。
*			  0x12 0x34 转化为 0x31 0x32 0x20 0x33 0x34 0x00  即 "1234"
*	形    参: 	_pHex   ：输入的数据，二进制数组
*				_pAscii ：存放转换结果, ASCII字符串，0结束。1个二进制对应2个ASCII字符.
*	返 回 值: 转换得到的整数
*********************************************************************************************************
*/
void HexToAscll(uint8_t * _pHex, char *_pAscii, uint16_t _BinBytes)
{
	uint16_t i;
	
	if (_BinBytes == 0)
	{
		_pAscii[0] = 0;
	}
	else
	{
		for (i = 0; i < _BinBytes; i++)
		{
			_pAscii[3 * i] = BcdToChar(_pHex[i] >> 4);
			_pAscii[3 * i + 1] = BcdToChar(_pHex[i] & 0x0F);
			_pAscii[3 * i + 2] = ' ';
		}
		_pAscii[3 * (i - 1) + 2] = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: AsciiToUint32
*	功能说明: 变长的 ASCII 字符转换为32位整数  ASCII 字符以空格或者0结束 。 支持16进制和10进制输入
*	形    参: *pAscii ：要转换的ASCII码
*	返 回 值: 转换得到的整数
*********************************************************************************************************
*/
uint32_t AsciiToUint32(char *pAscii)
{
	char i;
	char bTemp;
	char bIsHex;
	char bLen;
	char bZeroLen;
	uint32_t lResult;
	uint32_t lBitValue;

	/* 判断是否是16进制数 */
	bIsHex = 0;
	if ((pAscii[0] == '0') && ((pAscii[1] == 'x') || (pAscii[1] == 'X')))
	{
		bIsHex=1;
	}

	lResult=0;
	// 最大数值为 4294967295, 10位+2字符"0x" //
	if (bIsHex == 0)
	{ // 十进制 //
		// 求长度 //
		lBitValue=1;

		/* 前导去0 */
		for (i = 0; i < 8; i++)
		{
			bTemp = pAscii[i];
			if (bTemp != '0')
				break;
		}
		bZeroLen = i;

		for (i = 0; i < 10; i++)
		{
			if ((pAscii[i] < '0') || (pAscii[i] > '9'))
				break;
			lBitValue = lBitValue * 10;
		}
		bLen = i;
		lBitValue = lBitValue / 10;
		if (lBitValue == 0)
			lBitValue=1;
		for (i = bZeroLen; i < bLen; i++)
		{
			lResult += (pAscii[i] - '0') * lBitValue;
			lBitValue /= 10;
		}
	}
	else
	{	/* 16进制 */
		/* 求长度 */
		lBitValue=1;

		/* 前导去0 */
		for (i = 0; i < 8; i++)
		{
			bTemp = pAscii[i + 2];
			if(bTemp!='0')
				break;
		}
		bZeroLen = i;
		for (; i < 8; i++)
		{
			bTemp=pAscii[i+2];
			if (((bTemp >= 'A') && (bTemp <= 'F')) ||
				((bTemp>='a')&&(bTemp<='f')) ||
				((bTemp>='0')&&(bTemp<='9')) )
			{
				lBitValue=lBitValue * 16;
			}
			else
			{
				break;
			}
		}
		lBitValue = lBitValue / 16;
		if (lBitValue == 0)
			lBitValue = 1;
		bLen = i;
		for (i = bZeroLen; i < bLen; i++)
		{
			bTemp = pAscii[i + 2];
			if ((bTemp >= 'A') && (bTemp <= 'F'))
			{
				bTemp -= 0x37;
			}
			else if ((bTemp >= 'a') && (bTemp <= 'f'))
			{
				bTemp -= 0x57;
			}
			else if ((bTemp >= '0') && (bTemp <= '9'))
			{
				bTemp -= '0';
			}
			lResult += bTemp*lBitValue;
			lBitValue /= 16;
		}
	}
	return lResult;
}
 void Int_To_Hex( uint32_t integer )
    {
        uint8_t c;
        int     i, j, digit;
        
        for ( i = 7, j = 0; i >= 0; i--, j++ )
        {
            digit = (integer >> (i * 4)) & 0xf;
            if (digit < 10)
            {
                c = digit + 0x30;
            }
            else
            {
                c = digit + 0x37;
            }
        }
    }
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
