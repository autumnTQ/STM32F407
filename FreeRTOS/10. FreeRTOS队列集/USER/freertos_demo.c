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
#define TASK2_PRIO      3                   /* 任务优先级 */
#define TASK2_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */
/******************************************************************************************************/

QueueSetHandle_t  queueset_handle;   //队列集句柄
QueueHandle_t     queue_handle;    //队列句柄
QueueHandle_t     semphr_handle;     //二值信号量句柄
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    queueset_handle = xQueueCreateSet( 2 );   // 创建队列集
    if(queueset_handle != NULL)
    {
        printf("队列集queueset_handle创建成功\r\n");
    }    
    queue_handle = xQueueCreate( 1, sizeof(uint8_t) );   // 创建队列
    semphr_handle = xSemaphoreCreateBinary();            // 创建二值信号量
    
    xQueueAddToSet( queue_handle, queueset_handle );
    xQueueAddToSet( semphr_handle, queueset_handle );
    
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
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

// 任务1，实现队列发送以及信号量释放
void task1(void *pvParameters)
{
    uint8_t serial_buf;    
    BaseType_t err = 0;
       
    while(1)
    {
        serial_buf = USART_RX_BUF[0];    //读取串口接收到
        USART_RX_BUF[0] = 0;
        if(serial_buf == 0x11)
        {
            err = xQueueSend( queue_handle, &serial_buf, portMAX_DELAY );  //向队列中写入数据
            if(err == pdPASS)
            {
                printf("往queue_handle写入数据成功\r\n");
            }
        }
        else if(serial_buf == 0x22)
        {
            err = xSemaphoreGive( semphr_handle );      //释放二值信号量
            if(err == pdPASS)
            {
                printf("释放信号量成功\r\n");
            }
        }
        vTaskDelay(10);    /* 延时1000ticks */
    }
}

// 任务2，获取队列集的消息
void task2(void *pvParameters)
{
    uint8_t serial_buf;
    QueueSetMemberHandle_t member_handle;
    
    while(1)
    {
        //在任务中获取队列集中有有效消息的队列
        member_handle = xQueueSelectFromSet( queueset_handle, portMAX_DELAY);
        if(member_handle == queue_handle)
        {
            xQueueReceive( queue_handle, &serial_buf, portMAX_DELAY);
            printf("获取到的队列数据为：%d\r\n", serial_buf);
        }
        else if(member_handle == semphr_handle)
        {
            xSemaphoreTake( semphr_handle, portMAX_DELAY);
            printf("获取信号量成功\r\n");
        }
        
    }
}


