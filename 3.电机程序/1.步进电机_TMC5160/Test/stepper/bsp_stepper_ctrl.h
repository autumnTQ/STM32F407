#ifndef __BSP_STEP_MOTOR_CTRL_H
#define	__BSP_STEP_MOTOR_CTRL_H

/*宏定义*/
/*******************************************************/
#define T1_FREQ           (SystemCoreClock/(TIM_PRESCALER+1)) // 频率ft值

/*电机单圈参数*/
#define STEP_ANGLE				1.8f                        //步进电机的步距角 单位：度
#define FSPR              ((float)(360.0f/STEP_ANGLE))//步进电机的一圈所需脉冲数

#define MICRO_STEP        32          				        //细分器细分数 
#define SPR               (FSPR*MICRO_STEP)           //细分后一圈所需脉冲数

#define PULSE_RATIO       ((float)(SPR/ENCODER_TOTAL_RESOLUTION))//步进电机单圈脉冲数与编码器单圈脉冲的比值
#define TARGET_DISP      2                   //步进电机运动时的目标圈数，单位：转
#define SPEED_LIMIT      10000                //运行速度限制

#define SAMPLING_PERIOD   50                          //PID采样频率，单位Hz
//#define SAMPLING_PERIOD     (unsigned int)(1000 / GET_BASIC_TIM_PERIOD())  //PID采样频率，单位Hz
/* 补充：例程默认写固定了SAMPLING_PERIOD宏频率值，如果需要试PID助手的发送周期效果，注释切换下一行宏 */

typedef struct {
  unsigned char stepper_dir : 1;               //步进电机方向
  unsigned char stepper_running : 1;           //步进电机运行状态
  unsigned char MSD_ENA : 1;                   //驱动器使能状态
}__SYS_STATUS;


void MSD_ENA(int NewState);
void Set_Stepper_Stop(void);
void Set_Stepper_Start(void);
void Set_Stepper_Dir(int dir);
void Stepper_Speed_Ctrl(void);

#endif /* __STEP_MOTOR_CTRL_H */
