/**
 * @file Sketch_02_BuiltInLed.ino
 * @author ZhentaoLin@freenove (support@freenove.com)
 * @brief Built in led service.
 * @version Arduino esp32 version v3.3.12
 * @date 2026-11-14
 * 
 * @copyright Copyright (c) 2026. Freenove corporation.
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
