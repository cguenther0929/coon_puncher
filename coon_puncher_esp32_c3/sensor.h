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
#include <esp_sleep.h>    //This likely defines wire.h
#include <Preferences.h>
#include <esp_timer.h>
#include <WiFi.h>
#include "nvm.h"
#include "i2c.h"
#include "epdif.h"
#include "driver/rtc_io.h"
#include "epd1in54_V2.h"
#include "epdpaint.h"
#include "imagedata.h"
#include "lan.h"

// ==============================
// ==============================
#define     SW_VER_STRING       "1.0.3" 
// ==============================
// ==============================

/**
 * Sleep Time
 * According to at least one f
 * online source, the maximum sleep time is 
 * 35 minutes.  
 */
# define SLEEP_TIME_MIN               30                          //The value that the user shall modify (max is 35 min?). 
# define SLEEPS_UNTIL_DISP_UPDATE     2                           // update the display after this many sleeps 
# define SLEEPS_UNTIL_EMAIL           24                          // Send an email after this many sleeps 
# define SLEEP_TIME_SEC               SLEEP_TIME_MIN * 60.0 
# define SLEEP_TIME_MICROS            SLEEP_TIME_SEC * 1000000.0  // ESP32 sleep function allows for a 64 bit int  (584,942 years)

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
 * GPIO Power Enable Pin
 */
#define nGPIO_EN_PWR            0

/**
 * Serial parameters
 */
#define SERIAL_BAUD_RATE        115200

/**
 * Capacity of battery
 */
#define DEFAULT_BAT_CAP         1700  // 2×850 mAh

/**
 * Button related
 */
#define BUTTON_INPUT        1
#define SHORT_PRESS_50MS_TICKS   20      // 20 of these in a second
#define LONG_PRESS_50MS_TICKS    60      // 20 of these in a second

/**
 * Interrupt / button pin
 * 
 */
#define INTERRUPT_PIN               BUTTON_INPUT    //RTC pins are GPIO0-GPIO3; the button ties to IO1, so the mask shall be 1

/**
 * General parameters related 
 * to the hygrometer application
 */

/**
 * Sensor related
 */
#define SENSOR_1                    1
#define SENSOR_2                    2

/* Define IO */
#define nSENSOR_PWR_EN              3
#define SENSOR_MUX_RST_LINE         9

typedef enum State {
  STATE_SLEEP,
  STATE_READ_DATA,
  STATE_UPDATE_DISPLAY,
  STATE_SEND_EMAIL
};

/**
 * ESP 32 Analog related parameters
 * It seems that that informaiton available online 
 * is incorrect.  
 * Some of the following values had to be 
 * empirically derived. 
 * We know the attenuation is 11db, which works 
 * out to a voltage gain of 3.548.  Knowing 
 * this, we demanded the digital code be printed 
 * out from the ADC during the "battery read"
 * routein.  Knowing the voltage feeding the pin, 
 * the internal attenuation value, and the fact that
 * we are dealing with 4096 steps (12 bit), we are 
 * able to discern the analog reference voltage. 
 */
// #define ANALOG_BATT_PIN             A0      // The analog pin on the ESP32-C that is 'watching' the battery voltage
// #define HYG_ADC_REFERENCE           0.50    // ESP32-C3 ADC reference (calculated)


class SENSOR
{
    public:
        
        bool sensor_is_calibrated               = false;
        bool measure_distance_flag              = false;
        float current_distance                  0.0;
        float threshold_distance                0.0;
        
        /**
         * @brief Update class parameter float current_distance    
         * @param \p none 
         * @return nothing 
         */
        void SENSOR::get_distance( void )

        
};

#endif