/*
 * Copyright (c) 2006-2100, Tommy Guo
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-27     Tommy        first version
 */
 
#ifndef __WDG_H__
#define __WDG_H__

#include "main.h" 
//#include "stm32f10x_iwdg.h"
extern uint8_t iwdg_feed_flg;

void iwdg_init(uint8_t prer, uint16_t rlr);
void iwdg_feed(void);
void wwdg_init(uint8_t tr, uint8_t wr, uint32_t prer);

#endif
/*****END OF FILE*****/
