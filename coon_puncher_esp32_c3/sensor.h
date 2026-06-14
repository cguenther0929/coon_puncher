/**
 *  @filename   :   sensor.h
 *  @brief      :   Header file for sensor-related routines   
 *  @author     :   C. Guenther
 *  @details    :   The routines contained herein are very specific to the 
 *                  Coon Puncher project
 *  
 *  Copyright (C) C. Guenther May 25th 2026
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>    //This likely defines wire.h
#include <esp_timer.h>
#include "app.h"


class SENSOR
{
    public:
        bool sensor_ok_to_start                 = false;
        bool measure_distance_flag              = false;
        bool update_threshold                   = true;
        float current_distance                  = 0.0;
        float previous_distance                 = 0.0;
        float threshold_distance                = 99.99;
        uint8_t trip_count                      = 0;
        
        /**
         * @brief Sensor initialization routine
         * @param \p none 
         * @return nothing 
         */
        void init( void );

        /**
         * @brief Update class parameter float current_distance    
         * @param \p none 
         * @return nothing 
         */
        bool get_distance( void );

        
};

#endif