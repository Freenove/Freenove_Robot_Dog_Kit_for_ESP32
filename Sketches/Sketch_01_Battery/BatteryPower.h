#ifndef _BATTERYVOLTAGE_h
#define _BATTERYVOLTAGE_h

#include "Arduino.h"

#define VOLATAGE_RATIO   4     //Resistance partial voltage ratio
#define PIN_ANALOG_IN    32    //ESP32 GPIO number
#define NUM_OF_SAMPLES   64    //ADC sample times


void setupAdc();
uint32_t getBatteryVoltage();

#endif
