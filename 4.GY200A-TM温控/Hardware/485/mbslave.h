
 
#ifndef __MBSLAVE_H__
#define __MBSLAVE_H__

#include "main.h"
#include"string.h"
/* ������ */
#define MB_BAUD            115200
/* �㲥��ַ */
#define MB_ADDR_BROADCAST  0
/* ��С�ӵ�ַ */
#define MB_ADDR_MIN        1
/* ���ӵ�ַ */
#define MB_ADDR_MAX        255
/* ���պͷ��ͻ�������С */
#define MB_BUF_SIZE        255


extern uint8_t M_slave;
/* ��Ȧ״̬���ɶ���д�������룺01H��05H��0FH */
#define COILS_START        00001
#define COILS_SIZE         10
extern uint8_t coils_buf[COILS_SIZE];

/* ��ɢ����״̬��ֻ���������룺02H */
#define DISCRETE_START     256
#define DISCRETE_SIZE      10
extern uint8_t discrete_buf[DISCRETE_SIZE];

/* ����Ĵ�����ֻ���������룺04H */
#define INPUT_START       256
#define INPUT_NREGS        10
extern uint16_t input_buf[INPUT_NREGS];

/* ���ּĴ������ɶ���д�������룺03H��06H��10H */
#define HOLD_START         0
#define HOLD_NREGS         600

extern uint16_t *hold_buf[HOLD_NREGS];

/* ������� */
typedef enum
{
    /* �޴��� */
    MB_NOT_ERR  = 0x00,
    /* ��������� */
    MB_FUN_ERR  = 0x01,
    /* �Ĵ�����ַ���� */
    MB_ADDR_ERR = 0x02,
    /* ���ݰ����� */
    MB_DATA_ERR = 0x03,
}MB_ERROR_CODE;

typedef struct
{
    void (*init)(uint32_t baud);
    void (*mode)(uint8_t mode);
    void (*send)(uint8_t *data, uint8_t len);
    /* �����ֽ��� */
    uint8_t rx_byte_cnt;
    /* ���ջ����� */
    uint8_t rx_buf[MB_BUF_SIZE];
    /* �����ֽ��� */
    uint8_t tx_byte_cnt;
    /* ���ͻ����� */
    uint8_t tx_buf[MB_BUF_SIZE];
    uint8_t slave_id;
    /* ������ɱ�־ */
    uint8_t rx_end_flg;
    /* ���ն�ʱ��־ */
    uint8_t rx_timer_flg;
    /* ���ճ�ʱʱ�� */
    uint16_t rx_timeout;
}MB_PARAM;


extern MB_PARAM mb_usart3_t;

void mbslave_init(MB_PARAM *port, uint8_t slave_id, uint32_t baud);
void mbslave_timer(MB_PARAM *port);
void mbslave_poll(MB_PARAM *port);

#endif
/*****END OF FILE*****/
