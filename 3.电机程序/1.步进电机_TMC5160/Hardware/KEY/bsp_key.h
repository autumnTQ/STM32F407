#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f10x.h"
#include "Config.h"

#define KEY_Ok GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
#define KEY_Jia GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)
#define KEY_Jian GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)
#define Key (KEY_Ok && KEY_Jia && KEY_Jian)

typedef enum
{
    KEY_CHECK = 0,
    KEY_COMFIRM = 1,
    KEY_RELEASE = 2,
} KEY_STATE;

// 对应的按键值，
typedef enum
{
    KEY_NULL = 0,
    KEY_Ok_UP,
    KEY_Jia_UP,
    KEY_Jian_UP
} KEY_VALUE;

// 按键状态 短按 长按
typedef enum
{
    NULL_KEY = 0,
    SHORT_KEY = 1,
    LONG_KEY
} KEY_TYPE;

void Key_Init(void);
void Key_Scan(void);
void Key_State(void);



#endif
