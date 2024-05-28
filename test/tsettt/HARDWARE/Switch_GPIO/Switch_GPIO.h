#ifndef __SWITCH_GPIO_H
#define __SWITCH_GPIO_H

#include "stm32f4xx.h" 

//GPIOÃ¶¾Ù
typedef enum SwGPIO
{
    Sw_GPIOA = 1,
    Sw_GPIOB,
    Sw_GPIOC,
    Sw_GPIOD,
    Sw_GPIOE,
    Sw_GPIOF,
    Sw_GPIOG,
}SwGPIOType;


uint8_t Switch_GPIO(SwGPIOType type, uint16_t GPIO_Pin);
void Switch_GPIOInit(void);

#endif

