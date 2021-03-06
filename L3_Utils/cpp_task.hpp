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

/**
 * @file
 * @brief C++ Task class based on FreeRTOS
 * @ingroup Utilities
 *
 * This file provides the structure to create and manage a FreeRTOS task.
 * @see scheduler_task for further documentation
 */
#ifndef CPP_TASK_HPP_
#define CPP_TASK_HPP_
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"



/// Forward declaration of the scheduler task class
class scheduler_task;


/**
 * Adds your task to the scheduler
 */
bool scheduler_add_task(scheduler_task *task);

/**
 * Starts FreeRTOS scheduler with the tasks added by scheduler_add_task()
 * @param register_internal_tlm  If true, then this method will register the
 *        statistical telemetry for each task.  @note This is different from
 *        the regTlm() method that is called regardless.
 */
void scheduler_start(bool register_internal_tlm=true);


/**
 * Scheduler task.
 * This class should be inherited and at minimum, you would need to override the
 * run() function to execute as part of your task.  Note that the run() function
 * should return as the scheduler is responsible for the FreeRTOS task loop.
 *
 * The order of operations is the following :
 *      - init()
 *      - regTlm()
 *      - FreeRTOS is started
 *      - taskEntry()
 *      - main loop starts to call your run() function
 *
 * Example task that prints a message every 1000ms :
 * @code
 *      class my_task : public task_base
 *      {
 *          public :
 *          // Call task_base() constructor with your task name, stack size, and priority
 *          my_task() : task_base("task name", 1024, 1)
 *          {
 *              setFrequency(1000); // Optional frequency control for run()
 *          }
 *
 *          bool run(void *param) {
 *              puts("Hello World");
 *              return true;
 *          }
 *      };
 *
 *      // Add the task to the scheduler and start it :
 *      scheduler_add_task(new my_task());
 *      scheduler_start();
 * @endcode
 */
class scheduler_task
{
    public :
        /* No public API */

    protected:
        /** Constructor
         * @param name      The task's name
         * @param stack     The task's stack size in bytes
         * @param priority  FreeRTOS task priority
         * @param param     OPTIONAL param to pass to your run() when it's called.
         */
        scheduler_task(const char *name, uint32_t stack, uint8_t priority, void *param=0);
        virtual ~scheduler_task() {}

        /**
         * Optional: Override this function to initialize anything before FreeRTOS starts.
         * @return   true upon success.
         * @warning  DO NOT USE FreeRTOS blocking API in this function
         */
        virtual bool init() { return true; }

        /**
         * Optional: Override this function to register your telemetry before FreeRTOS starts.
         * @return   true upon success.
         * @warning  DO NOT USE FreeRTOS blocking API in this function
         */
        virtual bool regTlm() { return true; }

        /**
         * Optional : Override this function which is called ONCE after FreeRTOS starts
         * If your task depends on someone else's init(), then you can put your
         * relevant code here that is called after everyone's init().  You CAN
         * use FreeRTOS API but note that all other tasks will wait for everyone's
         * taskEntry() to take place before the run() method is called.
         *
         * @warning  Do not suspend your task otherwise all tasks will get suspended.
         * @return   true upon success.
         */
        virtual bool taskEntry() { return true; }

        /**
         * MUST override this function to run your tasks' code
         * @param param  The same pointer as the constructor's param
         * @return   false if something unexpected occurs.  In this case, the
         *           task will be suspended from doing further work.
         */
        virtual bool run(void *param)=0;

        /**
         * This can be used to set a desired time that the run() method will be called.
         * For example, if frequency is set to 1000, then the run() will be called
         * every 1 second regardless the duration that your run() method.  If your run()
         * takes longer than the milliseconds you set, then it will be called without a
         * delay.
         * @param milliseconds The frequency to call your run() function.
         *                     Set this to zero to disable this functionality.
         * @note By default, run() method is called continuously without a delay.
         */
        void setFrequency(uint32_t milliseconds)
        {
            mTaskDelayMs = milliseconds;
        }

        /** @{ Statistical values your task can obtain */
        inline uint32_t getFreeStack(void) const { return mFreeStack; }
        inline uint8_t  getCpuUsage(void) const { return mCpuPercent; }
        inline uint32_t getRunCount(void) const { return mRunCount; }
        /** @} */

        /**
         * Set the update rate in milliseconds that the CPU usage, and free
         * stack size is calculated at.  Default rate is 1 second (1000 ms)
         */
        inline void setStatUpdateRate(uint32_t rateMs) { mStatUpdateRateMs = rateMs; }

    private:
        scheduler_task() : mName(0), mParam(0), mPriority(0), mStackSize(0) {}

        /** @{ Other member variables */
        xTaskHandle mHandle;    /** Task handle of this task */
        uint32_t mFreeStack;    /** Free stack of this task */
        uint8_t mCpuPercent;    /** CPU usage in percentage of this task */
        uint32_t mRunCount;     /** run-count of run() method */
        uint32_t mTaskDelayMs;  /** To set task frequency */
        uint32_t mStatUpdateRateMs; /** Statistics update rate */
        /** @} */

        /** @{ FreeRTOS task creation parameters */
        const char *mName;          /**< Task name */
        const void *mParam;         /**< Parameter that will be passed to run() */
        const uint8_t mPriority;    /**< Task priority */
        const uint32_t mStackSize;  /**< Stack size in bytes */
        /** @} */

        /** @{ Give access to our private members to these functions */
        friend bool scheduler_init_all(bool register_task_tlm);
        friend void scheduler_c_task_private(void *param);
        /** @} */
};


#endif /* CPP_TASK_HPP_ */
