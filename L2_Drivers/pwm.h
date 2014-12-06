/*
 * pwm.h
 *
 *  Created on: Apr 23, 2013
 *      Author: Montoya
 */

#ifndef PWM_H_
#define PWM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "LPC17xx.h"
#include "sys_Config.h"

void initPWM(unsigned int pwm_rate);

void pwmUpdateMR(unsigned char MRNum, int data);

int pwmReadMR(unsigned char MRNum);

void pwmEn(unsigned char enable);

#ifdef __cplusplus
}
#endif
#endif /* PWM_H_ */
