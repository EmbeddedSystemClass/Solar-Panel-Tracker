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
 * @brief This file provides API to enable real-time clock FreeRTOS signals or alarms
 * @ingroup Utilities
 */

#ifndef RTC_SEM_HPP_
#define RTC_SEM_HPP_
#ifdef __cplusplus
extern "C" {
#endif



#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"



/**
 * Frequency of a recurring alarm
 */
typedef enum {
    everySecond = 0,
    everyMinute = 1,
    everyHour   = 2,
    everyDay    = 3,
} alarmFrequencyType;

/**
 * dailyAlarm_Create() returns the pointer to this structure.
 * This structure can be used to change the alarm time after
 * it has been created.
 */
typedef struct {
    uint8_t hour, min, sec;
} alarmTimeType;



/**
 * Enables a permanent recurring alarm every second, minute, or hour
 * @param signal  The signal (semaphore) to give when the alarm triggers
 * @param freq    The frequency of the alarm
 */
void rtc_EnableRecurringAlarm(xSemaphoreHandle signal, alarmFrequencyType freq);

/**
 * Enables alarm at the given @param time
 * @post pAlarm semaphore will be given when RTC time matches the given time.
 * @returns alarmTimeType that can be used to modify alarm time
 */
alarmTimeType* rtc_CreateDailyAlarm(alarmTimeType time, xSemaphoreHandle *pAlarm);

/**
 * Turns off an alarm that was created by dailyAlarm_Create()
 * Nothing special here, the hour is set to 25, which will never occur.
 */
inline void rtc_TurnOffDailyAlarm(alarmTimeType *p) { p->hour = 25; p->min = p->sec=0; }



#ifdef __cplusplus
}
#endif
#endif /* RTC_SEM_HPP_ */
