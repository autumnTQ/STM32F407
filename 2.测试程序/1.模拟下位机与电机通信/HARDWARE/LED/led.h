#ifndef __LED_H
#define __LED_H
#include "sys.h"

#define LED13_Toggle	GPIO_ToggleBits(GPIOC, GPIO_Pin_13)		// LED×´Ì¬·­×ª

void LED_Init(void);//³õÊ¼»¯	


#endif
