/*
 * servoMotor.cpp
 *
 *  Created on: Apr 27, 2013
 *      Author: Montoya
 */
#include "servoMotor.hpp"
#include "pwm.h"
#define Max_degree (double)(180)
#define Servo_Width (double)(3.1)
#define Servo_Start  (double)(0.2)
#define freq_ms  (double)(20.0)

void initilize_motor(){
    LPC_PINCON->PINSEL3 |= (2 << 14); // Set pin P1.23 as PWM1.4
    LPC_PINCON->PINSEL3 |= (2 << 16); // Set pin P1.23 as PWM1.5
    initPWM(200);
    pwmEn(1);
    change_direction_withDegree(90,ServoMotors::Top_servo);
    change_direction_withDegree(90,ServoMotors::Base_servo);
}
/**
     * Objective is allow user to give value in degrees
     * and calculate the corresponding MR value
     */

void change_direction_withDegree(double degree, ServoMotors::Enum servo){

    if (degree < 0)
    {
        degree = 0;
    }
    if (degree > Max_degree)
    {
        degree = Max_degree;
    }
    double max_value = LPC_PWM1->MR0;
    double target_value = ((degree/ Max_degree)*Servo_Width)+Servo_Start;
    target_value =(target_value/freq_ms) * max_value;
    pwmUpdateMR(servo, target_value);
}





