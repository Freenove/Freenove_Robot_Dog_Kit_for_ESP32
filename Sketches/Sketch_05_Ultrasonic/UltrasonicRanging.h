/**
 * @file UltrasonicRanging.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Ultrasonic ranging driver and serive.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

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
