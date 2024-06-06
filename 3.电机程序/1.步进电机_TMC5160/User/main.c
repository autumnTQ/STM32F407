/* ����ͷ�ļ� *****************************************************************/
#include "Config.h"
#include "IO_control.h"

/* ���� ------��Щ����������flash���� ����---------*/
int16_t sys_param_1[128] = {0}; // ģ�����
long XTARGET = 0;               // Ŀ��λ��
uint8_t Function_Selection = 0; // ���ݡ�0��������
uint32_t motor_step = 0;        // ��ȡ�����ǰλ��
uint32_t motor_speed = 0;        // ��ȡ���ʵ���ٶ�
uint32_t encoder_step = 0;      // ��ȡ����������
float f_value = 0, f = 0;       // ���ڱ�����ת���Ƕ�
char Display_String[20] = {0};  // ��ʾ���������õ��ַ���4λ������ʾ

uint16_t baudRate_Selection = 0; // ������ѡ��
uint8_t save_parm_bit = 0;       // �������������1=�������
/* ����ѡ����� -------------------------------------------*/
uint8_t UART_addr = 0; // modbus_rtuͨѶ��ַ

extern _pid pid;
extern int pid_status;


/**
 * @brief  ������
 * @param  ��
 * @retval ��
 */
int main(void)
{
    Key_Init();
    TIM3_Init();
    NVIC_cfg();
    GPIO_cfg();

    OLED_Init();  // ��ʾ��ʼ��
    OLED_Clear(); // ��ʾ����
    Delay_MS(100);
    OLED_ShowString(0, 0, "Pos:", 16);
    OLED_ShowString(0, 2, "Enc:", 16);
    OLED_ShowString(0, 5, "Ang:", 16);
    Delay_US(500);
    SPI5012B_Init();        // ��ʼ��������ͨ��IO��
    Delay_US(500);          // ��ʱ
    TLE5012B_Regest_Init(); // �������Ĵ�������

    Init_program_table();                // ����Ҫ��ʼ����ֵ�ڣ�Init_Program_Table.c�г�ʼ��Ȼ��ִ��������򣬽����ݱ��ֵĵ�Ƭ��flash����
    UART_addr = sys_param_1[1];          // modbus_rtuͨѶ��ַ
    baudRate_Selection = sys_param_1[2]; // ������ѡ��
    USART1_Init(baudRate_Selection);     // ��ʼ������1������
    b485Send(0);                         // ������ɺ�485��Ϊ����
    TMC5160_IO_Init();                   // ��ʼ�����ͨ��IO��

   TMC5160_Regest_Init(); // ����������Ĵ�������

    TMC5160_Regest_Init_Test();
    Delay_MS(100);         // ��ʱ
    TMC5160_ENCMODE_Set(); // �����������������
    
    
    TIM2_Init();  //pid�㷨ѭ������
    PID_param_init(); //PID�㷨������ʼ��
    pid.target_val = 0;
 
    while (1)
    {
        switch (Function_Selection)
        {
        case 0:
        {
            TMC5160_xi_fen_set();        // ����ϸ��  �ڽ��������޸�ʱ��������Ҫ�ŵ�while ѭ����
            TMC5160_current_set();       // ���õ���
            TMC5160_JiaSu_Set();         // T�ͼ������� ���������ٶȡ����ٶȡ����ٶ�
            TMC5160_DRV_STATUS_Return(); // ��ȡ״̬
            
            TMC5160_XTARGET_Set();       // ���е�Ŀ��λ������

            TMC5160_SPIReadInt(0x22, 0x00000000);              
            motor_speed = TMC5160_SPIReadInt(0x22, 0x00000000); // ��ȡʵ�ʵ���ٶ�

            Function_Selection++;
        }
        break;
        case 1:
        {                // ��������
            Key_State(); // ����״̬�ж�
            Function_Selection++;
        }
        break;
        case 2:
        {
            TMC5160_SPIReadInt(0x21, 0x00000000);              // ��ȡʵ�ʵ��λ�á�  ͬ����Ҫ������ �˴���Ϊѭ����
            motor_step = TMC5160_SPIReadInt(0x21, 0x00000000); // ��ȡʵ�ʵ��λ�á�  ͬ����Ҫ������ �˴���Ϊѭ����

            TMC5160_SPIReadInt(0x39, 0x00000000);                /// 4.0; // PAGE40:X_ENC��RW��ʵ�ʱ�����λ�ã�����������51.2*4��Ƶ=204.8��Ϊ�˲�
            encoder_step = TMC5160_SPIReadInt(0x39, 0x00000000); /// 4.0; // PAGE40:X_ENC��RW��ʵ�ʱ�����λ�ã�����������51.2*4��Ƶ=204.8��Ϊ�˲�

            OLED_ShowNum(40, 0, motor_step, 10, 16);
            OLED_ShowNum(40, 2, encoder_step, 10, 16);

            Function_Selection++;
        }
        break;
        case 3:
        {
            ReadAngle();                            // �����Ƕ�
            sys_param_1[25] = TLE5012B_Aangle / 10; // ��1λС��

            f = sys_param_1[25]; // �����Ƕ�;
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

        if (UARTU1_RX_bit == 1) // ���ڽ������ݲ�ѯ��־λ
        {
            checkComm0Modbus(); // ��ѯUART����
        }
    }
}

/*********************************************END OF FILE**********************/
