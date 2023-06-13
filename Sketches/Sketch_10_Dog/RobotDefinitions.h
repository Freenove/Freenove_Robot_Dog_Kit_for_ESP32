/**
 * @file RobotDefinitions.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Definition of robot hardware parameters.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _ROBOTDEFINITIONS_h
#define _ROBOTDEFINITIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Public.h"

#define SW_VERSION		"100"			// software version v1.0.0
#define ROBOT_NAME		"FREENOVE-DOG"
#define INTERNAL_CODE	"FNK006201"
#define FREENOVE_STR    "FREENOVE"

//constexpr 
#define PIN_SDA 		13         	//Define SDA pins
#define PIN_SCL 		14         	//Define SCL pins

#define PIN_BUZZER	 	33			//Buzzer pin
#define PIN_BATT		32			//Battery Sampling pin, multiplex
#define PIN_TRIG 		32			//Ultrasonic trig pin
#define PIN_ECHO 		12			//Ultrasonic echo pin

#define PIN_RGBLED 		0			//WS2812 RGB LED Data pin
#define PIN_TOUCH_PAD 	TOUCH_PAD_NUM3		//15 //Touch sensor pin : TOUCH_PAD_NUM3

#define LED_BUILT_IN 	2			//onboard led pin

// The camera pin is defined in the file "CameraService.h"

#define BODY_HEIGHT_DEFAULT	99		//cm
#define BODY_HEIGHT_MIN		55		//cm
#define BODY_HEIGHT_MAX		120		//cm

String getRobotId();


#endif

