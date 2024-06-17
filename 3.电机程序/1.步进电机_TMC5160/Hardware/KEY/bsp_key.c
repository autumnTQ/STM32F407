
#include "bsp_key.h"

KEY_STATE KeyState = KEY_CHECK;
KEY_TYPE g_KeyActionFlag = NULL_KEY;
KEY_VALUE g_Key = KEY_NULL;
extern uint32_t motor_step;

extern uint32_t encoder_step; // ��ȡ�����ǰλ��
extern _pid pid;
extern int pid_status;

// ������ʼ��
void Key_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;  // �˿����ͱ�������
    GPIO_StructInit(&GPIO_InitStructure); // �� GPIO_InitStruct�е�ÿ��������ȱʡֵ����

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 485С�Ƶĳ�ʼ��  ����С��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;        // �˿ں�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // �����ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

// ����״̬��ѯ����
void Key_State()
{
    switch (g_Key)
    {
    // ok����
    case KEY_Ok_UP:
    {
        if (g_KeyActionFlag == SHORT_KEY)
        {
            g_KeyActionFlag = NULL_KEY;
            g_Key = KEY_NULL;
        }
        else if (g_KeyActionFlag == LONG_KEY)
        {
            //                       __set_FAULTMASK(1);//��ֹ���еĿ������ж�
            //                       NVIC_SystemReset();//�����λ

            g_KeyActionFlag = NULL_KEY;
            g_Key = KEY_NULL;
        }
    }
    break;
    //+�Ӱ���
    case KEY_Jia_UP:
    {
        if (g_KeyActionFlag == SHORT_KEY)
        {
//            XTARGET += sys_param_1[8]; // �������Ŀ��λ��
//            pid.target_val = XTARGET;

            pid.target_val += sys_param_1[8]; // �������Ŀ��λ��
            g_KeyActionFlag = NULL_KEY;
            g_Key = KEY_NULL;
        }
        else if (g_KeyActionFlag == LONG_KEY)
        {
            g_KeyActionFlag = NULL_KEY;
            g_Key = KEY_NULL;
        }
    }
    break;
    //-������
    case KEY_Jian_UP:
    {
        if (g_KeyActionFlag == SHORT_KEY)
        {
//            XTARGET -= sys_param_1[8]; // �������Ŀ��λ��
//            pid.target_val = XTARGET;
            
            pid.target_val -= sys_param_1[8]; // �������Ŀ��λ��
            
            g_KeyActionFlag = NULL_KEY;
            g_Key = KEY_NULL;
        }
        else if (g_KeyActionFlag == LONG_KEY)
        {

            g_KeyActionFlag = NULL_KEY;
            g_Key = KEY_NULL;
        }
    }
    break;
    default:
        break;
    }
}

/**
 * �������̰��ͳ����¼�
 * �̰���ʱ�� 10ms < T < 1 s, ������ʱ�� T >1 s
 * ���ܣ�ʹ��״̬����ʽ��ɨ�赥��������ɨ������Ϊ10ms,10ms�պ�����������
 * ״̬��ʹ��switch case���ʵ��״̬֮�����ת
 * lock���������ж��Ƿ��ǵ�һ�ν��а���ȷ��״̬
 * ���������¼���ǰִ�У��̰����¼��ͷź��ִ��
 */
void Key_Scan(void)
{
    static uint8_t TimeCnt = 0;
    static uint8_t lock = 0;

    static uint8_t key1 = 0;
    static uint8_t key2 = 0;
    static uint8_t key3 = 0;

    //(KEY_Ok && KEY_Jia && KEY_Jian)
    switch (KeyState)
    {
    // ����δ����״̬����ʱ�ж�Key��ֵ
    case KEY_CHECK:
    {
        if (!Key)
        {
            KeyState = KEY_COMFIRM; // �������KeyֵΪ0��˵��������ʼ���£�������һ��״̬
        }
        TimeCnt = 0;
        lock = 0;
    }
    break;
    // ��������״̬��
    case KEY_COMFIRM:
    {
        if (!Key) // �ٴ�ȷ���Ƿ���
        {
            // �ఴ���ж�
            if (KEY_Ok == 0)
            {
                g_Key = KEY_Ok_UP;
            }
            else if (KEY_Jia == 0)
            {
                g_Key = KEY_Jia_UP;
            }
            else if (KEY_Jian == 0)
            {
                g_Key = KEY_Jian_UP;
            }

            if (!lock) // ����ǵ�һ�ν���
            {
                lock = 1;
            }

            TimeCnt++;

            if (TimeCnt > 100) // ����ʱ��ȷ��
            {
                g_KeyActionFlag = LONG_KEY;
                TimeCnt = 0;
                lock = 0;

                KeyState = KEY_RELEASE; // ��Ҫ���밴���ͷ�ת̬
            }
        }
        else
        {
            if (lock == 1) // ���ǵ�һ�ν��� �ͷŰ�����ִ��
            {
                g_KeyActionFlag = SHORT_KEY;
                TimeCnt = 0;
                lock = 0;

                KeyState = KEY_RELEASE;
            }
            else // ȷ��Ϊ�������򷵻���һ��״̬
            {
                KeyState = KEY_CHECK; // ������һ��״̬
            }
        }
    }
    break;
    // �����ͷ�״̬
    case KEY_RELEASE:
    {
        if (Key) // ��ǰKeyֵΪ1��˵�������Ѿ��ͷţ����ؿ�ʼ״̬
        {
            KeyState = KEY_CHECK;
        }
    }
    break;
    default:
        break;
    }
}
