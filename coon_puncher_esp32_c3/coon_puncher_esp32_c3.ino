/**
 *  @filename   :   coon_puncher_esp32_c3.ino
 *  @brief      :   Electronic Trap Actuator
 *  @author     :   C. Guenther
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
 * TODO Add filter to get distance ?
 * TODO determine if we're powering up for the first time, and if so, wait for calibration 
 */


#include <Arduino.h>
#include <stdio.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <SPI.h>
#include <Wire.h>
#include <esp_timer.h>
#include <Preferences.h>
#include "nvm.h"
#include "i2c.h"
#include "console.h"
#include "app_functions.h"
#include "lan.h"

/**
 * Instantiate the Preferences class
 * which is used for NVM
 */ 
Preferences pref;   

/**
 * Global variable to 
 * keep track of how many 
 * times the module has 
 * rebooted.  
 */
RTC_DATA_ATTR int rtc_boot_ctr        = 0;
RTC_DATA_ATTR int email_send_boot_ctr = 0;

/**
 * Timer parameters
 */
unsigned int    ms_ticks_50           = 0;
unsigned int    ms_ticks_100          = 0;
unsigned int    ms_ticks_500          = 0;
unsigned int    ms_ticks_1000         = 0;

bool            Timer50msFlag         = false;
bool            Timer100msFlag        = false;
bool            Timer500msFlag        = false;
bool            Timer1000msFlag       = false;

char rx_char                          = '\n';

/**
 * Time structure 
 */
hw_timer_t *timer = NULL;     

/**
 * Define class instances
 */

// I2C     main_i2c;
CONSOLE app_console;
// LAN     lan;
// NVM     nvm_functions;
APP     app_functions;
SENSOR  sensor_functions;

/**
 * Wake from deep sleep using a timer
 * ================================
 * ESP32 Deep Sleep Mode Discussion
 * https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
 * 
 * Some larger points from the article
 * First, you need to configure the wake up sources. 
 * This means configure what will wake up the ESP32. 
 * You can use one or combine more than one wake up source.
 * You can decide what peripherals to shut down or keep 
 * on during deep sleep. However, by default, the ESP32 automatically
 * powers down the peripherals that are not needed with the
 * wake up source you define. Finally, you use the esp_deep_sleep_start() 
 * function to put your ESP32 into deep sleep mode.
 * The ESP32 can go into deep sleep mode, and then wake up at predefined
 * periods of time. This feature is specially useful if you are
 * running projects that require time stamping or daily tasks, 
 * while maintaining low power consumption.
 * Enabling the ESP32 to wake up after a predefined amount of
 * time is very straightforward. In the Arduino IDE, you 
 * just have to specify the sleep time in microseconds in the following function:
 * esp_sleep_enable_timer_wakeup(time_in_us)
 * 
 * 
 * ================================
 * Wake from deep sleep using an IO pin
 * ================================
 * 
 * https://hutscape.com/tutorials/external-wakeup-arduino-esp32c3
 * 
 * There is some incorrect information floating around about waking the processor.  
 * The aforementioned article points out that we shall use 
 * esp_deep_sleep_enable_gpio_wakeup(1 << INTERRUPT_PIN, ESP_GPIO_WAKEUP_GPIO_HIGH);
 * instead of sp_sleep_enable_ext0_wakeup(WAKEUP_GPIO, 1); 
 * to wake the processor.  
 * 
 * The ESP32-C3 will wake up from an interrupt that can be caused
 * by an RTC pin.  According to a Google search, these are pins
 * GPIO0, GPIO1, GPIO2, and GPIO3.  On the hygrometer, the calibration 
 * button feeds into IO1.
 * 
 * The wakeup IO needs to be defined as a mask,
 * it was empirically determined that this value is one based.  Therefor, the mask
 * shall be 0 for GPIO0, 1 for GPIO1, and so on and so forth.  
 * 
 * 
 // This is how to place the ESP32 into deep sleep 
 //                               Value in uS  
 //                                 |  
 // esp_sleep_enable_timer_wakeup(1000);
 // esp_deep_sleep_start();  //This will put the module into deep sleep
 */

/**
 * Note for attaching an interrupt 
 * ================================
 * These are notes on attaching a button interrupt.
 * https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/
 * 
 */

/**
 * Note for using a timer interrupt  
 * ================================
 * https://circuitdigest.com/microcontroller-projects/esp32-timers-and-timer-interrupts
 *  
 */

/**
 * @brief Timer interrupt
 * @details This function has to 'live' 
 * up above the setup routine
 * 
 */
void IRAM_ATTR onTimer()
{
  Timer50msFlag = true;

  if(ms_ticks_50 == 2) {
    ms_ticks_50 = 0;
    Timer100msFlag = true;
    if(ms_ticks_100 == 5) {
      ms_ticks_50 = 0;               //Reset centi-seconds
      Timer500msFlag = true;
    
      if(ms_ticks_500 == 2) {         //See if we need to roll over
        ms_ticks_500 = 0;
        Timer1000msFlag = true;  
      }
      else {
        ms_ticks_500++;              //Increase 500ms timer
      }

    }
    else {
        ms_ticks_100++;
    }
    
  }
  else {
    ms_ticks_50++;
  }
}

/**
 * @brief Button Press Interrupt 
 * @details This function has to 'live' 
 * up above the setup routine
 */
void IRAM_ATTR button_press()    
{
  if(ENABLE_LOGGING)
  {
    Serial.println("^Button interrupt triggered.");
  }
  
  /**
   * All we need this function to 
   * do is to wake up the processor.
   * The buttonhandler routine will
   * increment the button counter
   * and act accordingly 
   */
  app_functions.btn_interrupt_triggered  = true;
  detachInterrupt(digitalPinToInterrupt(BUTTON_INPUT));  
}

/**
 * @brief Arduino Setup routine
 */
void setup() 
{
  Serial.begin(SERIAL_BAUD_RATE);
  
  pinMode(HEALTH_LED,OUTPUT);
  digitalWrite(HEALTH_LED, HIGH);
  
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  
  pinMode(ECHO_PIN, INPUT);
  
  pinMode(BUTTON_INPUT, INPUT_PULLUP);
  
  pinMode(HBR_IN1,OUTPUT);
  digitalWrite(HBR_IN1, LOW);
  
  pinMode(HBR_IN2,OUTPUT);
  digitalWrite(HBR_IN2, LOW);

}

  
  
  /**
   * @brief Define IO interrupt for push button input 
   */
  //           IO Pin number that shall trigger the interrupt                              
  //                      |         Name of the callback function               
  //                      |               |     Type of signal edge to detect    
  //                      |               |         |
  attachInterrupt(BUTTON_INPUT, button_press, FALLING); 

  /**
   * @brief Function for Defining which IO shall wake the MCU from deep sleep
   * @details This function will allow an IO pin (RTC1-5) 
   * to wake the processor from deep sleep mode
   * It's unclear if allowing the processor to be
   * awoken from deep sleep in this manner eats more 
   * power. 
   * 
   * If the "mask" version is used, the device will almost 
   * immediately wake up (BAD).  The third option allows for 
   * proper sleep operation, however, that routine doesn't
   * wake up the processor like we want.  
   * I also learned that the ESP32C3 does not support 
   * RTC wakeup, so we need to be careful when pulling examples
   * from online
   * The ESP32 Wiki mentions that GPIO wakeup is for ***light sleep***
   * If we want to wake the processor from deep sleep, then
   * we'll have to use a different processor
   */
  //                                  A mask value needs to be passed in (empirically found to be one-based)
  //                                     |                  Parameter for the input signal   
  //                                     |                     |
  //                                     |                     |
  // esp_deep_sleep_enable_gpio_wakeup(1 << (INTERRUPT_PIN + 1), ESP_GPIO_WAKEUP_GPIO_HIGH);  
  // esp_deep_sleep_enable_gpio_wakeup(GPIO_NUM_1, ESP_GPIO_WAKEUP_GPIO_HIGH);
  
  /**
   * Remaining initialization functions
   */
  if(ENABLE_LOGGING)
  {
    Serial.println("^Calling remaining initialization functions");
  }
  
  // main_i2c.init();          
  app_console.init();
  // lan.init();
  // nvm_functions.init();
  app_functions.init();
  
  //Initialize timer interrupt
  //                     The frequency of the timer   
  //                       |     
  timer = timerBegin(1000000);   
  
  //                 Name of timer (from above) 
  //                     |      Name of callback function       
  //                     |        |     true (the tutorial did not indicate what this mans)
  //                     |        |        |     
  timerAttachInterrupt(timer,&onTimer);    
  
  //       This is the timer struct 
  //           |        This is the alarm value (so alarm when we count up to this value)       
  //           |          |    true = to tell the timer to reload 
  //           |          |      |  Value to reload into the timer when auto reloading
  //           |          |      |   |
  timerAlarm(timer, 50000, true,0);   
  
}

/**
 * @brief Arduino main loop
 */
void loop() 
{

  // if(rtc_boot_ctr == 0)   
  // {
  //   Serial.println("====================== Reset ======================");
    
  //   if (ENABLE_LOGGING)
  //   {
  //     Serial.println("^Boot count is 0");
      
  //     main_i2c.temp_offset = nvm_functions.nvm_get_float(pref,PREF_TEMP_OFFSET); 
  //     Serial.print("^Temp offset: ");
  //     Serial.println(main_i2c.temp_offset);
      
  //     main_i2c.rh_offset = nvm_functions.nvm_get_float(pref,PREF_RH_OFFSET);   
  //     Serial.print("^RH offset: ");
  //     Serial.println(main_i2c.rh_offset);
      
  //     Serial.println("^Printing splash screen.");
  //   }
    
  //   app_functions.display_post_message();
  //   app_functions.full_screen_refresh(pref);
  //   app_functions.heartbeat_post();
    
  //   if(main_i2c.batt_sen_sealed()){
  //     if (ENABLE_LOGGING)
  //     {
  //       Serial.println("^Battery sensor is sealed");
  //     }
      // If in here, we'll need to figure out 
      // how to unseal and enter configuration mode
  //   }
    // else
    // {
    //   if (ENABLE_LOGGING)
    //   {
    //     Serial.println("^Battery sensor is not sealed");
    //   }

    // }
    
    // main_i2c.batt_sen_set_capacity(DEFAULT_BAT_CAP);  //Capacity is in mAh

    // rtc_boot_ctr++;
  // }
  
  // else
  // {
  //   rtc_boot_ctr++;
  //   email_send_boot_ctr++;
  //   /* Determine what state we need to be in */
  //   app_functions.state = nvm_functions.nvm_read_byte(pref,PREF_STATE);
    
  //   if (ENABLE_LOGGING)
  //   {
  //     Serial.print("^Boot counter is: ");
  //     Serial.println(rtc_boot_ctr);
      
  //     Serial.print("^Email sender boot counter is: ");
  //     Serial.println(email_send_boot_ctr);
  //   }
    
  // }

  // if(rtc_boot_ctr > SLEEPS_UNTIL_DISP_UPDATE) //Boot counter is one based, so greater than sign here
  // {
  //   app_functions.bool_update_display = true;
  //   rtc_boot_ctr = 1;           //A boot counter value of 0 should be reserved for power cycle resets
  // }
  
  // if(email_send_boot_ctr >= SLEEPS_UNTIL_EMAIL)
  // {
  //   app_functions.bool_send_email = true;
  //   email_send_boot_ctr = 1;           //A boot counter value of 0 should be reserved for power cycle resets
  // }

  
  while (true)
  {

    /**
     * EVERY 50ms
     */
    if(Timer50msFlag == true) 
    {
      
      Timer50msFlag = false;
      rx_char = Serial.read();
      
      if (rx_char == 'z' and !sensor_functions.ms50_timer_enabled)
      {
        if(ENABLE_LOGGING && !sensor_functions.ms50_timer_enabled)
        {
          Serial.println("^User wishes to enter the console");
        }
        app_console.console(app_functions);    
        Timer100msFlag = false;
        Timer500msFlag = false;
        Timer1000msFlag = false;
      }
      
      if(sensor_functions.ms50_timer_enabled)
      {
        sensor_functions.ms50_timer_ticks++;
      }
      else
      {
        sensor_functions.ms50_timer_ticks = 0;
      }

      app_functions.state_handler(app_functions, sensor_functions);
      app_functions.button_handler();
      
    }
    
    /**
     * EVERY 100ms
     */
    if(Timer100msFlag == true) 
    {
      Timer100msFlag = false;
    }
    
    /**
     * EVERY 500ms
     */
    if(Timer500msFlag == true) 
    {
      Timer500msFlag == false;
      digitalWrite(HEALTH_LED, !digitalRead(HEALTH_LED));
    }
    
    /**
     * EVER SECOND
     */
    if(Timer1000msFlag == true) 
    {
      Timer1000msFlag = false;
      
      sensor_functions.measure_distance_flag = true;
      app_functions.seconds_counter++;
      
      if(app_functions.btn_interrupt_triggered && !digitalRead(BUTTON_INPUT) &&
      !app_functions.btn_short_press_flag && !app_functions.btn_long_press_flag)
      {
        attachInterrupt(BUTTON_INPUT, button_press, FALLING); //Disabled in the ISR, so reenable
        app_functions.btn_interrupt_triggered  = false;
      }

      /* The following is nice for longer delays */
      if(app_functions.seconds_counter >= 30)  
      {
        app_functions.seconds_counter = 0;

      }
    } /* IF Timer1000msFlag */
  } /* WHILE (TRUE) */

} /* MAIN ARDUINO LOOP */
