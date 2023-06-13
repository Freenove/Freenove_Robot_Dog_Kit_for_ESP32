/**
 * @file BatteryPower.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Read, monitor and upload battery power.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _BATTERYVOLTAGE_h
#define _BATTERYVOLTAGE_h


#include "Public.h"

#define READ_BAT_VOL_INTERVAL_TIME						200
#define UPLOAD_VOL_TIME 								3000
#define LOW_POWER_WARNING_INTERVAL_TIME					5000
#define LOW_POWER_WARNING_COUNT_MAX_BEFORE_UNLOAD		10
#define VOLATAGE_RATIO				4		//Resistor voltage ratio

//unit: mV
#define BAT_VOL_PCT_0				6000		// Voltage when low power.
#define BAT_VOL_PCT_100				8300		// Voltage when full power.
#define BAT_VOL_UNLOAD				5900		//Stop powering servo under this voltage.

void setupAdc();
uint32_t getBatteryVoltage();

extern bool isRobotMoving;
void bleUploadBatteryInfo();
void task_BatteryPowerListener(void *pvParameters);

#endif

