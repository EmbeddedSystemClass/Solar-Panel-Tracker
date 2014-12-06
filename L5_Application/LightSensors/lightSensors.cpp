/*
 * lightSensors.cpp
 *
 *  Created on: Apr 27, 2013
 *      Author: Montoya
 */
#include "lightSensor.hpp"
#include <stdio.h>      // printf

#include <sstream>
// Dependent on ADC
#define ADC_MASK 0xFFF // masks 12-bits
#define ADC_MAX_RANGE (double)(1 << 12) //12 maximum value readable by ADC 4096

// Dependent on sensor
#define LIGHT_SENSOR_MAX_RANGE (double)(180) // theoretical maximum number of degrees for servo motor
#define Start_degree (double)(0) // theoretical maximum number of degrees for servo motor
#define Middle_degree (double)(1 << 11) // theoretical maximum number of degrees for servo motor
#define buffer (double)(1 << 5) // buffer
#define degreeRange (double)(180.0) // Range of Degrees for Servo
#define lowThreshold (double)(80.0) //low threshold degree
#define highThreshold (double)(110.0) //high threshold degree
#define amplifyRange (double)(200)//Used to create a bigger light value to work with
#define degreeChange (double)(5.0)

#define start_degree (double)(6) // start degree
#define middle_degree (double)(90) // middle degree
#define end_degree (double)(174) //  end degree
  /**
     * Objective is get the value of the light sensor
     * and calculate the corresponding degree for the Servo Motors
     *
     * 1.1
     */

short getDirectionTop(LightSensors::Enum sensor){

    const unsigned int temp =((ADC_MASK & adc0_getReading(sensor)) / ADC_MAX_RANGE) * amplifyRange;

    if (( lowThreshold) >= temp){//80  degree
        return -(degreeChange);
    }
    else if ((highThreshold) <= temp ){//100  degree
            return degreeChange;
    }
    else
        return 0.0;
}

short getDirectionBase(LightSensors::Enum sensor){

    const unsigned int temp =((ADC_MASK & adc0_getReading(sensor)) / ADC_MAX_RANGE) * amplifyRange;

    if (( lowThreshold) >= temp){//80  degree
        return -(degreeChange);

    }
    else if ((highThreshold) <= temp ){//100  degree
            return degreeChange;
    }
    else
        return 0.0;
}

int checkDegree(int degree ){
    if(degree < start_degree)
                   degree = start_degree;
           else if ( degree > end_degree)
               degree = end_degree;

    return degree;
}
int setDegree(int degree, int diff){

    degree +=diff;

    return degree;
}

