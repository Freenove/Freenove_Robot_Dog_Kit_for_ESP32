/**
 * @file BatteryPower.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Read, monitor and upload battery power.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "BatteryPower.h"

void setupAdc()
{
    analogReadResolution(12);     
    analogSetAttenuation(ADC_11db);
    adcAttachPin(PIN_ANALOG_IN);   
}

uint32_t getBatteryVoltage()
{
    uint32_t adc_reading = 0;         
    for (int i = 0; i < NUM_OF_SAMPLES; i++)
    {
        adc_reading += analogRead(PIN_ANALOG_IN);
    }
    adc_reading /= NUM_OF_SAMPLES;               

    uint32_t voltage = adc_reading*3300/4096;              
    return voltage * VOLATAGE_RATIO;             
}
