/*
 * Copyright (c) 2006-2100, Tommy Guo
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-05-28     Tommy        first version
 */

#include "mbslave.h"

extern   float P,I,D;
uint8_t  buf1[22]={0};
extern uint8_t buf[EE_SIZE];	/* 数据测试用缓冲区 16KB */

/* crc高位字节值表 */
static const uint8_t crc_hi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* crc低位字节值表 */
const uint8_t crc_lo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
u16 xxxxx[2]={1,2};
/* 线圈状态，可读可写，功能码：01H，05H，0FH */
uint8_t  coils_buf[COILS_SIZE] = {0};
/* 离散输入状态，只读，功能码：02H */
uint8_t  discrete_buf[DISCRETE_SIZE] = {0};
/* 输入寄存器，只读，功能码：04H */
uint16_t input_buf[INPUT_NREGS] = {0};
/* 保持寄存器，可读可写，功能码：03H，06H，10H */
uint16_t *hold_buf[HOLD_NREGS] = {&xxxxx[1],&xxxxx[0]};


MB_PARAM mb_usart3_t = {.init = usart3_init, .mode = usart3_mode, .send = usart3_send};


/**
 * @brief  计算crc校验码
 * @param  buf: 参与校验的数据
 * @param  len: 数据长度
 * @retval crc：16位整数值，高字节在前，低字节在后
 */
static uint16_t crc16_calc(uint8_t *buf, uint16_t len)
{ 
    uint8_t  byte_hi = 0xFF;
    uint8_t  byte_lo = 0xFF;
    uint16_t index;

    while (len--)
    {
        index = byte_hi ^ *buf++;
        byte_hi = byte_lo ^ crc_hi[index];
        byte_lo = crc_lo[index];
    }

    return (uint16_t)(byte_hi << 8 | byte_lo);
}

/**
 * @brief  读线圈状态，属于DO数字量输出，如电磁阀状态，LED状态等
 * @param  port：串口号
 * @retval 错误码
 * @eg     主机发：从机地址（1字节）+ 01 + 起始地址（2字节）+ 线圈数量（2字节）
 * @eg     从机回：从机地址（1字节）+ 01 + 字节数（1字节）  + 线圈状态（N*1字节）
 */
static uint8_t mbslave_01H(MB_PARAM *port)
{
    uint8_t i = 0, k = 0;
    uint8_t coils_state = 0, byte_num = 0, exit = 0;
    uint16_t addr = 0, coils_addr = 0, num = 0;//数据地址和数据长度记录

    addr = (port->rx_buf[2] << 8) + port->rx_buf[3];
    coils_addr = addr;//数据地址
    num = (port->rx_buf[4] << 8) + port->rx_buf[5];
    byte_num = num / 8;//数据长度
    if (num % 8 != 0)//非8位字节整数
    {
        byte_num++;//补齐字节数
    }

    /* 组织数据包响应主机 */
    port->tx_buf[0] = port->rx_buf[0];
    port->tx_buf[1] = port->rx_buf[1];
    port->tx_buf[2] = byte_num;
    port->tx_byte_cnt = byte_num + 3;

    if (addr < COILS_START || addr + num > COILS_START + COILS_SIZE)
    {
        return MB_ADDR_ERR;
    }

    for (k = 0; k < byte_num; k++)
    {
        port->tx_buf[k + 3] = 0;
        for (i = 0; i < 8; i++)
        {
            port->tx_buf[k + 3] |= coils_state << i;
            coils_addr++;
            if (coils_addr >= addr + num)
            {
                exit = 1;
                break;
            }
        }
        if (exit == 1)
        {
            break;
        }
    }

    return MB_NOT_ERR;
}

/**
 * @brief  读离散输入状态，属于DI数字量输入，如按键状态，拨码开关状态等
 * @param  port：串口号
 * @retval 错误码
 * @eg     主机发：从机地址（1字节）+ 02 + 起始地址（2字节）+ 寄存器数量（2字节）
 * @eg     从机回：从机地址（1字节）+ 02 + 字节数（1字节）  + 寄存器值（N*1字节）
 * @eg     discrete_buf[0] = 1，对应离散输入量的状态为ON，否则为OFF
 */
static uint8_t mbslave_02H(MB_PARAM *port)
{
    uint8_t i = 0, k = 0;
    uint8_t discrete_state = 0, byte_num = 0, exit = 0;
    uint16_t addr = 0, discrete_addr = 0, num = 0;

    addr = (port->rx_buf[2] << 8) + port->rx_buf[3];
    discrete_addr = addr;
    num = (port->rx_buf[4] << 8) + port->rx_buf[5];
    byte_num = num / 8;
    if (num % 8 != 0)
    {
        byte_num++;
    }

    /* 组织数据包响应主机 */
    port->tx_buf[0] = port->rx_buf[0];
    port->tx_buf[1] = port->rx_buf[1];
    port->tx_buf[2] = byte_num;
//    port->tx_byte_cnt = byte_num + 3;
    port->tx_byte_cnt = 3;
//    if (addr < DISCRETE_START || addr + num > DISCRETE_START + DISCRETE_SIZE)
	    if (addr < DISCRETE_START)
    {
        return MB_ADDR_ERR;
    }

    for (k = 0; k < byte_num; k++)
    {
        port->tx_buf[k + 3] = 0;
        for (i = 0; i < 8; i++)
        {
            port->tx_buf[k + 3] |= discrete_state << i;
            discrete_addr++;
            if (discrete_addr >= addr + num)
            {
                exit = 1;
                break;
            }
        }
        if (exit == 1)
        {
            break;
        }
    }

    return MB_NOT_ERR;
}

/**
 * @brief  读保持寄存器，属于AO模拟量输出，如变量阈值，PID运行参数，传感器报警上下限参数等
 * @param  port：串口号
 * @retval 错误码
 * @eg     主机发：从机地址（1字节）+ 03 + 起始地址（2字节）+ 寄存器数量（2字节）
 * @eg     从机回：从机地址（1字节）+ 03 + 字节数（1字节）  + 寄存器值（N*2字节）
 */

static uint8_t mbslave_03H(MB_PARAM *port)
{
    uint8_t i = 0, byte_num = 0;
    uint16_t addr = 0, hold_addr = 0, num = 0, hold_value = 0;

    addr = (port->rx_buf[2] << 8) + port->rx_buf[3];
    hold_addr = addr;
    num = (port->rx_buf[4] << 8) + port->rx_buf[5];
    byte_num = num * 2;

    /* 组织数据包响应主机 */
    port->tx_buf[0] = port->rx_buf[0];
    port->tx_buf[1] = port->rx_buf[1];
    port->tx_buf[2] = byte_num;
    port->tx_byte_cnt = 1;

    if (addr < HOLD_START || addr + num > HOLD_START+ 10)
    {
        return MB_ADDR_ERR;
    }

    for (i = 0; i < num; i++, hold_addr++)
    {
        port->tx_buf[port->tx_byte_cnt++] = hold_value >> 8;
        port->tx_buf[port->tx_byte_cnt++] = hold_value & 0xff;
    }

    return MB_NOT_ERR;
}

/**
 * @brief  读输入寄存器，属于AI模拟量输出，如电压值、电流值、温度值、发电量等
 * @param  port：串口号
 * @retval 错误码
 * @eg     主机发：从机地址（1字节）+ 04 + 起始地址（2字节）+ 寄存器数量（2字节）
 * @eg     从机回：从机地址（1字节）+ 04 + 字节数（1字节）  + 寄存器值（N*2字节）
 */
static uint8_t mbslave_04H(MB_PARAM *port)
{
    uint8_t i = 0, byte_num = 0;
    uint16_t addr = 0, input_addr = 0, num = 0, input_value = 0;

    addr = (port->rx_buf[2] << 8) + port->rx_buf[3];
    input_addr = addr;
    num = (port->rx_buf[4] << 8) + port->rx_buf[5];
    byte_num = num * 2;

    /* 组织数据包响应主机 */
    port->tx_buf[0] = port->rx_buf[0];
    port->tx_buf[1] = port->rx_buf[1];
//    port->tx_buf[2] = byte_num;
    port->tx_byte_cnt = 1;

//    if (addr < INPUT_START || addr + num > INPUT_START + INPUT_NREGS)
	    if (addr < INPUT_START )
    {
        return MB_ADDR_ERR;
    }

    for (i = 0; i < num; i++, input_addr++)
    {
        port->tx_buf[port->tx_byte_cnt++] = input_value >> 8;
        port->tx_buf[port->tx_byte_cnt++] = input_value & 0xff;
    }

    return MB_NOT_ERR;
}

/**
 * @brief  写单个线圈，属于DO数字量输出，如电磁阀输出，LED显示等
 * @param  port：串口号
 * @retval 错误码
 * @eg     主机发：从机地址（1字节）+ 05 + 地址（2字节）+ 寄存器值（0x0000关，0xFF00开）（2字节）
 * @eg     从机回：从机地址（1字节）+ 05 + 地址（2字节）+ 寄存器值（0x0000关，0xFF00开）（2字节）
 */
static uint8_t mbslave_05H(MB_PARAM *port)
{
    uint8_t on_off = 0, coils_state = 0;
    uint16_t coils_addr = 0;

    coils_addr = (port->rx_buf[2] << 8) + port->rx_buf[3];

    if (coils_addr < COILS_START || coils_addr > COILS_START + COILS_SIZE - 1)
    {
        return MB_ADDR_ERR;
    }

    on_off = port->rx_buf[4];
    if (on_off == 0xFF)
    {
        coils_state = 1;
    }
    else if (on_off == 0x00)
    {
        coils_state = 0;
    }

    /* 组织数据包响应主机 */
    port->tx_byte_cnt = 6;
    memcpy(port->tx_buf, port->rx_buf, port->tx_byte_cnt);

    return MB_NOT_ERR;
}

/**
 * @brief  写单个保持寄存器，属于AO模拟量输出，如变量阈值，PID运行参数，传感器报警上下限参数等
 * @param  port：串口号
 * @retval 错误码
 * @eg     主机发：从机地址（1字节）+ 06 + 地址（2字节）+ 寄存器值（2字节）
 * @eg     从机回：从机地址（1字节）+ 06 + 地址（2字节）+ 寄存器值（2字节）
 */
static uint8_t mbslave_06H(MB_PARAM *port)
{
    uint16_t hold_addr = 0;

    hold_addr = (port->rx_buf[2] << 8) + port->rx_buf[3];

    if (hold_addr < HOLD_START || hold_addr > HOLD_START + HOLD_NREGS - 1)
    {
        return MB_ADDR_ERR;
    }


    /* 组织数据包响应主机 */
    port->tx_byte_cnt = 6;
    memcpy(port->tx_buf, port->rx_buf, port->tx_byte_cnt);

    return MB_NOT_ERR;
}

/**
 * @brief  写多个线圈，属于DO数字量输出，如电磁阀输出，LED显示等
 * @param  port：串口号
 * @retval 错误码
 * @eg     主机发：从机地址（1字节）+ 0F + 起始地址（2字节）+ 寄存器数量（2字节）+ 字节数（1字节）+ 寄存器值（N*1字节）
 * @eg     从机回：从机地址（1字节）+ 0F + 起始地址（2字节）+ 寄存器数量（2字节）
 * @eg     addr = 1，num = 9，value = {1,1,1,1,0,1,1,1, 1}，发送的寄存器值为：F7 01，对应的线圈地址：8 7 6 5 4 3 2 1  9
 */
static uint8_t mbslave_0FH(MB_PARAM *port)
{
    uint8_t i = 0, k = 0;
    uint8_t coils_state = 0, byte_num = 0, exit = 0;
    uint16_t addr = 0, coils_addr = 0, num = 0;

    addr = (port->rx_buf[2] << 8) + port->rx_buf[3];
    coils_addr = addr;
    num =(port->rx_buf[4] << 8) + port->rx_buf[5];
    byte_num = port->rx_buf[6];

    if (addr < COILS_START || addr + num > COILS_START + COILS_SIZE)
    {
        return MB_ADDR_ERR;
    }

    for (k = 0; k < byte_num; k++)
    {
        for (i = 0; i < 8; i++)
        {
            coils_state = (port->rx_buf[7 + k] >> i) & 0x01;
            coils_addr++;
            if (coils_addr >= addr + num)
            {
                exit = 1;
                break;
            }
        }
        if (exit == 1)
        {
            break;
        }
    }

    /* 组织数据包响应主机 */
    port->tx_byte_cnt = 6;
    memcpy(port->tx_buf, port->rx_buf, port->tx_byte_cnt);

    return MB_NOT_ERR;
}

/**
 * @brief  写多个保持寄存器，属于AO模拟量输出，如变量阈值，PID运行参数，传感器报警上下限参数等
 * @param  port：串口号
 * @retval 错误码
 * @eg     主机发：从机地址（1字节）+ 10 + 起始地址（2字节）+ 寄存器数量（2字节）+ 字节数（1字节）+ 寄存器值（N*2字节）
 * @eg     从机回：从机地址（1字节）+ 10 + 起始地址（2字节）+ 寄存器数量（2字节）
 */
static uint8_t mbslave_10H(MB_PARAM *port)
{
    uint8_t i = 0;
    uint16_t addr = 0, hold_addr = 0, num = 0, hold_value = 0;

    addr = (port->rx_buf[2] << 8) + port->rx_buf[3];
    hold_addr = addr;
    num =  port->rx_buf[4];
   if (addr < HOLD_START || addr  > HOLD_START + HOLD_NREGS)	
    {
        return MB_ADDR_ERR;
    }

    for (i = 0; i < num; i++, hold_addr++)
    {
        hold_value = (port->rx_buf[i * 2 + 7] << 8) + port->rx_buf[i * 2 + 8];

    }

    /* 组织数据包响应主机 */
    port->tx_byte_cnt = 8;
    memcpy(port->tx_buf, port->rx_buf, port->tx_byte_cnt);

    return MB_NOT_ERR;
}

/**
 * @brief  验证接收的数据
 * @param  port：串口号
 * @retval 0：正确，1：错误
 */
static uint8_t recv_check(MB_PARAM *port)
{
    /* 接收的数据长度大于接收缓冲区 */
    if (port->rx_byte_cnt > MB_BUF_SIZE)
    {
        return 1;
    }

    /* 不是本机地址，也不是广播地址 */
    if (port->rx_buf[0] != port->slave_id && port->rx_buf[0] != 0)
    {
        return 1;
    }

    /* CRC16校验和不符合规则 */
    if (crc16_calc(port->rx_buf, port->rx_byte_cnt) != 0)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief  数据发送，并追加2字节CRC16校验码
 * @param  port：串口号
 * @retval None
 */
static void data_send(MB_PARAM *port)
{
    uint16_t crc_code;

    crc_code = crc16_calc(port->tx_buf, port->tx_byte_cnt);
    port->tx_buf[port->tx_byte_cnt++] = crc_code >> 8;
    port->tx_buf[port->tx_byte_cnt++] = crc_code & 0xFF;
    port->mode(TX);
    port->send(port->tx_buf, port->tx_byte_cnt);
    port->mode(RX);
}

/**
 * @brief  发送错误应答
 * @param  port：串口号
 * @param  error_code: 错误代码
 * @retval None
 */
static void error_ack_send(MB_PARAM *port ,uint8_t error_code)
{
    uint16_t crc_code;

    port->tx_byte_cnt = 0;
    port->tx_buf[0] = port->rx_buf[0];
    port->tx_buf[1] = port->rx_buf[1] | 0x80;
    port->tx_buf[2] = error_code;
    crc_code = crc16_calc(port->tx_buf, 3);
    port->tx_buf[3] = crc_code >> 8;
    port->tx_buf[4] = crc_code & 0xFF;
    port->mode(TX);
    port->send(port->tx_buf, 5);
    port->mode(RX);
}

/**
 * @brief  modbus从机初始化
 * @param  port：串口号
 * @param  slave_id：从机地址
 * @param  baud：波特率
 * @retval None
 *///                    结构体命         从机ID         波特率
void mbslave_init(MB_PARAM *port, uint8_t slave_id, uint32_t baud)
{
    port->rx_byte_cnt = 0;/* 接收字节数 */
    memset(port->rx_buf, 0x00, MB_BUF_SIZE);//把0x00赋值到port->rx_buf的前MB_BUF_SIZE个字符

    port->tx_byte_cnt = 0;/* 发送字节数 */
    memset(port->tx_buf, 0x00, MB_BUF_SIZE);//把0x00赋值到port->rx_buf的前MB_BUF_SIZE个字符

    if (slave_id < MB_ADDR_MIN || slave_id > MB_ADDR_MAX)//判断地址合法性
    {
        port->slave_id = 0x01;//不合法，自动赋值为1号
    }
    else
    {
        port->slave_id = slave_id;//合法，则赋值为设定ID
    }

    port->rx_end_flg = 0; /* 接收完成标志 */
    port->rx_timer_flg = 0;/* 接收定时标志 */
    port->rx_timeout = 0;/* 接收超时时间 */

    port->init(baud);//给定工作的波特率
}

/**
 * @brief  modbus接收定时，超时值：35000000/波特率，单位us，可适当放大
 * @param  port：串口号
 * @retval None
 */
void mbslave_timer(MB_PARAM *port)
{
    /* 启动定时 */
    if (port->rx_timer_flg == 1)
    {
        if (++port->rx_timeout >= 8)
        {
            /* 停止定时 */
            port->rx_timer_flg = 0;
            /* 一帧数据接收完成标志 */
            port->rx_end_flg = 1;
        }
    }
}

/**
 * @brief  解析modbus数据包
 * @param  port：串口号
 * @retval None
 */
void mbslave_poll(MB_PARAM *port)
{
    uint8_t error_code = 0; //错误代码

    /* 没有超时 */
    if (port->rx_end_flg == 0)/* 接收完成标志 */
    {
        /* 继续接收 */
        return;
    }

    /* 接收的数据验证正确 */
    if (recv_check(port) == 0)
    {
        switch (port->rx_buf[1])
        {
            case 0x01:
                error_code = mbslave_01H(port);
                break;

            case 0x02:
                error_code = mbslave_02H(port);
                break;

            case 0x03:
                error_code = mbslave_03H(port);
                break;

            case 0x04:
                error_code = mbslave_04H(port);
                break;

            case 0x05:
                error_code = mbslave_05H(port);
                break;

            case 0x06:
                error_code = mbslave_06H(port);
                break;

            case 0x0F:
                error_code = mbslave_0FH(port);
                break;

            case 0x10:
                error_code = mbslave_10H(port);
                break;

            default:
                error_code = MB_FUN_ERR;
                break;
        }

        /* 是本机地址 */
        if (port->slave_id == port->rx_buf[0])
        {
            /* 无错误码 */
            if (error_code == MB_NOT_ERR)
            {
                /* 接收到的数据处理 */
                {
					if(port->rx_buf[1]==0x03&&port->rx_buf[2]==0x00&&port->rx_buf[3]==0x00&&port->rx_buf[4]==0x00)
					{
						 port->tx_buf[0]=port->rx_buf[0];
						 port->tx_buf[1]=0x03;
						 port->tx_buf[2]=0x02;
						 port->tx_buf[3]=0x00;
						 port->tx_buf[4] = port->rx_buf[0];
						data_send(port);				                /* 发送数据包响应主机 */
					}
					else if(port->rx_buf[1]==0x04)
					{
					  switch(port->rx_buf[2])
					  {
						  case 0x01:
						  {
							  switch(port->rx_buf[3])
							  {
								  case  0X00:					//温度第一路			  
									  {    
											port->tx_buf[1] = 0x04;
										    port->tx_buf[2] = 0x04;
											port->tx_buf[3] = buf[11];
											port->tx_buf[4] = buf[10];
										    port->tx_buf[5] = buf[9];
											port->tx_buf[6] = buf[8];
											data_send(port);									  
									  }	
									break;									  
								  case 0x01:				//P参数
									  if(port->rx_buf[4]==0x00 && port->rx_buf[5]== 0x02)
									  {
										    port->tx_buf[1] = 0x04;
											port->tx_buf[2] = 0x02;
											port->tx_buf[3] = buf[2];
											port->tx_buf[4] = buf[3];
											data_send(port);									  
									  }
									  break;
								  case 0x02:				//I参数
									    if(port->rx_buf[4]==0x00 && port->rx_buf[5]== 0x02)
									  {
										    port->tx_buf[1] = 0x04;
											port->tx_buf[2] = 0x02;
											port->tx_buf[3] = buf[4];
											port->tx_buf[4] = buf[5];
											data_send(port);									  
									  }
									  break;
								  case 0x03:					//D参数
									    if(port->rx_buf[4]==0x00 && port->rx_buf[5]== 0x02)
									  {
										    port->tx_buf[1] = 0x04;
											port->tx_buf[2] = 0x02;
											port->tx_buf[3] = buf[6];
											port->tx_buf[4] = buf[7];
											data_send(port);									  
									  }
									  break;
								  default: break;
							  
							  }
				  
						  }
							  break;
						  case 0x02:
							{
							  switch(port->rx_buf[3])
							  {		
								  case 0X00:		//温度第二路						   
									  {
										    port->tx_buf[1] = 0x04;
											port->tx_buf[2] = 0x04;
											port->tx_buf[3] = buf[21];
											port->tx_buf[4] = buf[20];
										    port->tx_buf[5] = buf[19];
											port->tx_buf[6] = buf[18];
											data_send(port);									  
									  }							
								  break;
								  case 0x01:				//P参数
									  if(port->rx_buf[4]==0x00 && port->rx_buf[5]== 0x02)
									  {
										    port->tx_buf[1] = 0x04;
											port->tx_buf[2] = 0x02;
											port->tx_buf[3] = buf[12];
											port->tx_buf[4] = buf[13];
											data_send(port);									  
									  }
									  break;
								  case 0x02:				//I参数
									    if(port->rx_buf[4]==0x00 && port->rx_buf[5]== 0x02)
									  {
										    port->tx_buf[1] = 0x04;
											port->tx_buf[2] = 0x02;
											port->tx_buf[3] = buf[14];
											port->tx_buf[4] = buf[15];
											data_send(port);									  
									  }
									  break;
								  case 0x03:					//D参数
									    if(port->rx_buf[4]==0x00 && port->rx_buf[5]== 0x02)
									  {
										    port->tx_buf[1] = 0x04;
											port->tx_buf[2] = 0x02;
											port->tx_buf[3] = buf[16];
											port->tx_buf[4] = buf[17];
											data_send(port);									  
									  }
									  break;
								  default: break;							  
							  }				  
						  }  						  
							  break;
						  default:break;					  					  
					  }					
					}
				else if(port->rx_buf[1]==0x06)
				{
					switch(port->rx_buf[2])
					{
						case 0x00:
						{
							buf1[0]=port->tx_buf[4];
							buf1[1]=port->tx_buf[5];
							ee_WriteBytes(&buf1[0], 0X0000, 2);
								data_send(port);	
						}							
					    break;
						
						case 0x01:
						{
						   switch(port->rx_buf[3])
						   {
							   case 0x01:
							   {
							        buf1[2]=port->tx_buf[4];
									buf1[3]=port->tx_buf[5];
									ee_WriteBytes(&buf1[2], 0X0002, 2);
									data_send(port);	
							   }
								   break;
							   case 0x02:
								   {
							        buf1[4]=port->tx_buf[4];
									buf1[5]=port->tx_buf[5];
									ee_WriteBytes(&buf1[4], 0X0004, 2);
									data_send(port);	
							   }
								   break;
							   case 0x03:
								   {
							        buf1[6]=port->tx_buf[4];
									buf1[7]=port->tx_buf[5];
									ee_WriteBytes(&buf1[6], 0X0006, 2);
									data_send(port);	                /* 发送数据包响应主机 */
							   }
								   break;
							   default :break;
						   
						   }
						}
							
						break;
						
						case 0x02:
								{
						   switch(port->rx_buf[3])
						   {
							   case 0x01:
								   {
							        buf1[12]=port->tx_buf[4];
									buf1[13]=port->tx_buf[5];
									ee_WriteBytes(&buf1[12], 0X000C, 2);
									data_send(port);	
							   }
								   break;
							   case 0x02:
								   {
							        buf1[14]=port->tx_buf[4];
									buf1[15]=port->tx_buf[5];
									ee_WriteBytes(&buf1[14], 0X000E, 2);
									data_send(port);	
							   }
								   break;
							   case 0x03:
								   {
							        buf1[16]=port->tx_buf[4];
									buf1[17]=port->tx_buf[5];
									ee_WriteBytes(&buf1[16], 0X0010, 2);
									data_send(port);	
							   }
								   break;

							   default :break;						   
						   }
						}							
						break;						
						default : break;					
					}				
				}
				else if(port->rx_buf[1]==0x10)	
				{
				   switch(port->rx_buf[2])
				   {
					   case  0x01:
					   {				               
									buf1[8]=port->tx_buf[7];
									buf1[9]=port->tx_buf[6];
									buf1[10]=port->tx_buf[5];
									buf1[11]=port->tx_buf[4];
									ee_WriteBytes(&buf1[8], 0X0008, 4);
									port->tx_buf[3]=0X04; 
									data_send(port);	                /* 发送数据包响应主机 */
					   
					   }
					   break;
					   case  0x02:
						   {
							        buf1[18]=port->tx_buf[7];
									buf1[19]=port->tx_buf[6];
									buf1[20]=port->tx_buf[5];
									buf1[21]=port->tx_buf[4];
									ee_WriteBytes(&buf1[18], 0X0012, 4);
							       port->tx_buf[3]=0X04;
									data_send(port);	                /* 发送数据包响应主机 */
							   
							   }
					   break;
					   default :  break;
				   
				   }
				
				}					
                }



            }
            /* 有错误码 */
            else
            {
                /* 响应错误码，功能码或寄存器地址错误 */
                error_ack_send(port, error_code);
            }
        }
        /* 是广播地址 */
        else if (port->rx_buf[0] == MB_ADDR_BROADCAST)
        {
            /* 不响应主机 */
        }
    }
    /* 接收的数据有错误 */
    else
    {
        /* 是本机地址 */
        if (port->slave_id == port->rx_buf[0])
        {
            /* 响应错误码：数据错误 */
            error_ack_send(port, MB_DATA_ERR);
        }
        /* 不是本机地址 */
        else
        {
            /* 不响应主机 */
        }
    }

    port->rx_byte_cnt = 0;
    port->rx_end_flg = 0;
}
/*****END OF FILE*****/
