/*
 * servo_motor.hpp
 *
 *  Created on: Apr 23, 2013
 *      Author: Montoya
 */

#ifndef SERVO_MOTOR_HPP_
#define SERVO_MOTOR_HPP_


/**
 * Servo_Motor class used to control the direction of the Servo Motors
 *Servo_Motor
 * @ingroup BoardIO
 */
class Servo_Motor : public SingletonTemplate<Servo_Motor>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

       // unsigned short getRawValue();       ///< @returns Photo sensor reading
       // unsigned short getPercentValue();   ///< @returns Photo sensor reading as percentage
        void change_direction(float); // chages from 0 to 180 degrees
        void set_degree(float);
    private:
        Servo_Motor() { }  ///< Private constructor of this Singleton class
        friend class SingletonTemplate<Servo_Motor>;  ///< Friend class used for Singleton Template


};


#endif /* SERVO_MOTOR_HPP_ */
