/*
 * servoMotor.hpp
 *
 *  Created on: Apr 27, 2013
 *      Author: Montoya
 */

#ifndef SERVOMOTOR_HPP_
#define SERVOMOTOR_HPP_

namespace ServoMotors
{
    enum Enum
    {
        Top_servo = 4,
        Base_servo = 5
    }; // Mapped to ADC channel number
}
void initilize_motor();
void change_direction_withDegree(double,ServoMotors::Enum servo); // changes from 0 to 180 degrees

#endif /* SERVOMOTOR_HPP_ */
