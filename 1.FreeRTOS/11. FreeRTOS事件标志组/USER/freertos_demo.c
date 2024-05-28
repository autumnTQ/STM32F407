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
EventGroupHandle_t eventgroup_handle; // 事件标志组的句柄
#define EVENTBIT_0 0X01               // 事件
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
    eventgroup_handle = xEventGroupCreate(); // 创建事件标志组
    if (eventgroup_handle != NULL)
    {
        printf("事件标志组创建成功\r\n");
    }

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

// 任务1，
void task1(void *pvParameters)
{
    uint8_t serial_buf;
    EventBits_t event_sync = 0;

    while (1)
    {
        serial_buf = USART_RX_BUF[0]; // 读取串口接收到
        USART_RX_BUF[0] = 0;
        if (serial_buf == 0x01)
        {
            xEventGroupSetBits(eventgroup_handle, EVENTBIT_0); // 设置事件标志组的bit0置1
            
//            event_sync = xEventGroupSync(eventgroup_handle, EVENTBIT_1, EVENTBIT_0, portMAX_DELAY); //等待bit0置1，然后将bit1置1
//            printf("sync同步等待的事件标志位值为：%#x\r\n", event_sync);
        }
        else if (serial_buf == 0x02)
        {
            xEventGroupSetBits(eventgroup_handle, EVENTBIT_1); // 设置事件标志组的bit1置1
            event_sync = xEventGroupSync(eventgroup_handle, EVENTBIT_0, EVENTBIT_1, portMAX_DELAY); //等待bit0置1，然后将bit1置1
            printf("sync同步等待的事件标志位值为：%#x\r\n", event_sync);
        }
        vTaskDelay(10); /* 延时1000ticks */
    }
}

// 任务2，获取队列集的消息
void task2(void *pvParameters)
{
    EventBits_t event_bit = 0; // 等待事件标志位函数的返回值
    while (1)
    {
        event_bit = xEventGroupWaitBits(eventgroup_handle,       // 事件标志组句柄
                                        EVENTBIT_0 | EVENTBIT_1, // 等待的事件标志组的bit0和bit1位
                                        pdTRUE,                  // 成功等待到事件标志位后，清除事件
                                        pdTRUE,                  // 是否等待所有的事件标志位都置1
                                        portMAX_DELAY);
        printf("等待的事件标志位值为：%#x\r\n", event_bit);
    }
}
