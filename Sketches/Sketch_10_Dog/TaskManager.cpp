/**
 * @file TaskManager.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Task Manager.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "TaskManager.h"

TaskHandle_t taskHandle_Command_Service = NULL;
TaskHandle_t taskHandle_Motion_Service = NULL;
TaskHandle_t taskHandle_Power_Listener = NULL;
TaskHandle_t taskHandle_Show_Leds = NULL;
TaskHandle_t taskHandle_Buzzered = NULL;
TaskHandle_t taskHandle_Camera = NULL;
TaskHandle_t taskHandle_AutoWalking = NULL;
TaskHandle_t taskHandle_Secondary = NULL;
TaskHandle_t taskHandle_Touch = NULL;

void startTask(uint8_t t)
{
    switch (t)
    {
    case TASK_SECONDRAY:
        xTaskCreateUniversal(loopSecondary, TSK_NAME_SECONDARY, 8192, NULL, 1, &taskHandle_Secondary, 1);
        break;
    case TASK_COMMAND_SERVICE:
        xTaskCreateUniversal(task_CommandService, TSK_NAME_MOTION_SERVICE, 8192, NULL, 1, &taskHandle_Command_Service, 1); // task_MotionService uses core 1.
        break;
    case TASK_MOTION_SERVICE:
        xTaskCreateUniversal(task_MotionService, TSK_NAME_MOTION_SERVICE, 8192, NULL, 1, &taskHandle_Motion_Service, 1); // task_MotionService uses core 1.
        break;
    case TASK_BATTERY_POWER_LISTRNER:
        // xTaskCreateUniversal(task_BatteryPowerListener, TSK_NAME_BATTERY_LISTENER, 8192, NULL, 1, &taskHandle_Power_Listener, 1);
        break;
    case TASK_SHOW_LEDS:
        xTaskCreateUniversal(task_RGBLeds, TSK_NAME_SHOW_LEDS, 1024, NULL, 1, &taskHandle_Show_Leds, 1);
        break;
	case TASK_TOUCH:
        xTaskCreateUniversal(task_Touch, TSK_NAME_TOUCH, 1024, NULL, 1, &taskHandle_Touch, 1);
        break;
    case TASK_BUZZERED:
        xTaskCreateUniversal(task_Buzzered, TSK_NAME_BUZZERED, 2048, NULL, 1, &taskHandle_Buzzered, 0);
        break;
    case TASK_CAMERA_SERVICE:
        // xTaskCreateUniversal(task_CameraService, TSK_NAME_CAMERA_SERVICE, 8192, NULL, configMAX_PRIORITIES, &taskHandle_Camera, 1);
        xTaskCreateUniversal(task_CameraService, TSK_NAME_CAMERA_SERVICE, 8192, NULL, 1, &taskHandle_Camera, 0);
        break;
    case TASK_AUTO_WALKING:
        // xTaskCreateUniversal(task_AutoWalking, TSK_NAME_AUTO_WALKING, 8192, NULL, 1, &taskHandle_AutoWalking, tskNO_AFFINITY);
        // xTaskCreateUniversal(task_AutoWalking, TSK_NAME_AUTO_WALKING, 8192, NULL, 1, &taskHandle_AutoWalking, 0);
        break;
    default:
        break;
    }
    vTaskDelay(10);
}

eTaskState getTaskState(TaskHandle_t taskHandle)
{
    if (taskHandle != NULL)
    {
        return eTaskGetState(taskHandle);
    }
    else
    {
        return eInvalid;
    }
}

void controlTask(uint8_t task, uint8_t act)
{
    TaskHandle_t handle = NULL;
    switch (task)
    {
    case TASK_COMMAND_SERVICE:
        handle = taskHandle_Command_Service;
        break;
    case TASK_MOTION_SERVICE:
        handle = taskHandle_Motion_Service;
        break;
    case TASK_BATTERY_POWER_LISTRNER:
        handle = taskHandle_Power_Listener;
        break;
    case TASK_SHOW_LEDS:
        handle = taskHandle_Show_Leds;
        break;
    case TASK_BUZZERED:
        handle = taskHandle_Buzzered;
        break;
    case TASK_CAMERA_SERVICE:
        handle = taskHandle_Camera;
        break;
    case TASK_AUTO_WALKING:
        handle = taskHandle_AutoWalking;
        break;
    default:
        break;
    }
    if (handle != NULL)
    {
        switch (act)
        {
        case TASK_SUSPEND:
            if (eTaskGetState(handle) != eSuspended)
            {
                vTaskSuspend(handle);
            }
            break;
        case TASK_RESUME:
            if (eTaskGetState(handle) != eRunning)
            {
                vTaskResume(handle);
            }
            break;
        case TASK_DELETE:
            if (eTaskGetState(handle) != eDeleted)
            {
                vTaskDelete(handle);
            }
            break;
        default:
            break;
        }
    }
}