
/**
 * @file BuiltInLed.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Built in led service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "BuiltInLed.h"

uint32_t sqValue = 0b00000101;
uint32_t lastLedUpdateTime = 0;
uint16_t intervalTime = 100; // ms
bool ledStatus = false;
bool lastLedStatus = true;
uint8_t ledRunnigStatus = 0;

void setupBuiltInLed()
{
    pinMode(LED_BUILT_IN, OUTPUT);
}

void setBuiltInLed(bool state)
{
	digitalWrite(LED_BUILT_IN, state);
}

void task_showBuiltInLed(void *pvParameters)
{
    static uint8_t bitIndex = 0;
    static uint8_t bitDiff = 0;
    if (millis() - lastLedUpdateTime > intervalTime)
    {
        // Serial.printf("intervalTime: %ld \n", millis() - lastLedUpdateTime);
        //  vTaskDelay(1);      //Important delay !!!
        switch (ledRunnigStatus)
        {
        case 0: // running
            ledStatus = ((sqValue >> bitIndex) & 0x01) ? true : false;
            if (ledStatus != lastLedStatus)
            {
                // Serial.printf("ledStatus: %d \n", ledStatus);
                //mqTx.enterForced("BLD#" + String(millis() - lastLedUpdateTime) + "#" + String(ledStatus) + "#\n");
                digitalWrite(LED_BUILT_IN, (uint8_t)ledStatus);
                lastLedStatus = ledStatus;
                bitDiff++;
            }
            bitIndex++;
            if (bitIndex >= 10)
            {
                // intervalTime = (10 - bitDiff / 2) * 100;
                intervalTime = 600;
                // Serial.printf("bitDiff: %d, intervalTime: %d \n", bitDiff, intervalTime);
                ledRunnigStatus = 1;
                bitDiff = 0;
                bitIndex = 0;
            }
            break;
        case 1: // off
            intervalTime = 100;
            ledRunnigStatus = 0;
            break;
        default:
            break;
        }
        lastLedUpdateTime = millis();
    }
}