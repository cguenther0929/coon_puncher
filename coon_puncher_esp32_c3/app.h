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
#include <esp_sleep.h>    //This likely defines wire.h
#include <Preferences.h>
#include <esp_timer.h>
#include <WiFi.h>
#include "nvm.h"
// #include "i2c.h"
// #include "epdif.h"
// #include "driver/rtc_io.h"
// #include "epd1in54_V2.h"
// #include "epdpaint.h"
// #include "imagedata.h"
#include "sensor.h"
#include "lan.h"

// ==============================
// ==============================
#define     SW_VER_STRING       "0.0.1" 
// ==============================
// ==============================

/**
 * Sleep Time
 * According to at least one f
 * online source, the maximum sleep time is 
 * 35 minutes.  
 */
// # define SLEEP_TIME_MIN               30                          //The value that the user shall modify (max is 35 min?). 
// # define SLEEPS_UNTIL_DISP_UPDATE     2                           // update the display after this many sleeps 
// # define SLEEPS_UNTIL_EMAIL           24                          // Send an email after this many sleeps 
// # define SLEEP_TIME_SEC               SLEEP_TIME_MIN * 60.0 
// # define SLEEP_TIME_MICROS            SLEEP_TIME_SEC * 1000000.0  // ESP32 sleep function allows for a 64 bit int  (584,942 years)

/**
 * Set to true to 
 * enable logging
 */
#define ENABLE_LOGGING          false

/**
 * Health LED
 */
#define HEALTH_LED              10

/**
 * Reset Trap Input
 */
#define BUTTON_INPUT              7

/**
 * H-Bridge Related
 */
#define HBR_IN1                   18
#define HBR_IN2                   19
s
/**
 * Sensor related
 */
#define TRIG_PIN                  3
#define ECHO_PIN                  18

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
#define BUTTON_INPUT        BUTTON_INPUT
#define SHORT_PRESS_50MS_TICKS   20      // 20 of these in a second
#define LONG_PRESS_50MS_TICKS    60      


/**
 * Interrupt / button pin
 * 
 */
// #define INTERRUPT_PIN               BUTTON_INPUT    //RTC pins are GPIO0-GPIO3; the button ties to IO1, so the mask shall be 1


typedef enum State {
  STATE_SLEEP,
  STATE_UNKNOWN,
  STATE_READ_DISTANCE,
  STATE_EVALUATE_DISTANCE,
  STATE_CALIBRATE,
  STATE_RESET_TRAP
};


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
        bool reset_trap                         = false;
        bool ms50_timer_enabled                 = false;

        uint16_t seconds_counter                = 0x0000;

        // float battery_charge_percentage  = 0.0;

        bool heartbeat_enabled                  = true;

        /**
         * @brief APP init function
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
        void state_handler (APP & app_functions, SENSOR sensor_functions);
        
        /**
         * @brief Blink the health LED for quick verification   
         * @param \p none 
         * @return nothing 
         */
        void heartbeat_post( void );
        
        /**
         * @brief Reset the trap
         * @param \p none 
         * @return nothing 
         */
        void APP::reset_trap( void );
        
        /**
         * @brief Function to set H-bridge to extend trap
         * @param \p none 
         * @return nothing 
         */
        void APP::solenoid_extend( void )
        
        /**
         * @brief Function to set H-bridge to retract trap
         * @param \p none 
         * @return nothing 
         */
        void APP::solenoid_retract( void )
        
        /**
         * @brief Function to disable the H-bridge/solenoids
         * @param \p none 
         * @return nothing 
         */
        void APP::solenoid_stop( void )
};

#endif