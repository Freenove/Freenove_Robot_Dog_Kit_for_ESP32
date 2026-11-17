/**
 * @file Sketch_08_BLE.ino
 * @author ZhentaoLin@freenove (support@freenove.com)
 * @brief BLE service.
 * @version Arduino esp32 version v3.3.12
 * @date 2026-11-14
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */
#include "BluetoothService.h"

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nProgram begin ... ");
  bleSetup();
}

void loop() {
  String buf = "I#8400#100\nI";
  bleSend(buf);
  delay(3000);
}

void onBleReceived(BLECharacteristic *pCharacteristic) {
  String rxValue = pCharacteristic->getValue();
  static String bleInputString = "";
  Serial.print("onBleReceived: ");
  if (rxValue.length() > 0) {
    for (int i = 0; i < rxValue.length(); i++) {
      bleInputString += rxValue[i];
      if (rxValue[i] == '\n') {
        Serial.println(bleInputString);
        /*You can do something with the information you receive*/

        bleInputString = "";  //Clear bleInputString.
      }
    }
  }
}
