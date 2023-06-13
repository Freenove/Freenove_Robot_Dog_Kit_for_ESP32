/**
 * @file Sketch_03_Buzzer.ino
 * @author suhayl@freenove (support@freenove.com)
 * @brief Buzzer service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */
#include "Buzzer.h"

void setup() {
  setupBuzzer();
  for (int i = 0; i < 3; i++) {
    setBuzzer(1000);
    delay(100);
    setBuzzer(0);
    delay(100);
  }
  delay(3000);
  setMelodyToPlay(MELODY_POWER_UP);
}

void loop() {
  delay(1000);
}