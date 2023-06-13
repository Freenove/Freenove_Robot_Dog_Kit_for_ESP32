/**
 * @file Sketch_06_Touch.ino
 * @author suhayl@freenove (support@freenove.com)
 * @brief Touchpad driver and service.
 * @version v1.0.0
 * @date 2022-04-13
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
