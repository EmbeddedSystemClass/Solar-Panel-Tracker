/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "cpp_task.hpp"
#include "c_list.h"
#include "c_tlm_comp.h"
#include "c_tlm_var.h"


/** @{ Private global variables */
static c_list_ptr mTaskList = NULL;        /**< Instance of our task list */
static xTaskHandle mRunInitTaskHandle = 0; /**< Task handle that will call runInit() for everyone */
static xSemaphoreHandle mRunTask = 0;      /**< Semaphore for a task to proceed to run() method */
/** @} */

/** Print two strings followed by newline characters */
static void print_strings(const char *one, const char *two)
{
    while(*one) {
        putchar(*one++);
    }
    puts(two);
}

/**
 * This is the C task that is created multiple times.  Each task receives its
 * own parameter therefore differentiating between different tasks.
 */
void scheduler_c_task_private(void *task_ptr)
{
    scheduler_task& task = *(scheduler_task*)task_ptr;

    /* Have the responsible task call taskEntry() for each task */
    if (mRunInitTaskHandle == task.mHandle) {
        const uint32_t num_tasks = c_list_node_count(mTaskList);
        void *hint = 0;
        bool failure = false;

        print_strings(task.mName, " task calling taskEntry() for all tasks ... ");
        for (uint32_t i=0; i < num_tasks; i++)
        {
            scheduler_task *t = (scheduler_task*)c_list_get_elm_at(mTaskList, i, &hint);
            if (!t->taskEntry()) {
                print_strings(t->mName, "  --> FAILED taskEntry()");
                failure = true;
            }
        }

        if (failure) {
            puts("ERROR: Killing FreeRTOS due to error(s)");
            vTaskEndScheduler();
        } else {
            /* Give permission for everyone to start the run() loop */
            for (uint32_t i=0; i < num_tasks; i++)
            {
                xSemaphoreGive(mRunTask);
            }
        }
    }

    // Wait until we're given the go ahead by the task giving semaphore above
    xSemaphoreTake(mRunTask, portMAX_DELAY);

    portTickType xLastWakeTime = xTaskGetTickCount();
    portTickType xNextStatTime = 0;

    for (;;)
    {
        // Run the task code and suspend when an error occurs
        if (!task.run((void*)task.mParam)) {
            print_strings(task.mName, " --> FAILURE detected; suspending this task ...");
            vTaskSuspend(0);
        }
        ++(task.mRunCount);

        // Update the task statistics once in a short while :
        if (xTaskGetTickCount() > xNextStatTime) {
            xNextStatTime = xTaskGetTickCount() + OS_MS(task.mStatUpdateRateMs);
            task.mFreeStack = uxTaskGetStackHighWaterMark(task.mHandle);
            task.mCpuPercent = uxTaskGetCpuUsage(task.mHandle);
        }

        // Delay if set by setFrequency()
        if (task.mTaskDelayMs) {
            vTaskDelayUntil( &xLastWakeTime, OS_MS(task.mTaskDelayMs));
        }
    }
}

bool scheduler_init_all(bool register_internal_tlm)
{
    const uint32_t num_tasks = c_list_node_count(mTaskList);
    bool failure = false;
    void *hint = 0;

    /* If not tasks ... */
    if (0 == num_tasks) {
        puts("ERROR: NO tasks added by scheduler_add_task()");
        failure = true;
        return failure;
    }

    /* Initialize all tasks */
    puts("Initializing tasks ...");
    for (uint32_t i=0; i < num_tasks; i++)
    {
        scheduler_task *task = (scheduler_task*)c_list_get_elm_at(mTaskList, i, &hint);
        if (!task->init()) {
            print_strings(task->mName, "  --> FAILED init()");
            failure = true;
        }
    }

    /* Register telemetry for all tasks */
#if ENABLE_TELEMETRY
    puts("Registering tasks' telemetry ...");
    for (uint32_t i=0; i < num_tasks; i++)
    {
        scheduler_task *task = (scheduler_task*)c_list_get_elm_at(mTaskList, i, &hint);
        if (!task->regTlm()) {
            failure = true;
        }

        /* Register statistical variables of this task */
        if (register_internal_tlm) {
            tlm_component *comp = tlm_component_add(task->mName);
            if (!tlm_variable_register(comp, "CPU %", &(task->mCpuPercent),
                sizeof(task->mCpuPercent), 1, tlm_uint)) {
                failure = true;
            }
            if (!tlm_variable_register(comp, "Free stack", &(task->mFreeStack),
                 sizeof(task->mFreeStack), 1, tlm_uint)) {
                failure = true;
            }
            if (!tlm_variable_register(comp, "Run count", &(task->mRunCount),
                 sizeof(task->mRunCount), 1, tlm_uint)) {
                failure = true;
            }
        }
        if (failure) {
            print_strings(task->mName, "  --> FAILED telemetry registration");
        }
    }
#endif

    puts("Creating tasks ...");
    for (uint32_t i=0; i < num_tasks; i++)
    {
        mRunTask = xSemaphoreCreateCounting(num_tasks, 0);
        scheduler_task *task = (scheduler_task*)c_list_get_elm_at(mTaskList, i, &hint);
        if (!xTaskCreate(scheduler_c_task_private,
                        (signed char*)task->mName,      /* Name  */
                        STACK_BYTES(task->mStackSize),  /* Stack */
                        task,                           /* Task param    */
                        task->mPriority,                /* Task priority */
                        &(task->mHandle)))              /* Task Handle   */
        {
            print_strings(task->mName, "  --> FAILED xTaskCreate()");
            failure = true;
        }
    }

    /* Find the task with highest stack, and this task will then be used
     * to call taskEntry() for everyone once FreeRTOS starts
     */
    uint32_t highestStack = 0;
    for (uint32_t i=0; i < num_tasks; i++)
    {
        scheduler_task *task = (scheduler_task*)c_list_get_elm_at(mTaskList, i, &hint);
        if (task->mStackSize > highestStack) {
            highestStack = task->mStackSize;
            mRunInitTaskHandle = task->mHandle;
        }
    }

    return failure;
}

bool scheduler_add_task(scheduler_task* task)
{
    if (0 == mTaskList) {
        mTaskList = c_list_create();
    }

    return c_list_insert_elm_end(mTaskList, task);
}

void scheduler_start(bool register_internal_tlm)
{
    /* If no failure, start the FreeRTOS scheduler */
    if (!scheduler_init_all(register_internal_tlm)) {
        puts("Starting scheduler ...");
        vTaskStartScheduler();

        // vTaskStartScheduler() should not return
        puts("ERROR: Someone killed the scheduler");
    }
    else {
        puts("ERROR: Refusing to start OS scheduler due to error(s)");
        while(1);
    }
}



scheduler_task::scheduler_task(const char *name, uint32_t stack, uint8_t priority, void *param) :
   mHandle(0),
   mFreeStack(0),
   mCpuPercent(0),
   mRunCount(0),
   mTaskDelayMs(0),
   mStatUpdateRateMs(1000),
   mName(name),
   mParam(param),
   mPriority(priority),
   mStackSize(stack)
{

}
