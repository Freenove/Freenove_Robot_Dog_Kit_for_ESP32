/**
 * @file Sketch_01_Battery.ino
 * @author suhayl@freenove (support@freenove.com)
 * @brief Read, monitor and upload battery power.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */
#include "BatteryPower.h"

void setup()
{
  Serial.begin(115200);
  setupAdc();
}

void loop()
{
  float voltage = getBatteryVoltage() / 1000.0;
  Serial.printf("The battery voltage is %fV\r\n", voltage);
  delay(1000);
}