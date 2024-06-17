/* 包含头文件 *****************************************************************/
#include "Config.h"
/*******************************************************************************
 * @函数名称
 * @函数说明   主函数
 * @输入参数   无
 * @输出参数   无
 * @返回参数   无
 *****************************************************************************/
void Init_program_table(void) // 初始化配置参数
{
    // 系统产生
    sys_param_1[0] = 0;    // 备用
    sys_param_1[1] = 1;    // 通讯地址 0-255
    sys_param_1[2] = 9600; // 通讯波特率  0：4800，1：9600，2:19200，3：115200
    sys_param_1[3] = 1;    // 驱动细分
    sys_param_1[4] = 18;   // 电机转角 18=1.8°
    sys_param_1[5] = 500;  // 运行速度
    sys_param_1[6] = 200;  // 加速度
    sys_param_1[7] = 200;  // 减速度
    sys_param_1[8] = 200;  // 按键点动脉冲数
    sys_param_1[9] = 6;    // 电机运行电流
    sys_param_1[10] = 2;   // 静止状态下电机电流
    sys_param_1[11] = 0;   //
    sys_param_1[12] = 0;   //
    sys_param_1[13] = 0;   //
    sys_param_1[14] = 0;   //
    sys_param_1[15] = 0;   //
    sys_param_1[16] = 0;   //
    sys_param_1[17] = 0;   //
    sys_param_1[18] = 0;   //
    sys_param_1[19] = 0;   //
    // 地址20-39数据调用保存程序
    sys_param_1[20] = 0; //
    sys_param_1[21] = 0; //
    sys_param_1[22] = 0; //
    sys_param_1[23] = 0; //
    sys_param_1[24] = 0; //
    sys_param_1[25] = 0; //
    sys_param_1[26] = 0; //
    sys_param_1[27] = 0; //
    sys_param_1[28] = 0; //
    sys_param_1[29] = 0; //

    sys_param_1[30] = 0; //
    sys_param_1[31] = 0; //
    sys_param_1[32] = 0; //
    sys_param_1[33] = 0; //
    sys_param_1[34] = 0; //
    sys_param_1[35] = 0; //
    sys_param_1[36] = 0; //
    sys_param_1[37] = 0; //
    sys_param_1[38] = 0; //
    sys_param_1[39] = 0; //

    sys_param_1[40] = 0; //
    sys_param_1[41] = 0; //
    sys_param_1[42] = 0; //
    sys_param_1[43] = 0; //
    sys_param_1[44] = 0; //
    sys_param_1[45] = 0; //
    sys_param_1[46] = 0; //
    sys_param_1[47] = 0; //
    sys_param_1[48] = 0; //
    sys_param_1[49] = 0; //

    sys_param_1[100] = 0; // INTPOL
    sys_param_1[101] = 0; // DEDGE
    sys_param_1[102] = 0; // MRES3-0

    sys_param_1[103] = 0; // RBL1-0
    sys_param_1[104] = 0; // CHM
    sys_param_1[105] = 0; // RNDTF
    sys_param_1[106] = 0; // HDEC1-0
    sys_param_1[107] = 0; // HEND3-0
    sys_param_1[108] = 0; // HSTRT2-0
    sys_param_1[109] = 0; // TOFF3-0

    sys_param_1[110] = 0; // SEIMIN
    sys_param_1[111] = 0; // SEDN1-0
    sys_param_1[112] = 0; // SEMAX3-0
    sys_param_1[113] = 0; // SEUP1-0
    sys_param_1[114] = 0; // SEMIN3-0

    sys_param_1[115] = 0; // SFILT
    sys_param_1[116] = 0; // SGT6-0
    sys_param_1[117] = 0; // CS4-0

    sys_param_1[118] = 0; // TST
    sys_param_1[119] = 0; // SLPH1-0
    sys_param_1[120] = 0; // SLPL1-0
    sys_param_1[121] = 0; // DISS2G
    sys_param_1[122] = 0; // TS2G1-0
    sys_param_1[123] = 0; // SDOFF
    sys_param_1[124] = 0; // VSENSE
    sys_param_1[125] = 0; // RDSEL1-0
}
