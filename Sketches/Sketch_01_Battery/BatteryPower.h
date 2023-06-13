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

#include "Arduino.h"

#define VOLATAGE_RATIO   4     //Resistance partial voltage ratio
#define PIN_ANALOG_IN    32    //ESP32 GPIO number
#define NUM_OF_SAMPLES   64    //ADC sample times


void setupAdc();
uint32_t getBatteryVoltage();

#endif
