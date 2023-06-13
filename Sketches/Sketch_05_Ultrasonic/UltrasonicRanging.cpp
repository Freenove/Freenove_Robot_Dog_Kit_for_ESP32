/**
 * @file UltrasonicRanging.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Ultrasonic ranging driver and serive.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "UltrasonicRanging.h"

constexpr int MAX_DISTANCE = 300;             // Maximum sensor distance is rated at 400-500cm.
constexpr float timeOut = MAX_DISTANCE * 60;  // timeOut= 2*MAX_DISTANCE /100 /340 *1000000 = MAX_DISTANCE*58.8
constexpr int soundVelocity = 340;            // define sound speed=340m/s

void setupSonar() {
  pinMode(PIN_TRIG, OUTPUT);  // set trigPin to output mode
  pinMode(PIN_ECHO, INPUT);   // set echoPin to input mode
}
float getSonar() {
  unsigned long pingTime, t0, t1;
  float distance;
  // make PIN_TRIG output high level lasting for 10μs to triger HC_SR04
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  // Wait HC-SR04 returning to the high level and measure out this waitting time
  t0 = micros();
  pingTime = pulseIn(PIN_ECHO, HIGH, timeOut);  // unit: us
  t1 = micros() - t0;
  // calculate the distance according to the time
  if (t1 < timeOut) {
    distance = (float)pingTime * soundVelocity / 2 / 10000;
  } else {
    distance = MAX_DISTANCE;
  }
  return distance;  // return the distance value
}
