#include "app.h"

/**
 * Instantiate class instances
*/
APP     app_functions;
SENSOR  app_sensor_functions;

void APP::init(void) 
{
  this -> state = STATE_WAIT_FOR_CAL; 
}

/**
 * These are the states
 *   STATE_WAIT_FOR_CAL,
 *   STATE_SLEEP,
 *   STATE_READ_DISTANCE,
 *   STATE_EVALUATE_DISTANCE,
 *   STATE_CALIBRATE,
 *   STATE_RESET_TRAP
 */
void APP::state_handler( APP & app_functions, SENSOR & app_sensor_functions ) 
{
  switch(app_functions.state) 
  {
    
    case STATE_WAIT_FOR_CAL:
    
      if(ENABLE_LOGGING)
      {
        Serial.println("^**WAITING FOR CALIBRATION**");
        delay(10);
      }
      if(app_sensor_functions.sensor_is_calibrated)
      {
        this -> state = STATE_READ_DISTANCE;
        app_functions.enable_led = false;
      }
    break;
    
    case STATE_SLEEP:
      if(ENABLE_LOGGING)
      {
        Serial.println("^In state **SLEEP**");
        delay(10);
      }

      if(app_sensor_functions.measure_distance_flag &&
         !app_functions.trap_has_triggered)
      {
        app_sensor_functions.measure_distance_flag = false;
        this -> state = STATE_READ_DISTANCE;
      }
    break;
    
    case STATE_READ_DISTANCE:
      if(ENABLE_LOGGING)
      {
        Serial.println("^In state **READ DISTANCE**");
        delay(10);
      }
      app_sensor_functions.get_distance();
      this -> state = STATE_EVALUATE_DISTANCE;
      break;
      
    case STATE_EVALUATE_DISTANCE:
      
      if(ENABLE_LOGGING)
      {
        Serial.println("^In state **EVALUATE DISTANCE**");
        delay(10);
      }
      if((app_sensor_functions.current_distance < app_sensor_functions.threshold_distance) && 
          app_sensor_functions.sensor_is_calibrated)
        {
          if(ENABLE_LOGGING)
          {
            Serial.println("^Trap is actuating!");
            delay(10);
          }
          app_functions.solenoid_retract();
          app_functions.ms50_timer_enabled = true;
        }
        
      if(ms50_timer_ticks > SOLENOID_TRAVEL_50MS_TICKS_REQUIRED)
      {
        app_functions.ms50_timer_enabled = false;
        app_functions.ms50_timer_ticks = 0;
        app_functions.solenoid_stop();
        app_functions.trap_has_triggered = true;
        app_functions.fast_blink = true;
        app_functions.enable_led = true;
        this -> state = STATE_SLEEP;
      }
      
      if(!app_functions.ms50_timer_enabled)
      {
        this -> state = STATE_SLEEP;
      }

    break;

    case STATE_CALIBRATE:
      if(ENABLE_LOGGING)
      {
        Serial.println("^In state **CALIBRATE**");
        delay(10);
      }      

      app_sensor_functions.get_distance();
      app_sensor_functions.threshold_distance = app_sensor_functions.current_distance * 0.9;
      app_sensor_functions.sensor_is_calibrated  = true;
      app_functions.enable_led = false;
      digitalWrite(HEALTH_LED, HIGH);       //Turn LED OFF

      this -> state = STATE_SLEEP;

    break;
    
    case STATE_RESET_TRAP:
      if(ENABLE_LOGGING)
      {
        Serial.println("^In state **RESET TRAP**");
        delay(10);
      }      
      
      if(!app_functions.ms50_timer_enabled)
      {
        app_functions.solenoid_extend();
        app_functions.ms50_timer_enabled = true;
      }

      if(ms50_timer_ticks > SOLENOID_TRAVEL_50MS_TICKS_REQUIRED)
      {
        app_functions.ms50_timer_enabled = false;
        app_functions.ms50_timer_ticks = 0;
        app_functions.solenoid_stop();
        app_functions.trap_has_triggered = false;
        app_functions.enable_led = false;
        digitalWrite(HEALTH_LED, HIGH);
        this -> state = STATE_SLEEP;
      }
      
      if(!app_functions.ms50_timer_enabled)
      {
        this -> state = STATE_SLEEP;
      }

    break;
    
    default:
      this -> state = STATE_SLEEP;
    break;
  }
  
}

void APP::button_handler ( void )
{


  /**
   * if the button is pushed, simply
   * update the counter
   */
  if(!digitalRead(BUTTON_INPUT) &&
     !btn_short_press_flag && 
     !btn_long_press_flag
    )
    {
      btn_press_ctr++;
    }

    else 
    {
      if(ENABLE_LOGGING && btn_press_ctr > 0)
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
         !btn_short_press_flag && digitalRead(BUTTON_INPUT)
        )
        {
          btn_short_press_flag  = true;
        }
      else if (btn_press_ctr >= LONG_PRESS_50MS_TICKS &&
               !btn_long_press_flag && !btn_short_press_flag &&
               digitalRead(BUTTON_INPUT)
              )
        {
          btn_long_press_flag  = true;
        }
      
      btn_press_ctr = 0;
    }
    
    
  /**
   * Determine what to do based on 
   * press duration flags that were
   * set above
   */
  if(btn_short_press_flag && !btn_long_press_flag) 
  {
    btn_short_press_flag = false;
    this -> state = STATE_RESET_TRAP;
    if(ENABLE_LOGGING)
    {
      Serial.println("^Button short press has been handled");
    }
  }
  
  if(btn_long_press_flag && !btn_short_press_flag) 
  {
    btn_long_press_flag = false;
    this -> state = STATE_CALIBRATE;
    if(ENABLE_LOGGING)
    {
      Serial.println("^Button long press has been handled.");
    }
  }
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
  digitalWrite(HBR_IN2, LOW);
}






