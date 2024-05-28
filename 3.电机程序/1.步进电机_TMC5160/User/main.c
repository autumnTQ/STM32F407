/* 包含头文件 *****************************************************************/
#include "Config.h"
#include "IO_control.h"

/* 变量 ------这些变量将用于flash程序 程序---------*/
int16_t sys_param_1[128] = {0}; // 模块参数
long XTARGET = 0;
uint8_t Function_Selection = 0; // 根据【0】的数字
uint32_t motor_step = 0;        // 记录点击运行位置
float f_value = 0, f = 0;       // 用于编码器转动角度
char Display_String[20] = {0};  // 显示运行数据用的字符串4位整数显示

uint16_t baudRate_Selection = 0; // 波特率选择
uint8_t save_parm_bit = 0;       // 保存参数变量置1=保存参数
/* 功能选择变量 -------------------------------------------*/
uint8_t UART_addr = 0; // modbus_rtu通讯地址

/**
 * @brief  主函数
 * @param  无
 * @retval 无
 */
int main(void)
{
    Key_Init();
    TIM3_Init();
    NVIC_cfg();
    GPIO_cfg();

    OLED_Init();  // 显示初始化
    OLED_Clear(); // 显示清屏
    Delay_MS(100);
    OLED_ShowString(0, 0, "Position:", 16);
    OLED_ShowString(0, 5, "Angle:          ", 16);
    Delay_MS(100);
    SPI5012B_Init();

    // 将需要初始化的值在，Init_Program_Table.c中初始化然后执行下面程序，将数据保持的单片机flash里面
    Init_program_table();                // 初始化配置参数
    UART_addr = sys_param_1[1];          // modbus_rtu通讯地址
    baudRate_Selection = sys_param_1[2]; // 波特率选择
    USART1_Init(baudRate_Selection);     // 初始化串口1波特率
    b485Send(0);                         // 发送完成后将485设为接收

    TMC5160_IO_Init();
    TMC5160_Regest_Init();
    Delay_MS(100);
    TMC5160_ENCMODE_Set(); // 编码器设置

    TMC5160_xi_fen_set();  // 设置细分  在接收数据修改时，这里需要放到while 循环中
    TMC5160_current_set(); // 设置电流
    TMC5160_JiaSu_Set();   // T型加速设置 设置运行速度、加速度、减速度 

    TMC5160_DRV_STATUS_Return(); // 获取状态

    while (1)
    {
        switch (Function_Selection)
        {
        case 0:
        {                          // 控制
            TMC5160_XTARGET_Set(); // 运行到目标位置设置
            Function_Selection++;
        }
        break;
        case 1:
        {                // 按键处理
            Key_State(); // 按键状态判断
            Function_Selection++;
        }
        break;
        case 2:
        {
            motor_step = TMC5160_SPIReadInt(0x21, 0x00000000); // 读取当前位置

            OLED_ShowNum(30, 2, motor_step, 10, 16);
            Function_Selection++;
        }
        break;
        case 3:
        {
            ReadAngle();                            // 测量角度
            sys_param_1[25] = TLE5012B_Aangle / 10; // 带1位小数

            f = sys_param_1[25]; // 测量角度;
            f /= 10;
            f_value = f;
            sprintf(Display_String, "%6.1f", f_value);
            OLED_ShowString(50, 5, &Display_String[0], 16);

            Function_Selection = 0;
        }
        break;
        default:
            break;
        }

        if (UARTU1_RX_bit == 1) // 串口接收数据查询标志位
        {
            checkComm0Modbus(); // 查询UART数据
        }
    }
}

/*********************************************END OF FILE**********************/
