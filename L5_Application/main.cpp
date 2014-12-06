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
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/io_functions.h if you wish to override printf/scanf functions.
 *
 * @note  printf of %f is turned off to save memory, this can be configured at sys_config.h
 */

#include <stdio.h>      // printf
#include "tasks.hpp"
#include "handles.h"
#include "spi1.h"
#include "adc0.h"  // ADC driver
#include "pwm.h"   // PWM driver
#include "LightSensors/lightSensor.hpp"
#include "ServoMotors/ServoMotor.hpp"

#define createTask(function, stacksize, priority, handle)
/**
 * The main() creates tasks or "threads".  See the documentation of
 * scheduler_task class @ cpp_task.hpp for details.  There is a very
 * simple example towards the beginning of this class declaration.
 */
int main(void)
{

    HandlesType SysHandles;
    SysHandles.Queue.degree_Base = xQueueCreate(1, 4); // length , number of bytes
    SysHandles.Queue.degree_Top = xQueueCreate(1, 4); // length , number of bytes

    adc0_initialize(4);                           // Light Sensor
    adc0_initialize(5);
    initilize_motor();                           // Servo Motor
    printf("Starting Task\n");


    xTaskCreate( LightSensorsTask, (signed char*)"LightSensors", STACK_BYTES(1024*3), &SysHandles, 1,  &SysHandles.Task.lightSensor);
    xTaskCreate( ServoDirectionTask, (signed char*)"ServoDirection", STACK_BYTES(1024*3), &SysHandles, 1,  &SysHandles.Task.servoSensor);



    vTaskStartScheduler();
    return -1;
}

