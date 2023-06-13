/**
 * @file Sketch_04_WS2812.ino
 * @author suhayl@freenove (support@freenove.com)
 * @brief WS2812 RGB LED driver and service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */
#include "RGBLED_WS2812.h"

u8 m_color[5][3] = { { 255, 0, 0 }, { 0, 255, 0 }, { 0, 0, 255 }, { 255, 255, 255 }, { 0, 0, 0 } };

void setup() {
  setupRGBLED();
}

void loop() {
  for (int i = 1; i < 6; i++) {
    if (i == 1) {
      for (int j = 0; j < 5; j++) {
        setRGBLED(i, m_color[j][0], m_color[j][1], m_color[j][2]);
        delay(500);
      }
    } else {
      setRGBLED(i, 100, 15, 100);
      delay(5000);
    }
  }
}