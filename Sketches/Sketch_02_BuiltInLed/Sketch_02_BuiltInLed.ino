/**
 * @file Sketch_02_BuiltInLed.ino
 * @author suhayl@freenove (support@freenove.com)
 * @brief Built in led service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "BuiltInLed.h"

void setup() {
  setupBuiltInLed();
  for (int i = 0; i < 5; i++) {
    setBuiltInLed(HIGH);
    delay(1000);
    setBuiltInLed(LOW);
    delay(1000);
  }
}

void loop() {
  task_showBuiltInLed(NULL);
}
