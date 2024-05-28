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

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO 3            /* 任务优先级 */
#define TASK2_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task2Task_Handler; /* 任务句柄 */
void task2(void *pvParameters); /* 任务函数 */
/******************************************************************************************************/
#define EVENTBIT_0 0X01               // 事件标志位
#define EVENTBIT_1 0X02

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
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t)task1,
                (const char *)"task1",
                (uint16_t)TASK1_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIO,
                (TaskHandle_t *)&Task1Task_Handler);
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t)task2,
                (const char *)"task2",
                (uint16_t)TASK2_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK2_PRIO,
                (TaskHandle_t *)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

// 任务1，发送任务通知值
void task1(void *pvParameters)
{
    uint8_t serial_buf;

    while (1)
    {
        serial_buf = USART_RX_BUF[0]; // 读取串口接收到的数据
        USART_RX_BUF[0] = 0;
        if (serial_buf == 0x01)
        {
             printf("将bit0位置1\r\n");
            //发送任务通知值给任务2，把bit0置1， 参数3为任务通知方式（更新指定bit）
            xTaskNotify(Task2Task_Handler, EVENTBIT_0, eSetBits);
        }
        else if (serial_buf == 0x02)
        {
             printf("将bit1位置1\r\n");
            //发送任务通知值给任务2，把bit1置1， 参数3为任务通知方式（更新指定bit）
            xTaskNotify(Task2Task_Handler, EVENTBIT_1, eSetBits);
        }
        vTaskDelay(10); /* 延时1000ticks */
    }
}

// 任务2，接收任务通知值
void task2(void *pvParameters)
{
    uint32_t notify_val = 0;
    uint32_t event_bit = 0;
    while (1)
    {
        //参数1：进去之前是否清除原先的值（0为不清除）
        //参数2：退出之后清除所有的位
        xTaskNotifyWait( 0, 0xFFFFFFFF, &notify_val, portMAX_DELAY );
        if(notify_val & EVENTBIT_0)  //如果bit0位已经被置1了
        {
            event_bit |= EVENTBIT_0;
        }
        if(notify_val & EVENTBIT_1)  //如果bit1位也被置1了
        {
            event_bit |= EVENTBIT_1;
        }
        if(event_bit == (EVENTBIT_0 | EVENTBIT_1))  //如果bit0和bit1都被置1了
        {
            printf("任务通知模拟事件标志组接收成功！！\r\n");
            event_bit = 0;  //清零
        }
    }
}
