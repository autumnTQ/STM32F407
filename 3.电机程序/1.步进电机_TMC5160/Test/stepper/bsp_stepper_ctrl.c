/**
 ******************************************************************************
 * @file    bsp_stepper_init.c
 * @author  fire
 * @version V1.0
 * @date    2019-xx-xx
 * @brief   ���������ʼ��
 ******************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:Ұ��  STM32 F407 ������
 * ��̳    :http://www.firebbs.cn
 * �Ա�    :http://firestm32.taobao.com
 *
 ******************************************************************************
 */
#include "bsp_stepper_ctrl.h"
#include "math.h"
#include "bsp_pid.h"
#include "protocol.h"

extern _pid pid;
extern __IO uint16_t OC_Pulse_num; // �Ƚ�����ļ���ֵ
extern uint32_t encoder_step;      // ��ȡ����������

#define PID_ASSISTANT_EN 1

__SYS_STATUS sys_status = {0};

/**
 * @brief  ����������ֹͣ
 * @param  NewState��ʹ�ܻ��߽�ֹ
 * @retval ��
 */
void MSD_ENA(int NewState)
{
    if (NewState)
    {
        // ENAʧ�ܣ���ֹ��������������ѻ�״̬����ʱ���Ϊ�ޱ�������״̬�������ֶ���ת���
//        MOTOR_EN(OFF);
        sys_status.MSD_ENA = 0;
    }
    else
    {
        // ENAʹ�ܣ���ʱ���Ϊ��������״̬
//        MOTOR_EN(ON);
        sys_status.MSD_ENA = 1;
    }
}

/**
 * @brief  �������ɲ��
 * @param  ��
 * @retval ��
 */
void Set_Stepper_Stop(void)
{
    /*ʧ�ܱȽ�ͨ��*/
    // TIM_CCxChannelCmd(MOTOR_PUL_TIM,MOTOR_PUL_CHANNEL_x,TIM_CCx_DISABLE);
    sys_status.stepper_running = 0;
}

/**
 * @brief  �����������
 * @param  ��
 * @retval ��
 */
void Set_Stepper_Start(void)
{
    /*ʹ��������*/
    MSD_ENA(ON);
    /*ʹ�ܱȽ�ͨ�����*/
    // TIM_CCxChannelCmd(MOTOR_PUL_TIM,MOTOR_PUL_CHANNEL_x,TIM_CCx_ENABLE);
    sys_status.MSD_ENA = 1;
    sys_status.stepper_running = 1;
}

/**
 * @brief  �����������
 * @param  ��
 * @retval ��
 */
void Set_Stepper_Dir(int dir)
{
    if (sys_status.stepper_dir != dir)
    {
        Set_Stepper_Stop();
    }
}

/**
 * @brief  �����������ʽPID����
 * @retval ��
 * @note   ������ʱ���ж��ڵ���
 */
void Stepper_Speed_Ctrl(void)
{
    /* ��������ر��� */
    static __IO int32_t last_count = 0;
    __IO int32_t capture_count = 0;
    __IO int32_t capture_per_unit = 0;
    /* ����pid����������ֵ */
    static __IO float cont_val = 0.0f;

    __IO float timer_delay = 0.0f;
     static int16_t target_err = 0;

    TMC5160_SPIReadInt(0x39, 0x00000000); // ��ȡ�������ļ���ֵ��ͬ����Ҫ������ �˴���Ϊѭ����
    capture_count = TMC5160_SPIReadInt(0x39, 0x00000000);
  
    encoder_step = capture_count;
    target_err = pid.target_val  - capture_count;   //Ŀ��ֵ��������ֵ
    target_err = abs(target_err);  // ���������ֵ
 
    /* ������˶�ʱ������pid���� */
   if (target_err >= 10)
   {
        /* ��һ�� --> ���㵱ǰ�������������� */
        // ��ǰ�����ܼ���ֵ = ��ǰ��ʱ���ļ���ֵ + ����ʱ����������� * ��ʱ������װ��ֵ��
        // capture_count =__HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (encoder_overflow_count * ENCODER_TIM_PERIOD);

        /* �ڶ��� --> ����ǰ����������������Ϊʵ��ֵ����pid������ */
        cont_val += PID_realize((float)capture_count);

        /* ������ --> ��PID�����������ֵ(����ֵ)�����ڲ������ */
        // �ж�PID���������ֵ�ķ���
        // cont_val > 0 ? (MOTOR_DIR(CW)) : (MOTOR_DIR(CCW));
        // ���ڱȽϼ���������������Ϊ��ֵ���Լ���ó�������ֵȡ����ֵ */
        timer_delay = fabsf(cont_val);
        /* ������������ٶ� ����ֹ��ת*/
        timer_delay >= SPEED_LIMIT ? (timer_delay = SPEED_LIMIT) : timer_delay;
        // �Ƚϼ����������� =  (�Ƚϼ�ʱ����Ƶ��/(����ֵ�ľ���ֵ * ��������������������� )) / 2
        
        TMC5160_SPIWriteInt(0x2D, cont_val); // Ŀ��λ�þ���λ��
        
        XTARGET = cont_val; 
#if PID_ASSISTANT_EN

        int Temp = capture_count;                                // ��λ����Ҫ����������ת��һ��
        set_computer_value(SEND_FACT_CMD, CURVES_CH1, &Temp, 1); // ��ͨ�� 1 ����ʵ��ֵ
#else
//        printf("ʵ��ֵ��%d��Ŀ��ֵ��%.0f\r\n", capture_per_unit, pid.target_val); // ��ӡʵ��ֵ��Ŀ��ֵ
#endif
   }
   else
   {
       /*ͣ��״̬���в�������*/
       last_count = 0;
       cont_val = 0;
       pid.actual_val = 0;
       pid.err = 0;
       pid.err_last = 0;
       pid.err_next = 0;
   }
}
