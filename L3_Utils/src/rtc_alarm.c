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

#include <stdlib.h>
#include "rtc_alarm.h"
#include "rtc.h"
#include "c_list.h"
#include "LPC17xx.h"



/**
 * Structure that holds the time alarm will trigger, and
 * the semaphore that will be given when the alarm triggers
 */
typedef struct {
    xSemaphoreHandle *pAlarm;   ///< Semaphore that is given when alarm is triggered
    alarmTimeType time;         ///< The time that triggers the alarm
} alarmType;

static c_list_ptr mDailyAlarmList = 0;            ///< Alarms for a specified time
static c_list_ptr mRecurAlarmList[4] = { 0 }; ///< Recurring alarms, such as "every second"

static void rtcEnableInterrupt(void)
{
    LPC_RTC->CIIR |= (1 << 0);
    NVIC_EnableIRQ(RTC_IRQn);
}

static c_bool forEachRecurringAlarmCallback(void *item, void *arg1, void *arg2, void *arg3)
{
    long switchReq=0;
    xSemaphoreHandle signal = (xSemaphoreHandle)item;
    xSemaphoreGiveFromISR(signal, &switchReq);
    return 1;
}

static c_bool forEachDailyAlarmCallback(void *item, void *arg1, void *arg2, void *arg3)
{
    const RTC time = rtc_gettime();
    alarmType *a = (alarmType*)item;
    if(a->time.hour == time.hour &&
       a->time.min == time.min &&
       a->time.sec == time.sec)
    {
        long switchReq = 0;
        xSemaphoreGiveFromISR(*(a->pAlarm), &switchReq);
    }
    return 1;
}




void rtc_EnableRecurringAlarm(xSemaphoreHandle signal, alarmFrequencyType freq)
{
    if(signal && freq >= everySecond && freq <= everyDay)
    {
        if (!mRecurAlarmList[freq]) {
            mRecurAlarmList[freq] = c_list_create();
            rtcEnableInterrupt();
        }
        c_list_insert_elm_end(mRecurAlarmList[freq], signal);
    }
}

alarmTimeType* rtc_CreateDailyAlarm(alarmTimeType time, xSemaphoreHandle *pAlarm)
{
    if (!mDailyAlarmList) {
        mDailyAlarmList = c_list_create();
        rtcEnableInterrupt();
    }

    alarmType *pNewAlarm = (alarmType*)malloc(sizeof(alarmType));
    pNewAlarm->time.hour = time.hour;
    pNewAlarm->time.min  = time.min;
    pNewAlarm->time.sec  = time.sec;
    pNewAlarm->pAlarm = pAlarm;

    c_list_insert_elm_end(mDailyAlarmList, pNewAlarm);
    return &(pNewAlarm->time);
}

#ifdef __cplusplus
extern "C" {
#endif
void RTC_IRQHandler(void)
{
    LPC_RTC->ILR |= (1 << 0); // Clear Increment Interrupt

    const RTC time = rtc_gettime();
    c_list_for_each_elm(mRecurAlarmList[everySecond], forEachRecurringAlarmCallback, NULL, NULL, NULL);
    if(0 == time.sec) {
        c_list_for_each_elm(mRecurAlarmList[everyMinute], forEachRecurringAlarmCallback, NULL, NULL, NULL);
        if(0 == time.min) {
            c_list_for_each_elm(mRecurAlarmList[everyHour], forEachRecurringAlarmCallback, NULL, NULL, NULL);
            if(0 == time.hour) {
                c_list_for_each_elm(mRecurAlarmList[everyDay], forEachRecurringAlarmCallback, NULL, NULL, NULL);
            }
        }
    }

    c_list_for_each_elm(mDailyAlarmList, forEachDailyAlarmCallback, NULL, NULL, NULL);
}
#ifdef __cplusplus
}
#endif
