/*
 * pwm.c
 *
 *  Created on: Apr 23, 2013
 *      Author: Montoya
 */


#include "pwm.h"

#include <stdio.h>           // printf


void initPWM(unsigned int pwm_rate){
    LPC_SC->PCONP |= (1<<6); // Enable PWM power on

    LPC_PWM1->MCR |= (1<<1); // Reset on PWMMR0: the PWMTC will be reset if PWMMR0 matches it.

    LPC_PWM1->MR0 =  getCpuClock()/pwm_rate; // Set PWM Frequency Ideal 50Hz (20ms) for reference
    printf("pulse Width: %i\n", getCpuClock()/pwm_rate);

    LPC_PWM1->MR4 = LPC_PWM1->MR0; // Set Channel 4 Duty Cycle to 50% // Changed
    LPC_PWM1->MR5 = LPC_PWM1->MR0; // Set Channel 5 Duty Cycle to 50% // Changed

    LPC_PWM1->LER |= (1<<0); // Allows the last value written to the PWM Match0 register to become effective when the
                             // timer is next reset by a PWM match event.
    LPC_PWM1->LER |= (1<<4); // Allows the last value written to the PWM Match4 register to become effective when the
                                 // timer is next reset by a PWM match event. //Changed
     LPC_PWM1->LER |= (1<<5); // Allows the last value written to the PWM Match5 register to become effective when the
                                    // timer is next reset by a PWM match event.// Changed

    LPC_PWM1->TCR |= (1 << 0); // Enable Timer counter and PWM prescale counter
    LPC_PWM1->TCR |= (1 << 3); // Enable PWM

    LPC_PWM1->CTCR &= ~(0xF<<0);

    pwmEn(0);
}

void pwmEn(unsigned char En){
    switch(En){
        case 0: // Turn PWM's Output Off
                    LPC_PWM1->PCR &= ~(1<<12);//4
                    LPC_PWM1->PCR &= ~(1<<13);//5

                    break;
        case 1: // Turn PWM's Output On
                    LPC_PWM1->PCR |= (1<<12);//4
                    LPC_PWM1->PCR |= (1<<13);//5
                    break;
    }
}

void pwmUpdateMR(unsigned char MRNum, int data){
    switch(MRNum){
        case 0:
            LPC_PWM1->MR0 = data;
            LPC_PWM1->LER |= (1<<0);
            break;
        case 1:
            LPC_PWM1->MR1 = data;
            LPC_PWM1->LER |= (1<<1);
            break;
        case 2:
            LPC_PWM1->MR2 = data;
            LPC_PWM1->LER |= (1<<2);
            break;
        case 3:
            LPC_PWM1->MR3 = data;
            LPC_PWM1->LER |= (1<<3);
            break;
        case 4:
            LPC_PWM1->MR4 = data;
            LPC_PWM1->LER |= (1<<4);
            break;
        case 5:
            LPC_PWM1->MR5 = data;
            LPC_PWM1->LER |= (1<<5);
            break;
        case 6:
            LPC_PWM1->MR6 = data;
            LPC_PWM1->LER |= (1<<6);
            break;
    }
}

int pwmReadMR(unsigned char MRNum){
    switch(MRNum){
        case 0:
            return(LPC_PWM1->MR0);
            break;
        case 1:
            return(LPC_PWM1->MR1);
            break;
        case 2:
            return(LPC_PWM1->MR2);
            break;
        case 3:
            return(LPC_PWM1->MR3);
            break;
        case 4:
            return(LPC_PWM1->MR4);
            break;
        case 5:
            return(LPC_PWM1->MR5);
            break;
        case 6:
            return(LPC_PWM1->MR6);
            break;
        default:
            return 0;
            break;
    }
}


