/**
 * @file TaskMotionService.h
 * @author zhentao lin@freenove (support@freenove.com)
 * @brief Motion command execution server.
 * @version v1.2.0
 * @date 2026-11-17
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

