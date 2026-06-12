#include "sensor.h"
#include "app.h"


SENSOR  sensor_functions;


void SENSOR::init(void) 
{

}

void SENSOR::get_distance( void ) 
{
  
  // unsigned long avg_array[3];
  unsigned long duration_us = 0;
  unsigned long sum = 0;
  uint8_t i = 0;
  // Send 10 us trigger pulse

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse width, timeout 30 ms
  // 30 ms ~= 5 meters max distance
  for(i=0; i<3; i++)
  {
     sum += pulseIn(ECHO_PIN, HIGH, 30000UL);
  }

  duration_us = sum/3;



  if (sum == 0) {
    this -> current_distance = -1;   // timeout / no echo
  }

  // Speed of sound: ~343 m/s
  // distance cm = time_us * 0.0343 / 2
  this -> current_distance = (duration_us * 0.0343f / 2.0f);
}

