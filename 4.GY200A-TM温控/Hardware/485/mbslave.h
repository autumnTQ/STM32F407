
 
#ifndef __MBSLAVE_H__
#define __MBSLAVE_H__

#include "main.h"
#include"string.h"
/* 波特率 */
#define MB_BAUD            115200
/* 广播地址 */
#define MB_ADDR_BROADCAST  0
/* 最小从地址 */
#define MB_ADDR_MIN        1
/* 最大从地址 */
#define MB_ADDR_MAX        255
/* 接收和发送缓冲区大小 */
#define MB_BUF_SIZE        255


extern uint8_t M_slave;
/* 线圈状态，可读可写，功能码：01H，05H，0FH */
#define COILS_START        00001
#define COILS_SIZE         10
extern uint8_t coils_buf[COILS_SIZE];

/* 离散输入状态，只读，功能码：02H */
#define DISCRETE_START     256
#define DISCRETE_SIZE      10
extern uint8_t discrete_buf[DISCRETE_SIZE];

/* 输入寄存器，只读，功能码：04H */
#define INPUT_START       256
#define INPUT_NREGS        10
extern uint16_t input_buf[INPUT_NREGS];

/* 保持寄存器，可读可写，功能码：03H，06H，10H */
#define HOLD_START         0
#define HOLD_NREGS         600

extern uint16_t *hold_buf[HOLD_NREGS];

/* 错误代码 */
typedef enum
{
    /* 无错误 */
    MB_NOT_ERR  = 0x00,
    /* 功能码错误 */
    MB_FUN_ERR  = 0x01,
    /* 寄存器地址错误 */
    MB_ADDR_ERR = 0x02,
    /* 数据包错误 */
    MB_DATA_ERR = 0x03,
}MB_ERROR_CODE;

typedef struct
{
    void (*init)(uint32_t baud);
    void (*mode)(uint8_t mode);
    void (*send)(uint8_t *data, uint8_t len);
    /* 接收字节数 */
    uint8_t rx_byte_cnt;
    /* 接收缓冲区 */
    uint8_t rx_buf[MB_BUF_SIZE];
    /* 发送字节数 */
    uint8_t tx_byte_cnt;
    /* 发送缓冲区 */
    uint8_t tx_buf[MB_BUF_SIZE];
    uint8_t slave_id;
    /* 接收完成标志 */
    uint8_t rx_end_flg;
    /* 接收定时标志 */
    uint8_t rx_timer_flg;
    /* 接收超时时间 */
    uint16_t rx_timeout;
}MB_PARAM;


extern MB_PARAM mb_usart3_t;

void mbslave_init(MB_PARAM *port, uint8_t slave_id, uint32_t baud);
void mbslave_timer(MB_PARAM *port);
void mbslave_poll(MB_PARAM *port);

#endif
/*****END OF FILE*****/
