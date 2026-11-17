/**
 * @file Sketch_06_Touch.ino
 * @author ZhentaoLin@freenove (support@freenove.com)
 * @brief Touchpad driver and service.
 * @version Arduino esp32 version v3.3.12
 * @date 2026-11-14
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */
#include "TouchPad.h"

void setup() {
  Serial.begin(115200);
  setupTouchPad();
}

void loop() {
  int isTouch = getTouch();
  Serial.printf("Touch state: %d\r\n", isTouch);
  delay(100);
}
