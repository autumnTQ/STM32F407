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
#include "malloc.h"	   
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"

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
#define TASK1_PRIO      1                   /* 任务优先级 */
#define TASK1_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
void task1(void *pvParameters);             /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO      1                   /* 任务优先级 */
#define TASK2_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

/******************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
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

/**
 * @brief       task1
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task1(void *pvParameters)
{    
    while(1)
    {
//        printf("task1运行!!!\r\n");
        vTaskDelay(1000);                                               /* 延时1000ticks */
    }
}


char task_buff[500];

/**
 * @brief       task2
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task2(void *pvParameters)
{
    UBaseType_t task_num = 0;
    TaskStatus_t * status_array = NULL;    //用于获取所有任务状态的数组
    TaskStatus_t * status_array2 = NULL;   //用于获取单个任务状态的数组
    uint8_t i = 0;
    
    task_num = uxTaskGetNumberOfTasks();  // 获取任务的数量
    status_array = mymalloc(SRAMIN, sizeof(TaskStatus_t)* task_num);
    task_num = uxTaskGetSystemState( (TaskStatus_t*)status_array, /* 任务状态信息 buffer */
                                     (UBaseType_t)task_num,       /* buffer 大小 */
                                     (uint32_t*)NULL);            /* 不获取任务运行时间信息 */
    printf("任务名\t\t 优先级\t\t 任务编号\r\n");
    for (i=0; i<task_num; i++)
    {
        printf("%s\t%ld\t\t%ld\t\t\t%d\r\n",
                                status_array[i].pcTaskName,
                                status_array[i].uxCurrentPriority,
                                status_array[i].xTaskNumber,
                                status_array[i].usStackHighWaterMark);
    }
    
    status_array2 = mymalloc(SRAMIN, sizeof(TaskStatus_t));
    vTaskGetInfo( Task2Task_Handler, status_array2, pdTRUE, eInvalid);
    
    printf("任务名：%s\r\n", status_array2->pcTaskName);
    printf("任务优先级：%ld\r\n", status_array2->uxCurrentPriority);
    printf("任务编号：%ld\r\n", status_array2->xTaskNumber);
    printf("任务状态：%d\r\n", status_array2->eCurrentState);
    
    myfree(SRAMIN, status_array);  
    myfree(SRAMIN, status_array2);
    
    vTaskList(task_buff); //以“表格”的形式获取系统中任务的信息
    printf("%s\r\n", task_buff);
    

    while(1)
    {
        vTaskDelay(1000);     /* 延时1000ticks */
    }
}
