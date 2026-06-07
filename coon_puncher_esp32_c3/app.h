/**
 *  @filename   :   app.h
 *  @brief      :   Header file for general hygrometer application functions  
 *  @author     :   C. Guenther
 *  @details    :   The routines contained herein are very specific to the 
 *                  Digital Hygrometer project
 *  
 *  Copyright (C) C. Guenther December 10 2024
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

#ifndef APPFUNCTIONS_H
#define APPFUNCTIONS_H

#include <Arduino.h>    //This likely defines wire.h
#include "sensor.h"

// ==============================
// ==============================
#define     SW_VER_STRING       "1.0.0" 
// ==============================
// ==============================


/**
 * Set to true to 
 * enable logging
 */
#define ENABLE_LOGGING          true

/**
 * Health LED
 */
#define HEALTH_LED              10

/**
 * Button related
 */
#define BUTTON_INPUT              0
#define SHORT_PRESS_50MS_TICKS   20      // 20 of these in a second
#define LONG_PRESS_50MS_TICKS    60      // 20 of these in a second

/**
 * H-Bridge Related
 */
#define HBR_IN1                   18
#define HBR_IN2                   19

/**
 * Sensor related
 */
#define TRIG_PIN                  3
#define ECHO_PIN                  4

/**
 * Serial parameters
 */
#define SERIAL_BAUD_RATE          115200

/**
 * 50ms ticks required for the solenoid to move
 */
#define SOLENOID_TRAVEL_50MS_TICKS_REQUIRED         20


/**
 * Button related
 */
#define SHORT_PRESS_50MS_TICKS   20      // 20 of these in a second
#define LONG_PRESS_50MS_TICKS    60      

typedef enum State {
  STATE_WAIT_FOR_CAL,
  STATE_SLEEP,
  STATE_UNKNOWN,
  STATE_READ_DISTANCE,
  STATE_EVALUATE_DISTANCE,
  STATE_CALIBRATE,
  STATE_RESET_TRAP
};

class SENSOR;                         // Inserting a forward declaration here to thwart compiler errors

class APP
{
    public:
        uint8_t state                           = STATE_UNKNOWN; //Initalized to READ_DISTANCE  in the init routine

        uint8_t local_boot_counter              = 0x00;
        uint16_t btn_press_ctr                  = 0x0000;
        uint16_t ms50_timer_ticks               = 0x0000;
        
        bool btn_interrupt_triggered            = false;
        bool btn_short_press_flag               = false;
        bool btn_long_press_flag                = false;
        bool enable_led                         = true;     //Start enabled so we blink for calibration 
        bool fast_blink                         = false;
        bool trap_has_triggered                 = false;
        bool ms50_timer_enabled                 = false;

        uint16_t seconds_counter                = 0x0000;

        bool heartbeat_enabled                  = true;

        /**
         * @brief APP init function
         * Constructors run automatically, thus
         * this init function needs to be called.
         * @param \p none 
         * @return nothing
         */
        void init(void);

        /**
         * @brief Handle button press   
         * @param \p none 
         * @return nothing 
         */
        void button_handler ( void );
        
        /**
         * @brief Process current state or switch to new state   
         * @param \p Preferences Instance \p Application Instance 
         * @return nothing 
         */
        void state_handler (APP & app_functions, SENSOR & sensor_functions);
        
        /**
         * @brief Function to set H-bridge to extend trap
         * @param \p none 
         * @return nothing 
         */
        void solenoid_extend( void );
        
        /**
         * @brief Function to set H-bridge to retract trap
         * @param \p none 
         * @return nothing 
         */
        void solenoid_retract( void );
        
        /**
         * @brief Function to disable the H-bridge/solenoids
         * @param \p none 
         * @return nothing 
         */
        void solenoid_stop( void );
};

#endif