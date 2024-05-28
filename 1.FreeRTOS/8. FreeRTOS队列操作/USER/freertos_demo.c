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

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO      2                   /* 任务优先级 */
#define TASK1_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
void task1(void *pvParameters);             /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO      2                   /* 任务优先级 */
#define TASK2_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO      2                   /* 任务优先级 */
#define TASK3_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task3Task_Handler;  /* 任务句柄 */
void task3(void *pvParameters);             /* 任务函数 */
/******************************************************************************************************/

QueueHandle_t xiao_queue;    //小队列
QueueHandle_t da_queue;    //大队列

char buff[100] = {"大的数组的队列"};
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    //队列的创建
    xiao_queue = xQueueCreate(2, sizeof(uint8_t));
    if(xiao_queue != NULL)
    {
        printf("xiao_queue队列创建成功\r\n");
    }
    
    da_queue = xQueueCreate(1, sizeof(char *));
    if(da_queue != NULL)
    {
        printf("da_queue队列创建成功\r\n");
    }
    
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
    vTaskStartScheduler();
}


/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到) 
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
     
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    /* 创建任务3 */
    xTaskCreate((TaskFunction_t )task3,
                (const char*    )"task3",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler);
                
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

// 任务1task1 实现入队
void task1(void *pvParameters)
{
    uint8_t serial_buf;
    char *buf;
    BaseType_t err = 0;
    
    buf = buff;
    
    while(1)
    {
        serial_buf = USART_RX_BUF[0];    //读取串口接收到
        USART_RX_BUF[0] = 0;
        if(serial_buf == 0x11)
        {
            err = xQueueSend( xiao_queue, &serial_buf, portMAX_DELAY );
            if(err != pdTRUE)
            {
                printf("xiao_queue队列发送失败\r\n");
            }
        }
        
        if(serial_buf == 0x22)
        {
            err = xQueueSend( da_queue, &buf, portMAX_DELAY);
            if(err != pdTRUE)
            {
                printf("da_queue队列发送失败\r\n");
            }
        }
       
        vTaskDelay(10);    /* 延时1000ticks */
    }
}

// 任务2task2 小数据出队列
void task2(void *pvParameters)
{
    uint8_t serial_buf;
    BaseType_t err;
    
    while(1)
    {
        err = xQueueReceive( xiao_queue, &serial_buf, portMAX_DELAY);
        if(err != pdTRUE)
        {
            printf("xiao_queue队列读取失败\r\n");
        }
        else
        {
            printf("%c", serial_buf); 
        }
    }
}

// 任务3task3  大数据出列
void task3(void *pvParameters)
{
    char * buf;
    BaseType_t err;
    
    while(1)
    {
        err = xQueueReceive( da_queue, &buf, portMAX_DELAY);
        if(err != pdTRUE)
        {
            printf("da_queue队列读取失败\r\n");
        }
        else
        {
            printf("%s", buf);
        }
     }
}

