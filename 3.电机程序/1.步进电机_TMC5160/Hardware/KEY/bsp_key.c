
#include "bsp_key.h"

KEY_STATE KeyState = KEY_CHECK;
KEY_TYPE g_KeyActionFlag = NULL_KEY;
KEY_VALUE g_Key = KEY_NULL;
extern uint32_t motor_step;

extern uint32_t encoder_step; // 读取电机当前位置
extern _pid pid;
extern int pid_status;

// 按键初始化
void Key_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;  // 端口类型变量声明
    GPIO_StructInit(&GPIO_InitStructure); // 把 GPIO_InitStruct中的每个参数按缺省值填入

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 485小灯的初始化  点亮小灯
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;        // 端口号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 传送速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

// 按键状态查询函数
void Key_State()
{
    switch (g_Key)
    {
    // ok按键
    case KEY_Ok_UP:
    {
        if (g_KeyActionFlag == SHORT_KEY)
        {
            g_KeyActionFlag = NULL_KEY;
            g_Key = KEY_NULL;
        }
        else if (g_KeyActionFlag == LONG_KEY)
        {
            //                       __set_FAULTMASK(1);//禁止所有的可屏蔽中断
            //                       NVIC_SystemReset();//软件复位

            g_KeyActionFlag = NULL_KEY;
            g_Key = KEY_NULL;
        }
    }
    break;
    //+加按键
    case KEY_Jia_UP:
    {
        if (g_KeyActionFlag == SHORT_KEY)
        {
//            XTARGET += sys_param_1[8]; // 电机运行目标位置
//            pid.target_val = XTARGET;

            pid.target_val += sys_param_1[8]; // 电机运行目标位置
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
    //-减按键
    case KEY_Jian_UP:
    {
        if (g_KeyActionFlag == SHORT_KEY)
        {
//            XTARGET -= sys_param_1[8]; // 电机运行目标位置
//            pid.target_val = XTARGET;
            
            pid.target_val -= sys_param_1[8]; // 电机运行目标位置
            
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
 * 按键检测短按和长按事件
 * 短按：时间 10ms < T < 1 s, 长按：时间 T >1 s
 * 功能：使用状态机方式，扫描单个按键；扫描周期为10ms,10ms刚好跳过抖动；
 * 状态机使用switch case语句实现状态之间的跳转
 * lock变量用于判断是否是第一次进行按键确认状态
 * ：长按键事件提前执行，短按键事件释放后才执行
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
    // 按键未按下状态，此时判断Key的值
    case KEY_CHECK:
    {
        if (!Key)
        {
            KeyState = KEY_COMFIRM; // 如果按键Key值为0，说明按键开始按下，进入下一个状态
        }
        TimeCnt = 0;
        lock = 0;
    }
    break;
    // 按键按下状态：
    case KEY_COMFIRM:
    {
        if (!Key) // 再次确认是否按下
        {
            // 多按键判断
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

            if (!lock) // 如果是第一次进入
            {
                lock = 1;
            }

            TimeCnt++;

            if (TimeCnt > 100) // 按键时长确认
            {
                g_KeyActionFlag = LONG_KEY;
                TimeCnt = 0;
                lock = 0;

                KeyState = KEY_RELEASE; // 需要进入按键释放转态
            }
        }
        else
        {
            if (lock == 1) // 不是第一次进入 释放按键才执行
            {
                g_KeyActionFlag = SHORT_KEY;
                TimeCnt = 0;
                lock = 0;

                KeyState = KEY_RELEASE;
            }
            else // 确认为抖动，则返回上一个状态
            {
                KeyState = KEY_CHECK; // 返回上一个状态
            }
        }
    }
    break;
    // 按键释放状态
    case KEY_RELEASE:
    {
        if (Key) // 当前Key值为1，说明按键已经释放，返回开始状态
        {
            KeyState = KEY_CHECK;
        }
    }
    break;
    default:
        break;
    }
}
