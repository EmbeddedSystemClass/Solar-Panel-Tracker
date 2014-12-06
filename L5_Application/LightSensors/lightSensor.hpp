/*
 * lightSensor.hpp
 *
 *  Created on: Apr 27, 2013
 *      Author: Montoya
 */

#ifndef LIGHTSENSOR_HPP_
#define LIGHTSENSOR_HPP_
#include "adc0.h"   // ADC driver for channels 0 - 7

namespace LightSensors
{
    enum Enum
    {
        Left = 4,
        Right = 5
    }; // Mapped to ADC channel number
}

short getDirectionTop(LightSensors::Enum sensor);
short getDirectionBase(LightSensors::Enum sensor);
int checkDegree(int );
int setDegree(int, int);

#endif /* LIGHTSENSOR_HPP_ */
