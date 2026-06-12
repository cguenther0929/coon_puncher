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
 * TODO do we need a routine for resetting the trap
 */

#include <Arduino.h>
#include <stdio.h>
#include <esp_timer.h>
#include "console.h"
#include "app.h"


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

char            rx_char               = '\n';

/**
 * Time structure 
 */
hw_timer_t *timer = NULL;     

/**
 * Define class instances
 */
CONSOLE app_console;
APP     ino_app_functions;
SENSOR  ino_sensor_functions;

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
   * The button handler (which is called 
   * outside of this interrupt) will handle
   * this flag.  Also, the interrupt is 
   * disabled so we don't keep jumping
   * down into this routine.  The interrupt 
   * is reattached up in the main while loop
   * only after button actions have been properly 
   * handled.
   */
  ino_app_functions.btn_interrupt_triggered  = true;
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
  
  pinMode(BUTTON_INPUT,INPUT);
  

  pinMode(HBR_IN1, OUTPUT_OPEN_DRAIN);
  digitalWrite(HBR_IN1, LOW);
  
  pinMode(HBR_IN2, OUTPUT_OPEN_DRAIN);
  digitalWrite(HBR_IN2, LOW);


  
  /**
   * @brief Define IO interrupt for push button input 
   */
  //           IO Pin number that shall trigger the interrupt                              
  //                      |         Name of the callback function               
  //                      |               |     Type of signal edge to detect    
  //                      |               |         |
  attachInterrupt(BUTTON_INPUT, button_press, FALLING); 

  /**
   * Remaining initialization functions
   */
  if(ENABLE_LOGGING)
  {
    Serial.println("^Calling remaining initialization functions");
  }
  
  app_console.init();
  ino_app_functions.init();
  ino_sensor_functions.init();
  
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

  /**
   * @brief Main super loop
   */
  if(ENABLE_LOGGING)
  {
    Serial.println("^Entering the main loop.");
  }
  while (true)
  {

    /**
     * EVERY 50ms
     */
    if(Timer50msFlag == true) 
    {
      Timer50msFlag = false;
      rx_char = Serial.read();
      
      if (rx_char == 'z' && !ino_app_functions.ms50_timer_enabled)
      {
        if(ENABLE_LOGGING )
        {
          Serial.println("^User wishes to enter the console");
        }
        app_console.user_console(ino_app_functions);    
        Timer100msFlag = false;
        Timer500msFlag = false;
        Timer1000msFlag = false;
      }
      
      if(ino_app_functions.ms50_timer_enabled)
      {
        ino_app_functions.ms50_timer_ticks++;
      }

      else
      {
        ino_app_functions.ms50_timer_ticks = 0;
      }

      ino_app_functions.button_handler();
      ino_app_functions.state_handler(ino_app_functions, ino_sensor_functions);

      
    }
    
    /**
     * EVERY 100ms
     */
    if(Timer100msFlag == true) 
    {
      Timer100msFlag = false;
      ino_sensor_functions.measure_distance_flag = true;
    }
    
    /**
     * EVERY 500ms
     */
    if(Timer500msFlag == true) 
    {
      Timer500msFlag = false;
      if(ino_app_functions.enable_led && ino_app_functions.fast_blink)
      {
        digitalWrite(HEALTH_LED, !digitalRead(HEALTH_LED));
      }

      
    }
    /**
     * EVERY SECOND
     */
    if(Timer1000msFlag == true) 
    {
      Timer1000msFlag = false;
      
      if(ino_app_functions.enable_led && !ino_app_functions.fast_blink)
      {
        digitalWrite(HEALTH_LED, !digitalRead(HEALTH_LED));
      }

      
      
      ino_app_functions.seconds_counter++;
      
      if(ino_app_functions.btn_interrupt_triggered && digitalRead(BUTTON_INPUT) &&
      !ino_app_functions.btn_short_press_flag && !ino_app_functions.btn_long_press_flag)
      {
        attachInterrupt(BUTTON_INPUT, button_press, FALLING); //Disabled in the ISR, so reenable
        ino_app_functions.btn_interrupt_triggered  = false;
      }

      /* The following is nice for longer delays */
      if(ino_app_functions.seconds_counter >= 30)  
      {
        ino_app_functions.seconds_counter = 0;

      }
    } /* IF Timer1000msFlag */
  } /* WHILE (TRUE) */

} /* MAIN ARDUINO LOOP */
