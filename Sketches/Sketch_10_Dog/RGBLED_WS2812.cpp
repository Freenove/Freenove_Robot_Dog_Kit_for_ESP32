/**
 * @file RGBLED_WS2812.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief WS2812 RGB LED driver and service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "RGBLED_WS2812.h"

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

struct LedConfig
{
    uint8_t mode;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ledConfig_t;

void saveLedConfigToStorage()
{
    // Serial.printf("put ledConfig_t : %d, %d, %d, %d\n", ledConfig_t.mode, ledConfig_t.r, ledConfig_t.g, ledConfig_t.b);
    prefs.put(KEY_LED_MODE, ledConfig_t);
}

void getLedConfigFromStorage()
{
    if (prefs.getBytesLength(KEY_LED_MODE) == sizeof(ledConfig_t))
    // if (prefs.isKey(KEY_LED_MODE))
    {
        prefs.get(KEY_LED_MODE, ledConfig_t);
        Serial.printf("get ledConfig_t : %d, %d, %d, %d\n", ledConfig_t.mode, ledConfig_t.r, ledConfig_t.g, ledConfig_t.b);
    }
    else
    {
        prefs.put(KEY_LED_MODE, ledConfig_t);
        Serial.println("new borad! init ledConfig_t .");
    }
}

int followingColorStep = 0;
int breathingStep = 2;
int rainbowStep = 2;
bool blinkStateUp = true;
bool breathingStateUp = true;
u32 lastStripUpdateTime = 0;

void setupRGBLED()
{
    strip.begin();
    strip.setBrightness(255);
}
void setRGBLED(uint8_t mode, uint8_t r, uint8_t g, uint8_t b)
{
    ledConfig_t.mode = mode;
    ledConfig_t.r = r;
    ledConfig_t.g = g;
    ledConfig_t.b = b;
}

void task_showRGBLeds(void *pvParameters)
{
    // Serial.print("task_showRGBLeds() function running on core: ");
    // Serial.println(xPortGetCoreID());

    // while (1)
    {
        // Serial.printf("ledConfig_t.mode: %d,%d,%d,%d\n", ledConfig_t.mode, ledConfig_t.r, ledConfig_t.g, ledConfig_t.b);
        // vTaskDelay(20);
        switch (ledConfig_t.mode)
        {
        case LED_MODE_OFF:
            if (millis() - lastStripUpdateTime > 500)
            {
                for (int i = 0; i < LEDS_COUNT; i++)
                {
                    strip.setLedColorData(i, 0, 0, 0);
                }
                strip.show();
                lastStripUpdateTime = millis();
            }
            break;
        case LED_MODE_RGB:
            if (millis() - lastStripUpdateTime > 10)
            {
                for (int i = 0; i < LEDS_COUNT; i++)
                {
                    strip.setLedColorData(i, ledConfig_t.r, ledConfig_t.g, ledConfig_t.b);
                }
                strip.show();
                lastStripUpdateTime = millis();
            }
            break;
        case LED_MODE_FOLLOWING:
            if (millis() - lastStripUpdateTime > 100)
            {
                followingColorStep += 5;
                int j = followingColorStep;
                strip.setLedColor(j % 4, strip.Wheel((j / 4 * 86) & 255));
                lastStripUpdateTime = millis();
            }
            break;
        case LED_MODE_BLINK:
            if (millis() - lastStripUpdateTime > 500)
            {
                if (blinkStateUp)
                {
                    blinkStateUp = false;
                    for (int i = 0; i < LEDS_COUNT; i++)
                    {
                        strip.setLedColorData(i, ledConfig_t.r, ledConfig_t.g, ledConfig_t.b);
                    }
                    strip.show();
                }
                else
                {
                    blinkStateUp = true;
                    for (int i = 0; i < LEDS_COUNT; i++)
                    {
                        strip.setLedColorData(i, 0, 0, 0);
                    }
                    strip.show();
                }
                lastStripUpdateTime = millis();
            }
            break;
        case LED_MODE_BREATHING:
            if (millis() - lastStripUpdateTime > 10)
            {
                if (breathingStateUp)
                {
                    breathingStep += 10;
                    if (breathingStep >= 255)
                    {
                        breathingStep = 255;
                        breathingStateUp = false;
                    }
                }
                else
                {
                    breathingStep -= 10;
                    if (breathingStep <= 0)
                    {
                        breathingStep = 0;
                        breathingStateUp = true;
                    }
                }
                int j = breathingStep;
                strip.setBrightness(j);
                for (int i = 0; i < LEDS_COUNT; i++)
                {
                    strip.setLedColorData(i, ledConfig_t.r, ledConfig_t.g, ledConfig_t.b);
                }
                strip.show();
                lastStripUpdateTime = millis();
            }
            break;
        case LED_MODE_RAINBOW:
            if (millis() - lastStripUpdateTime > 10)
            {
                rainbowStep += 2;
                if (rainbowStep >= 255)
                {
                    rainbowStep = 0;
                }
                int j = rainbowStep;
                for (int i = 0; i < LEDS_COUNT; i++)
                {
                    strip.setLedColorData(i, strip.Wheel((i * 256 / LEDS_COUNT + j) & 255));
                }
                strip.show();
                lastStripUpdateTime = millis();
            }
            break;

        default:
            break;
        }
    }
}

void task_RGBLeds(void *pvParameters)
{
    while(1)
    {
        task_showRGBLeds(NULL);
    }
}