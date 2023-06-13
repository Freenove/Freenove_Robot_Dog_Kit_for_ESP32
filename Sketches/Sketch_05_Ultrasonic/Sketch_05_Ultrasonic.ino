/**
 * @file Sketch_05_Ultrasonic.ino
 * @author suhayl@freenove (support@freenove.com)
 * @brief Ultrasonic ranging driver and serive.
 * @version v1.0.0
 * @date 2022-04-13
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