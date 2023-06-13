/**
 * @file RGBLED_WS2812.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief WS2812 RGB LED driver and service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _RGBLED_WS2812_h
#define _RGBLED_WS2812_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Public.h"
#include "Freenove_WS2812_Lib_for_ESP32.h"

#define LEDS_COUNT  				4
#define LEDS_PIN					PIN_RGBLED
#define CHANNEL						0

#define LED_MODE_OFF 				0
#define LED_MODE_RGB 				1
#define LED_MODE_FOLLOWING 			2
#define LED_MODE_BLINK 				3
#define LED_MODE_BREATHING 			4
#define LED_MODE_RAINBOW 			5

void saveLedConfigToStorage();
void getLedConfigFromStorage();
void setupRGBLED();
void setRGBLED(uint8_t mode, uint8_t r, uint8_t g, uint8_t b);

void task_showRGBLeds(void *pvParameters);
void task_RGBLeds(void *pvParameters);

#endif

