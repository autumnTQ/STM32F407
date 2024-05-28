#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 


//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}


/*
 *函数功能：STM32软复位函数
 */
 void Soft_Reset(void)
 {
   __set_FAULTMASK(1);//禁止所有的可屏蔽中断
   NVIC_SystemReset();//软件复位
 }












