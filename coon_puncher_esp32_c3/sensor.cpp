#include "sensor.h"
#include "app.h"


SENSOR  sensor_functions;


void SENSOR::init(void) 
{

}

void SENSOR::get_distance( void ) 
{
  
  // Send 10 us trigger pulse
  // digitalWrite(TRIG_PIN, LOW);  //TODO I think we can remove these two lines
  // delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse width, timeout 30 ms
  // 30 ms ~= 5 meters max distance
  unsigned long duration_us = pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration_us == 0) {
    this -> current_distance = -1;   // timeout / no echo
  }

  // Speed of sound: ~343 m/s
  // distance cm = time_us * 0.0343 / 2
  this -> current_distance = (duration_us * 0.0343f / 2.0f);
}

