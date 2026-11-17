/**
 * @file Sketch_01_Battery.ino
 * @author ZhentaoLin@freenove (support@freenove.com)
 * @brief Read, monitor and upload battery power.
 * @version Arduino esp32 version v3.3.12
 * @date 2026-11-14
 * 
 * @copyright Copyright (c) 2026. Freenove corporation.
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