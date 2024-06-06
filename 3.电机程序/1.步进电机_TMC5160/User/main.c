/* 包含头文件 *****************************************************************/
#include "Config.h"
#include "IO_control.h"

/* 变量 ------这些变量将用于flash程序 程序---------*/
int16_t sys_param_1[128] = {0}; // 模块参数
long XTARGET = 0;               // 目标位置
uint8_t Function_Selection = 0; // 根据【0】的数字
uint32_t motor_step = 0;        // 读取电机当前位置
uint32_t motor_speed = 0;        // 读取电机实际速度
uint32_t encoder_step = 0;      // 读取编码器数字
float f_value = 0, f = 0;       // 用于编码器转动角度
char Display_String[20] = {0};  // 显示运行数据用的字符串4位整数显示

uint16_t baudRate_Selection = 0; // 波特率选择
uint8_t save_parm_bit = 0;       // 保存参数变量置1=保存参数
/* 功能选择变量 -------------------------------------------*/
uint8_t UART_addr = 0; // modbus_rtu通讯地址

extern _pid pid;
extern int pid_status;


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
    OLED_ShowString(0, 0, "Pos:", 16);
    OLED_ShowString(0, 2, "Enc:", 16);
    OLED_ShowString(0, 5, "Ang:", 16);
    Delay_US(500);
    SPI5012B_Init();        // 初始化编码器通信IO口
    Delay_US(500);          // 延时
    TLE5012B_Regest_Init(); // 编码器寄存器配置

    Init_program_table();                // 将需要初始化的值在，Init_Program_Table.c中初始化然后执行下面程序，将数据保持的单片机flash里面
    UART_addr = sys_param_1[1];          // modbus_rtu通讯地址
    baudRate_Selection = sys_param_1[2]; // 波特率选择
    USART1_Init(baudRate_Selection);     // 初始化串口1波特率
    b485Send(0);                         // 发送完成后将485设为接收
    TMC5160_IO_Init();                   // 初始化电机通信IO口

   TMC5160_Regest_Init(); // 电机驱动器寄存器配置

    TMC5160_Regest_Init_Test();
    Delay_MS(100);         // 延时
    TMC5160_ENCMODE_Set(); // 电机驱动编码器设置
    
    
    TIM2_Init();  //pid算法循环周期
    PID_param_init(); //PID算法参数初始化
    pid.target_val = 0;
 
    while (1)
    {
        switch (Function_Selection)
        {
        case 0:
        {
            TMC5160_xi_fen_set();        // 设置细分  在接收数据修改时，这里需要放到while 循环中
            TMC5160_current_set();       // 设置电流
            TMC5160_JiaSu_Set();         // T型加速设置 设置运行速度、加速度、减速度
            TMC5160_DRV_STATUS_Return(); // 获取状态
            
            TMC5160_XTARGET_Set();       // 运行到目标位置设置

            TMC5160_SPIReadInt(0x22, 0x00000000);              
            motor_speed = TMC5160_SPIReadInt(0x22, 0x00000000); // 读取实际电机速度

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
            TMC5160_SPIReadInt(0x21, 0x00000000);              // 读取实际电机位置。  同样需要读二次 此处因为循环中
            motor_step = TMC5160_SPIReadInt(0x21, 0x00000000); // 读取实际电机位置。  同样需要读二次 此处因为循环中

            TMC5160_SPIReadInt(0x39, 0x00000000);                /// 4.0; // PAGE40:X_ENC（RW）实际编码器位置：编码器因子51.2*4倍频=204.8，为了补
            encoder_step = TMC5160_SPIReadInt(0x39, 0x00000000); /// 4.0; // PAGE40:X_ENC（RW）实际编码器位置：编码器因子51.2*4倍频=204.8，为了补

            OLED_ShowNum(40, 0, motor_step, 10, 16);
            OLED_ShowNum(40, 2, encoder_step, 10, 16);

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
            OLED_ShowString(70, 5, &Display_String[0], 16);

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
