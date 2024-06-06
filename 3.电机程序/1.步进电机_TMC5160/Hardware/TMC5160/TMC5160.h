#ifndef _TMC5160_H_
#define _TMC5160_H_

#include "stm32f10x.h"

extern void Delay_MS(unsigned int TimeMS);
extern void Delay_US(unsigned int TimeUS);
extern uint16_t STATUS;

uint32_t TMC5160_SPIWriteInt(uint8_t add, long write_data);

void TMC5160_IO_Init(void);
void TMC5160_Regest_Init(void);
void TMC5160_xi_fen_set(void);//设置细分
void TMC5160_current_set(void);//设置电流
void TMC5160_JiaSu_Set(void);//加速设置
void TMC5160_XTARGET_Set(void);//运行到目标位置设置
void  TMC5160_DRV_STATUS_Return(void);//获取状态
void  TMC5160_ENCMODE_Set(void) ;//编码器设置

void TMC5160_Regest_Init_Test(void);
uint32_t TMC5160_SPIReadInt(uint8_t add, long write_data);
void zero_target(void);
void Move(long PosiTionData); // 移动数据

void mmymian(long PositionData);


#endif 

