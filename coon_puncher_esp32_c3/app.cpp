#include "app.h"

// extern RTC_DATA_ATTR int rtc_boot_ctr;

/**
 * Instantiate class instances
*/
// I2C     app_i2c;
APP     app_functions;
SENSOR  sensor_functions;
LAN     app_lan;
// NVM     app_nvm;

// char        app_wifi_pass[PREF_BUFF_ELEMENTS]       = {NULL};
// char        app_ssid[PREF_BUFF_ELEMENTS]            = {NULL};


/**
 * Display parameters
 */    
char app_temp_buffer[32]; 

void APP::init(void) 
{
  this -> state = STATE_READ_DISTANCE; 
}

/**
 * These are the states
 *   STATE_SLEEP,
 *   STATE_READ_DISTANCE,
 *   STATE_EVALUATE_DISTANCE,
 *   STATE_CALIBRATE,
 *   STATE_RESET_TRAP
 */
void APP::state_handler( APP & app_functions ) 
{
  switch(app_functions.state) 
  {
    case STATE_SLEEP:
      if(ENABLE_LOGGING)
      {
        Serial.println("^In state **HALT**");
        delay(10);
      }

      if(sensor_functions.measure_distance_flag)
      {
        sensor_functions.measure_distance_flag = false;
        this -> state = STATE_READ_DISTANCE
      }
      
      
    break;
    
    case STATE_READ_DISTANCE
      sensor_functions.get_distance();
      this -> state = STATE_EVALUATE_DISTANCE;
    break;

    case STATE_EVALUATE_DISTANCE

      if((sensor_functions.current_distance < sensor_functions.threshold_distance) && 
          sensor_functions.sensor_is_calibrated)
        {
          app_functions.solenoid_retract();
          sensor_functions.ms50_timer_enabled = true;
        }
        
      if(ms50_timer_ticks > SOLENOID_TRAVEL_50MS_TICKS_REQUIRED)
      {
        sensor_functions.ms50_timer_enabled = false;
        app_functions.solenoid_stop();
      }

      this -> state = STATE_READ_DISTANCE;
    break;

    case STATE_CALIBRATE_TRAP
      
      sensor_functions.get_distance();
      sensor_functions.threshold_distance = sensor_functions.current_distance * 0.9;
      sensor_functions.sensor_is_calibrated  = true;

      this -> state = STATE_SLEEP

    break;
    
    default:
      this -> state = STATE_SLEEP;
      // app_nvm.nvm_store_byte(pref, PREF_STATE, app_functions.state);
    break;
  }
  
}

void APP::button_handler ( void )
{


  // if(digitalRead(BUTTON_INPUT))
  // {
  //   Serial.println("\t***BTN PRESSED");
  //   if(!btn_short_press_flag && !btn_long_press_flag) {
  //     Serial.println("\t***FLAGS ARE CLEARED");
  //   }
  // }


  /**
   * if the button is pushed, simply
   * update the counter
   */
  if(digitalRead(BUTTON_INPUT) &&
     !btn_short_press_flag && 
     !btn_long_press_flag
    )
    {
      btn_press_ctr++;
      // Serial.println("\t***DEBUG INCREMENTING COUNTER");
    }

    else 
    {
      if(
         ENABLE_LOGGING && btn_press_ctr > 5
        )
      {
        Serial.print("^Button counter before clearing: ");
        Serial.println(btn_press_ctr);
      }

      /**
       * Determine if we need to define 
       * a short or long press flag
       * before we clear the counter
       */
      if(btn_press_ctr >= SHORT_PRESS_50MS_TICKS &&
         btn_press_ctr < LONG_PRESS_50MS_TICKS &&
         !btn_short_press_flag && !digitalRead(BUTTON_INPUT)
        )
        {
          btn_short_press_flag  = true;
        }
      else if (btn_press_ctr >= LONG_PRESS_50MS_TICKS &&
               !btn_long_press_flag && !btn_short_press_flag &&
               !digitalRead(BUTTON_INPUT)
              )
        {
          btn_long_press_flag  = true;
        }
      
      btn_press_ctr=0;
    }
    
    
  /**
   * Determine what to do based on 
   * press duration flags that were
   * set above
   */
  if(btn_short_press_flag) 
  {
    btn_short_press_flag = false;
    if(ENABLE_LOGGING)
    {
      Serial.println("^Button short press has been triggered");
    }
    reset_trap = true;
  }
  
  if(btn_long_press_flag) 
  {
    btn_long_press_flag = false;
    app_functions.state = STATE_CALIBRATE_TRAP
    if(ENABLE_LOGGING)
    {
      Serial.println("^Button long press has been triggered.");
    }
  }
}

void APP::reset_trap( void )
{

}

void APP::solenoid_extend( void )
{
  digitalWrite(HBR_IN1, HIGH);
  digitalWrite(HBR_IN2, LOW);
}

void APP::solenoid_retract( void )
{
  digitalWrite(HBR_IN1, LOW);
  digitalWrite(HBR_IN2, HIGH);
}

void APP::solenoid_stop( void )
{
  digitalWrite(HBR_IN1, LOW);
  digitalWrite(HBR_IN2, LOWks);
}


void APP::heartbeat_post( void ) 
{
  digitalWrite(HEALTH_LED , LOW);   
  delay(250);
  digitalWrite(HEALTH_LED , HIGH);   
  delay(250);
  digitalWrite(HEALTH_LED , LOW);   
  delay(250);
  digitalWrite(HEALTH_LED , HIGH);   
  delay(250);
  digitalWrite(HEALTH_LED , LOW);   
  delay(250);
  digitalWrite(HEALTH_LED , HIGH);   
}




