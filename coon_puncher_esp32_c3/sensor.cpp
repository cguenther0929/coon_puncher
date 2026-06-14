#include "sensor.h"
#include "app.h"


SENSOR  sensor_functions;


void SENSOR::init(void) 
{

}

bool SENSOR::get_distance( void ) 
{
  
  unsigned long duration_us = 0;
  unsigned long previous_us = 0;
  unsigned long temp_distance = 0;
  unsigned long sum = 0;
  uint8_t i = 0;
  
  
  // Measure echo pulse width, timeout 30 ms
  // 30 ms ~= 5 meters max distance
  for(i=0; i < AVERAGE_COUNT; i++)
  {
    
    // Send 10 us trigger pulse
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    duration_us = pulseIn(ECHO_PIN, HIGH, 30000UL);
    if (duration_us > 0)
    {
      sum += duration_us;
      previous_us = duration_us;      //Good distance reading, so add to summation
    }
    else 
    {
      sum += previous_us;               //Just use the pule-time reading 
    }

    if(i > 2 && sum == 0)      //Kick out because something is wrong (i.e. sensor disconnected)
    {

      this -> current_distance = 99.99;   // Something is wrong -- no ECHO
      return false;
    }

  }

  duration_us = sum/AVERAGE_COUNT;

  // Speed of sound: ~343 m/s
  // distance cm = time_us * 0.0343 / 2
  this -> current_distance = (duration_us * 0.0343f / 2.0f);
  return true;

}

