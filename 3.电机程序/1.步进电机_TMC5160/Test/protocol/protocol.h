
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "stm32f10x.h"
#include "bsp_debug_usart.h"

#ifdef _cplusplus
extern "C"
{
#endif

/* 数据接收缓冲区大�? */
#define PROT_FRAME_LEN_RECV 128

/* 校验数据的长�? */
#define PROT_FRAME_LEN_CHECKSUM 1

  /* 数据头结构体 */
  typedef __packed struct
//  typedef  struct
  {
    uint32_t head; // 包头
    uint8_t ch;    // 通道
    uint32_t len;  // 包长�?
    uint8_t cmd;   // 命令
    //  uint8_t sum;      // 校验�?

  } packet_head_t;

#define FRAME_HEADER 0x59485A53 // 帧头

/* 通道宏定�? */
#define CURVES_CH1 0x01
#define CURVES_CH2 0x02
#define CURVES_CH3 0x03
#define CURVES_CH4 0x04
#define CURVES_CH5 0x05

/* 指令(下位�? -> 上位�?) */
#define SEND_TARGET_CMD 0x01 // 发送上位机通道的目标�?
#define SEND_FACT_CMD 0x02   // 发送通道实际�?
#define SEND_P_I_D_CMD 0x03  // 发�? PID 值（同�?�上位机显示的值）
#define SEND_START_CMD 0x04  // 发送启动指令（同�?�上位机按钮状态）
#define SEND_STOP_CMD 0x05   // 发送停止指令（同�?�上位机按钮状态）
#define SEND_PERIOD_CMD 0x06 // 发送周期（同�?�上位机显示的值）

/* 指令(上位�? -> 下位�?) */
#define SET_P_I_D_CMD 0x10  // 设置 PID �?
#define SET_TARGET_CMD 0x11 // 设置�?标�?
#define START_CMD 0x12      // �?动指�?
#define STOP_CMD 0x13       // 停�?�指�?
#define RESET_CMD 0x14      // 复位指令
#define SET_PERIOD_CMD 0x15 // 设置周期

/* 空指�? */
#define CMD_NONE 0xFF // 空指�?

/* 索引值宏定义 */
#define HEAD_INDEX_VAL 0x3u // 包头索引值（4字节�?
#define CHX_INDEX_VAL 0x4u  // 通道索引值（1字节�?
#define LEN_INDEX_VAL 0x5u  // 包长索引值（4字节�?
#define CMD_INDEX_VAL 0x9u  // 命令索引值（1字节�?

#define EXCHANGE_H_L_BIT(data) ((((data) << 24) & 0xFF000000) | \
                                (((data) << 8) & 0x00FF0000) |  \
                                (((data) >> 8) & 0x0000FF00) |  \
                                (((data) >> 24) & 0x000000FF)) // 交换高低字节

#define COMPOUND_32BIT(data) (((*(data - 0) << 24) & 0xFF000000) | \
                              ((*(data - 1) << 16) & 0x00FF0000) | \
                              ((*(data - 2) << 8) & 0x0000FF00) |  \
                              ((*(data - 3) << 0) & 0x000000FF)) // 合成为一�?�?

  /**
   * @brief   接收数据处理
   * @param   *data:  要�?�算的数�?的数�?.
   * @param   data_len: 数据的大�?
   * @return  void.
   */
  void protocol_data_recv(uint8_t *data, uint16_t data_len);

  /**
   * @brief   初�?�化接收协�??
   * @param   void
   * @return  初�?�化结果.
   */
  int32_t protocol_init(void);

  /**
   * @brief   接收的数�?处理
   * @param   void
   * @return  -1：没有找到一�?正确的命�?.
   */
  int8_t receiving_process(void);

  /**
   * @brief 设置上位机的�?
   * @param cmd：命�?
   * @param ch: 曲线通道
   * @param data：参数指�?
   * @param num：参数个�?
   * @retval �?
   */
  void set_computer_value(uint8_t cmd, uint8_t ch, void *data, uint8_t num);



void usart1_senddata(u8*data, u8 len);

#ifdef _cplusplus
}
#endif

#endif
