#include "bsp_pid.h"
#include "math.h"
// #include "./stepper/bsp_stepper_ctrl.h"
// #include "./protocol/protocol.h"

/* 定义全局变量 */
_pid pid;
float set_point = 0.0;
int pid_status = 0;

extern uint32_t motor_speed;        // 读取电机实际速度
extern uint32_t motor_step;        // 读取电机当前位置
extern uint32_t encoder_step;      // 读取编码器数字
extern long XTARGET;               // 目标位置



/**
 * @brief  PID参数初始化
 *	@note 	无
 * @retval 无
 */
void PID_param_init()
{
  /* 初始化参数 */
  pid.target_val = 0.0;
  pid.actual_val = 0.0;
  pid.err = 0.0;
  pid.err_last = 0.0;
  pid.integral = 0.0;
  pid.Kp = 0.2;
  pid.Ki = 0;
  pid.Kd = 0.1;
}

/**
 * @brief  设置目标值
 * @param  val		目标值
 *	@note 	无
 * @retval 无
 */
void set_pid_actual(float temp_val)
{
  pid.target_val = temp_val; // 设置当前的目标值
}

/**
 * @brief  获取目标值
 * @param  无
 *	@note 	无
 * @retval 目标值
 */
float get_pid_actual(void)
{
  return pid.target_val; // 设置当前的目标值
}

/**
 * @brief  设置比例、积分、微分系数
 * @param  p：比例系数 P
 * @param  i：积分系数 i
 * @param  d：微分系数 d
 *	@note 	无
 * @retval 无
 */
void set_p_i_d(float p, float i, float d)
{
  pid.Kp = p; // 设置比例系数 P
  pid.Ki = i; // 设置积分系数 I
  pid.Kd = d; // 设置微分系数 D
}

/**
 * @brief  位置式PID算法实现
 * @param  actual_val：当前实际值
 *	@note 	无
 * @retval 通过PID计算后的输出
 */
float PID_realize(float actual_val)
{
  /*传入实际值*/
  pid.actual_val = actual_val;
  /*计算目标值与实际值的误差*/
  pid.err = pid.target_val - pid.actual_val;

  /*误差累积*/
  pid.integral += pid.err;
  /*PID算法实现*/
  pid.actual_val = pid.Kp * pid.err + pid.Ki * pid.integral + pid.Kd * (pid.err - pid.err_last);
  /*误差传递*/
  pid.err_last = pid.err;
  /*PID算法实现，并返回计算值*/
  return pid.actual_val;
}

/**
 * @brief  步进电机位置式PID控制
 * @retval 无
 * @note   基本定时器中断内调用
 */
void Stepper_Speed_Ctrl(void)
{

  /* 编码器相关变量 */
  __IO int32_t capture_per_unit = 0;
  __IO int32_t capture_count = 0;
  static __IO int32_t last_count = 0;

  /* 经过pid计算后的期望值 */
  __IO int32_t cont_val = 0;

  /* 当电机运动时才启动pid计算 */
  if (XTARGET != encoder_step)
  {
    /* 第一步 --> 计算当前编码器脉冲总数 */
    // 当前脉冲总计数值 = 当前定时器的计数值 + （定时器的溢出次数 * 定时器的重装载值）
    TMC5160_SPIReadInt(0x39, 0x00000000); // 4.0; // PAGE40:X_ENC（RW）实际编码器位置：编码器因子51.2*4倍频=204.8，为了补
    capture_count = TMC5160_SPIReadInt(0x39, 0x00000000);

    /* 第二步 --> 将当前编码器脉冲总数作为实际值传入pid控制器 */
    cont_val = PID_realize((float)capture_count);

    pid.target_val = cont_val;

    // /* 第三步 --> 用PID控制器的输出值(期望值)来调节步进电机 */
    // // 判断PID控制器输出值的方向
    // cont_val > 0 ? (MOTOR_DIR(CW)) : (MOTOR_DIR(CCW));
    // // 由于比较计数器的增量不能为负值，对计算得出的期望值取绝对值 */
    // cont_val = abs(cont_val);
    // /* 限制最大启动速度 ，防止堵转*/
    // cont_val >= SPEED_LIMIT ? (cont_val = SPEED_LIMIT) : cont_val;
    // // 比较计数器的增量 =  (比较计时器的频率/(期望值的绝对值 * 步进电机与编码器的脉冲比 )) / 2
    // OC_Pulse_num = ((uint16_t)(T1_FREQ / ((float)cont_val * PULSE_RATIO))) >> 1;
 }
 else
 {
   capture_per_unit = 0;
   cont_val = 0;
   pid.actual_val = 0;
   pid.err = 0;
   pid.err_last = 0;
   pid.integral = 0;
 }
}
