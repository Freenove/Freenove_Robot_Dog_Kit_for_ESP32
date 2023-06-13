/**
 * @file Public.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Public resource concentration statement
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _PUBLIC_h
#define _PUBLIC_h
#include <Arduino.h>
#include "esp_pm.h"
#include "esp32/pm.h"
#include <pthread.h>
#include "esp32/spiram.h"
#include "esp32/himem.h"

#include "esp_wifi.h"
#include "esp_adc_cal.h"
#include "RobotDefinitions.h"
#include "BatteryPower.h"
#include "TouchPad.h"
#include "TaskManager.h"
#include "TouchPad.h"
#include "Buzzer.h"
#include "MessageParser.h"
#include "RobotDefinitions.h"
#include "PreferencesPro.h"
#include "CameraService.h"
#include "MassageQueue.h"
#include "BluetoothService.h"
#include "Freenove_PCA9685.h"
#include "BluetoothOrders.h"
#include <EEPROM.h>

#include "UltrasonicRanging.h"
#include "RGBLED_WS2812.h"
#include "BuiltInLed.h"

#include "DanceMovements.h"
#include "Motion.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define square(x) pow(x, 2)

extern Freenove_PCA9685 pca;

extern DataQueue<String> mqMotion;  //	Motion message queue, When there are too many messages, some messages can be ignored to reduce latency.
extern DataQueue<String> mqInfo;    //  Info message queue, Important, can not ignore.
extern DataQueue<String> mqTx;      // Ble message
extern DataQueue<int> mqBz;         // Buzzer queue

extern float calibratePosition[4][3];
extern float lastPt[4][3];
extern float las[4][3];
extern float servoOffset[4][3];
extern bool isRobotStanding;

extern TaskHandle_t taskHandle_Command_Service;
extern TaskHandle_t taskHandle_Motion_Service;
extern TaskHandle_t taskHandle_Power_Listener;
extern TaskHandle_t taskHandle_Show_Leds;
extern TaskHandle_t taskHandle_Buzzered;
extern TaskHandle_t taskHandle_Camera;
extern TaskHandle_t taskHandle_AutoWalking;
extern TaskHandle_t taskHandle_Secondary;

extern PreferencesPro prefs;
extern pthread_mutex_t mutexPin33;

extern void enterMessageQueue(String msg);

#endif
