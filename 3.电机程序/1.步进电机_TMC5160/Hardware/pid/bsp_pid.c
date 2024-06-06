#include "bsp_pid.h"
#include "math.h"
// #include "./stepper/bsp_stepper_ctrl.h"
// #include "./protocol/protocol.h"

/* ����ȫ�ֱ��� */
_pid pid;
float set_point = 0.0;
int pid_status = 0;

extern uint32_t motor_speed;        // ��ȡ���ʵ���ٶ�
extern uint32_t motor_step;        // ��ȡ�����ǰλ��
extern uint32_t encoder_step;      // ��ȡ����������
extern long XTARGET;               // Ŀ��λ��



/**
 * @brief  PID������ʼ��
 *	@note 	��
 * @retval ��
 */
void PID_param_init()
{
  /* ��ʼ������ */
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
 * @brief  ����Ŀ��ֵ
 * @param  val		Ŀ��ֵ
 *	@note 	��
 * @retval ��
 */
void set_pid_actual(float temp_val)
{
  pid.target_val = temp_val; // ���õ�ǰ��Ŀ��ֵ
}

/**
 * @brief  ��ȡĿ��ֵ
 * @param  ��
 *	@note 	��
 * @retval Ŀ��ֵ
 */
float get_pid_actual(void)
{
  return pid.target_val; // ���õ�ǰ��Ŀ��ֵ
}

/**
 * @brief  ���ñ��������֡�΢��ϵ��
 * @param  p������ϵ�� P
 * @param  i������ϵ�� i
 * @param  d��΢��ϵ�� d
 *	@note 	��
 * @retval ��
 */
void set_p_i_d(float p, float i, float d)
{
  pid.Kp = p; // ���ñ���ϵ�� P
  pid.Ki = i; // ���û���ϵ�� I
  pid.Kd = d; // ����΢��ϵ�� D
}

/**
 * @brief  λ��ʽPID�㷨ʵ��
 * @param  actual_val����ǰʵ��ֵ
 *	@note 	��
 * @retval ͨ��PID���������
 */
float PID_realize(float actual_val)
{
  /*����ʵ��ֵ*/
  pid.actual_val = actual_val;
  /*����Ŀ��ֵ��ʵ��ֵ�����*/
  pid.err = pid.target_val - pid.actual_val;

  /*����ۻ�*/
  pid.integral += pid.err;
  /*PID�㷨ʵ��*/
  pid.actual_val = pid.Kp * pid.err + pid.Ki * pid.integral + pid.Kd * (pid.err - pid.err_last);
  /*����*/
  pid.err_last = pid.err;
  /*PID�㷨ʵ�֣������ؼ���ֵ*/
  return pid.actual_val;
}

/**
 * @brief  �������λ��ʽPID����
 * @retval ��
 * @note   ������ʱ���ж��ڵ���
 */
void Stepper_Speed_Ctrl(void)
{

  /* ��������ر��� */
  __IO int32_t capture_per_unit = 0;
  __IO int32_t capture_count = 0;
  static __IO int32_t last_count = 0;

  /* ����pid����������ֵ */
  __IO int32_t cont_val = 0;

  /* ������˶�ʱ������pid���� */
  if (XTARGET != encoder_step)
  {
    /* ��һ�� --> ���㵱ǰ�������������� */
    // ��ǰ�����ܼ���ֵ = ��ǰ��ʱ���ļ���ֵ + ����ʱ����������� * ��ʱ������װ��ֵ��
    TMC5160_SPIReadInt(0x39, 0x00000000); // 4.0; // PAGE40:X_ENC��RW��ʵ�ʱ�����λ�ã�����������51.2*4��Ƶ=204.8��Ϊ�˲�
    capture_count = TMC5160_SPIReadInt(0x39, 0x00000000);

    /* �ڶ��� --> ����ǰ����������������Ϊʵ��ֵ����pid������ */
    cont_val = PID_realize((float)capture_count);

    pid.target_val = cont_val;

    // /* ������ --> ��PID�����������ֵ(����ֵ)�����ڲ������ */
    // // �ж�PID���������ֵ�ķ���
    // cont_val > 0 ? (MOTOR_DIR(CW)) : (MOTOR_DIR(CCW));
    // // ���ڱȽϼ���������������Ϊ��ֵ���Լ���ó�������ֵȡ����ֵ */
    // cont_val = abs(cont_val);
    // /* ������������ٶ� ����ֹ��ת*/
    // cont_val >= SPEED_LIMIT ? (cont_val = SPEED_LIMIT) : cont_val;
    // // �Ƚϼ����������� =  (�Ƚϼ�ʱ����Ƶ��/(����ֵ�ľ���ֵ * ��������������������� )) / 2
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
