/**
 * @file Sketch_03_Buzzer.ino
 * @author ZhentaoLin@freenove (support@freenove.com)
 * @brief Buzzer service.
 * @version Arduino esp32 version v3.3.12
 * @date 2026-11-14
 * 
 * @copyright Copyright (c) 2026. Freenove corporation.
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