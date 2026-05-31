#include "sensor.h"

// extern RTC_DATA_ATTR int rtc_boot_ctr;

// I2C     app_i2c;
APP     app_functions;
LAN     app_lan;
SENSOR  sensor_functions;
// NVM     app_nvm;

// char        app_wifi_pass[PREF_BUFF_ELEMENTS]       = {NULL};
// char        app_ssid[PREF_BUFF_ELEMENTS]            = {NULL};


/**
 * Display parameters
 */    
char app_temp_buffer[32]; 

void SENSOR::init(void) 
{

}

void SENSOR::get_distance( void ) 
{
  
  // Send 10 us trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse width, timeout 30 ms
  // 30 ms ~= 5 meters max distance
  unsigned long duration_us = pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration_us == 0) {
    return -1.0;   // timeout / no echo
  }

  // Speed of sound: ~343 m/s
  // distance cm = time_us * 0.0343 / 2
  this -> current_distance = (duration_us * 0.0343f / 2.0f);
}
  
  
switch(app_functions.state) 
{
  break;
  
  default:
    this -> state = STATE_SLEEP;
    app_nvm.nvm_store_byte(pref, PREF_STATE, app_functions.state);
  break;
}
  



