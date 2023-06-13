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

#include "Public.h"

constexpr int MAX_DISTANCE = 300; // Maximum sensor distance is rated at 400-500cm.
//timeOut= 2*MAX_DISTANCE /100 /340 *1000000 = MAX_DISTANCE*58.8
constexpr float timeOut = MAX_DISTANCE * 60; 
constexpr int soundVelocity = 340; // define sound speed=340m/s

void setupSonar();
float getSonar();

void setEnableAutoWalking(bool b);
void task_AutoWalking(void *pvParameters);


#endif

