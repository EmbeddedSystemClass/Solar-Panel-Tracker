/*
 * photoresistor_sensor.hpp
 *
 *  Created on: Apr 23, 2013
 *      Author: Montoya
 */

#ifndef PHOTORESISTOR_SENSOR_HPP_
#define PHOTORESISTOR_SENSOR_HPP_



/**
 * Photo resister Sensor class used to get light reading from the Board's Light Sensor
 *
 * @ingroup BoardIO
 */
class Photo_Sensor : public SingletonTemplate<Photo_Sensor>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

        unsigned short getRawValue();       ///< @returns Photo sensor reading
        unsigned short getPercentValue();   ///< @returns Photo sensor reading as percentage
        unsigned short getServoValue();// Numbers between 3 and 13
    private:
        Photo_Sensor() { }  ///< Private constructor of this Singleton class
        friend class SingletonTemplate<Photo_Sensor>;  ///< Friend class used for Singleton Template

        /// The ADC pin the sensor's analog output is attached to
        static const char mAdcChannelOfSensor = 4;
};




#endif /* PHOTORESISTOR_SENSOR_HPP_ */
