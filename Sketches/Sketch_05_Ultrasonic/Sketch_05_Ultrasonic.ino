/**
 * @file Sketch_05_Ultrasonic.ino
 * @author ZhentaoLin@freenove (support@freenove.com)
 * @brief Ultrasonic ranging driver and serive.
 * @version Arduino esp32 version v3.3.12
 * @date 2026-11-14
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */
#include "UltrasonicRanging.h"

void setup() {
  Serial.begin(115200);
  setupSonar();
}

void loop() {
  float dist = getSonar();
  Serial.printf("dist: %.2fcm\r\n", dist);
  delay(500);
}