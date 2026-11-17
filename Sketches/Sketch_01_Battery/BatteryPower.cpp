#include "BatteryPower.h"

void setupAdc()
{
    analogReadResolution(12);     
    analogSetPinAttenuation(PIN_ANALOG_IN, ADC_11db);   
}

uint32_t getBatteryVoltage()
{
    uint32_t adc_reading = 0;         
    for (int i = 0; i < NUM_OF_SAMPLES; i++)
    {
        adc_reading += analogReadMilliVolts(PIN_ANALOG_IN);
    }
    adc_reading /= NUM_OF_SAMPLES;               

    //uint32_t voltage = adc_reading*3300/4096;              
    uint32_t voltage = adc_reading;    
    return voltage * VOLATAGE_RATIO;             
}
