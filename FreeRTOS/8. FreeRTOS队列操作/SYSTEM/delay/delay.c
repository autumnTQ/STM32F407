#include "delay.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
// 如果使用OS,则包括下面的头文件（以ucos为例）即可.
#if SYSTEM_SUPPORT_OS

#include "FreeRTOS.h"
#include "task.h"

#endif
//////////////////////////////////////////////////////////////////////////////////
// 本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32F407开发板
// 使用SysTick的普通计数模式对延迟进行管理(支持OS)
// 包括delay_us,delay_ms
// 正点原子@ALIENTEK
// 技术论坛:www.openedv.com
// 创建日期:2014/5/2
// 版本：V1.3
// 版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2014-2024
// All rights reserved
//********************************************************************************
// 修改说明
// V1.1 20140803
// 1,delay_us,添加参数等于0判断,如果参数等于0,则直接退出.
// 2,修改ucosii下,delay_ms函数,加入OSLockNesting的判断,在进入中断后,也可以准确延时.
// V1.2 20150411
// 修改OS支持方式,以支持任意OS(不限于UCOSII和UCOSIII,理论上任意OS都可以支持)
// 添加:delay_osrunning/delay_ostickspersec/delay_osintnesting三个宏定义
// 添加:delay_osschedlock/delay_osschedunlock/delay_ostimedly三个函数
// V1.3 20150521
// 修正UCOSIII支持时的2个bug：
// delay_tickspersec改为：delay_ostickspersec
// delay_intnesting改为：delay_osintnesting
//////////////////////////////////////////////////////////////////////////////////

static u8 fac_us = 0; // us延时倍乘数

extern void xPortSysTickHandler(void);

// systick中断服务函数,使用OS时用到
void SysTick_Handler(void)
{
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		xPortSysTickHandler();
	}
}

// 初始化延迟函数
// 当使用OS的时候,此函数会初始化OS的时钟节拍
// SYSTICK的时钟固定为AHB时钟的1/8
// SYSCLK:系统时钟频率
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS // 如果需要支持OS.
	u32 reload;
#endif
	/******************************************************************************************
	SySTick_Config，SysTick_CLKSourceConfig。SySTick_Config函数是总体给中断初始化一下，
	初始化的过程中它给systick的时钟配置成了默认的AHB时钟，还可以配置成AHB的八分频，这
	就需要用到SysTick_CLKSourceConfig这个函数

	AHB最大时钟为168MHz，APB2高速时钟最大频率为84MHz，而APB1低速时钟最大频率为42MHz
	————————————————
	版权声明：本文为CSDN博主「hhhjb2」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
	原文链接：https://blog.csdn.net/hhhjb2/article/details/114435638
	********************************************************************************************/
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 168/8=21M
	fac_us = SYSCLK;									  // 不论是否使用OS,fac_us都需要使用
#if SYSTEM_SUPPORT_OS									  // 如果需要支持OS.
	reload = SYSCLK;									  // 每秒钟的计数次数 单位为M
	reload *= 1000000 / configTICK_RATE_HZ;				  // 根据delay_ostickspersec设定溢出时间
											// reload为24位寄存器,最大值:16777216,在168M下,约合0.7989s左右
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // 开启SYSTICK中断
	SysTick->LOAD = reload;					   // 每1/delay_ostickspersec秒中断一次
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  // 开启SYSTICK
#endif
}

#if SYSTEM_SUPPORT_OS // 如果需要支持OS.
// 延时nus
// nus:要延时的us数.
// nus:0~204522252(最大值即2^32/fac_us@fac_us=21)
void delay_us(u32 nus)
{
	u32 ticks;
	u32 told, tnow, tcnt = 0;
	u32 reload = SysTick->LOAD; // LOAD的值
	ticks = nus * fac_us;		// 需要的节拍数
	told = SysTick->VAL;		// 刚进入时的计数器值
	while (1)
	{
		tnow = SysTick->VAL;
		if (tnow != told)
		{
			if (tnow < told)
				tcnt += told - tnow; // 这里注意一下SYSTICK是一个递减的计数器就可以了.
			else
				tcnt += reload - tnow + told;
			told = tnow;
			if (tcnt >= ticks)
				break; // 时间超过/等于要延迟的时间,则退出.
		}
	};
}
// 延时nms
// nms:要延时的ms数
// nms:0~65535
void delay_ms(u16 nms)
{
	uint32_t i;

	for (i = 0; i < nms; i++)
	{
		delay_us(1000);
	}
}
#else // 不用ucos时
// 延时nus
// nus为要延时的us数.
// 注意:nus的值,不要大于798915us(最大值即2^24/fac_us@fac_us=21)
void delay_us(u32 nus)
{
	u32 temp;
	SysTick->LOAD = nus * fac_us;			  // 时间加载
	SysTick->VAL = 0x00;					  // 清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开始倒数
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;		// 关闭计数器
	SysTick->VAL = 0X00;							// 清空计数器
}
// 延时nms
// 注意nms的范围
// SysTick->LOAD为24位寄存器,所以,最大延时为:
// nms<=0xffffff*8*1000/SYSCLK
// SYSCLK单位为Hz,nms单位为ms
// 对168M条件下,nms<=798ms
void delay_xms(u16 nms)
{
	u32 temp;
	SysTick->LOAD = (u32)nms * fac_ms;		  // 时间加载(SysTick->LOAD为24bit)
	SysTick->VAL = 0x00;					  // 清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开始倒数
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;		// 关闭计数器
	SysTick->VAL = 0X00;							// 清空计数器
}
// 延时nms
// nms:0~65535
void delay_ms(u16 nms)
{
	u8 repeat = nms / 540; // 这里用540,是考虑到某些客户可能超频使用,
						   // 比如超频到248M的时候,delay_xms最大只能延时541ms左右了
	u16 remain = nms % 540;
	while (repeat)
	{
		delay_xms(540);
		repeat--;
	}
	if (remain)
		delay_xms(remain);
}
#endif
