#ifndef _ULTRASONICRANGING_h
#define _ULTRASONICRANGING_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define PIN_TRIG 32  //Ultrasonic trig pin
#define PIN_ECHO 12  //Ultrasonic echo pin

void setupSonar();
float getSonar();

#endif
