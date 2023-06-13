/**
 * @file TaskManager.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Task Manager.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _TASKMANAGER_h
#define _TASKMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Public.h"

// CONFIG_ARDUINO_RUNNING_CORE : 1
// core 1 : loop()，task_CommandService
// core 1 : task_BatteryPowerListener
// core 1 : task_showRGBLeds
// core 1 : task_MotionService
// core 1 : task_CommandService
// core 1 : task_CmdService

// Core 0:
// core 0 : task_CameraService

#define TASK_COMMAND_SERVICE		0
#define TASK_MOTION_SERVICE			1
#define TASK_BATTERY_POWER_LISTRNER 2
#define TASK_SHOW_LEDS 				3
#define TASK_BUZZERED 				4
#define TASK_CAMERA_SERVICE         5
#define TASK_AUTO_WALKING           6
#define TASK_SECONDRAY              7
#define TASK_TOUCH                  8

//CONFIG_FREERTOS_MAX_TASK_NAME_LEN 16
#define TSK_NAME_COMMAND_SERVICE  "CMD_SVC"
#define TSK_NAME_MOTION_SERVICE	  "MTN_SVC"
#define TSK_NAME_BATTERY_LISTENER "BAT_LIS"
#define TSK_NAME_SHOW_LEDS		  "RGB_LED"
#define TSK_NAME_BUZZERED		  "BZD_SVC"
#define TSK_NAME_CAMERA_SERVICE	  "CAM_SVC"
#define TSK_NAME_AUTO_WALKING	  "AUT_WK"
#define TSK_NAME_SECONDARY	      "LOOP2"
#define TSK_NAME_TOUCH            "TOUCH"


#define TASK_SUSPEND                1
#define TASK_RESUME                 2
#define TASK_DELETE                 3

void startTask(uint8_t t);
//void task_SuspendOrResume(int t, bool b);
void controlTask(uint8_t task, uint8_t act);

eTaskState getTaskState(TaskHandle_t taskHandle);

extern void task_CommandService(void *pvParameters);
extern void task_MotionService(void *pvParameters);
extern void task_CameraService(void *pvParameters);
extern void task_AutoWalking(void *pvParameters);
extern void loopSecondary(void *pvParameters);


#endif
