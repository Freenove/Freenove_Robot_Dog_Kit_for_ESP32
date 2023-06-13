/**
 * @file TaskMotionService.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Motion command execution server.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _TASKMOTIONSERVICE_h
#define _TASKMOTIONSERVICE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Public.h"


void task_MotionService(void *pvParameters);

#endif

