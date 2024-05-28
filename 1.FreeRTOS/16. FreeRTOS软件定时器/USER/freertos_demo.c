/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 移植实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "freertos_demo.h"
#include "usart.h"
#include "led.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1            /* 任务优先级 */
#define START_STK_SIZE 128           /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;      /* 任务句柄 */
void start_task(void *pvParameters); /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO 2            /* 任务优先级 */
#define TASK1_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task1Task_Handler; /* 任务句柄 */
void task1(void *pvParameters); /* 任务函数 */

TimerHandle_t timer1_handle;   //单次定时器句柄
TimerHandle_t timer2_handle;   //周期定时器句柄
void timer1_callback( TimerHandle_t pxTimer );  //软件单次定时器回调函数
void timer2_callback( TimerHandle_t pxTimer );  //软件周期定时器回调函数
/******************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    xTaskCreate((TaskFunction_t)start_task,          /* 任务函数 */
                (const char *)"start_task",          /* 任务名称 */
                (uint16_t)START_STK_SIZE,            /* 任务堆栈大小 */
                (void *)NULL,                        /* 传入给任务函数的参数 */
                (UBaseType_t)START_TASK_PRIO,        /* 任务优先级 */
                (TaskHandle_t *)&StartTask_Handler); /* 任务句柄 */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();                    /* 进入临界区 */
    
    //创建单次软件定时器 
    timer1_handle =  xTimerCreate( "timer1",
                                    1000,
                                    pdFALSE,    //单次定时器
                                    (void *)1,
                                    timer1_callback );
    //创建周期软件定时器 
   timer2_handle =  xTimerCreate( "timer2",
                                    1000,      //1000系统时钟节拍 1s
                                    pdTRUE,    //周期定时器
                                    (void *)1,
                                    timer2_callback );
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t)task1,
                (const char *)"task1",
                (uint16_t)TASK1_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIO,
                (TaskHandle_t *)&Task1Task_Handler);
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

// 任务1，按键扫描并且控制软件定时器
void task1(void *pvParameters)
{
    uint8_t serial_buf;

    while (1)
    {
        serial_buf = USART_RX_BUF[0]; // 读取串口接收到的数据
        USART_RX_BUF[0] = 0;
        if (serial_buf == 0x01)   //开启定时器
        {
            xTimerStart(timer1_handle, portMAX_DELAY);
            xTimerStart(timer2_handle, portMAX_DELAY);
        }
        else if (serial_buf == 0x02)  //关闭定时器
        {
            xTimerStop(timer1_handle, portMAX_DELAY);
            xTimerStop(timer2_handle, portMAX_DELAY);
        }
        vTaskDelay(10); /* 延时10ticks */
    }
}

//timer1的超时回调函数
void timer1_callback( TimerHandle_t pxTimer )
{
    static uint32_t timer = 0;
    printf("timer1的运行次数：%d\r\n", ++timer);
}

//timer2的超时回调函数
void timer2_callback( TimerHandle_t pxTimer )
{
    static uint32_t timer = 0;
    printf("timer2的运行次数：%d\r\n", ++timer);
}

