/**
 * @file main.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Program entry.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "Public.h"

DataQueue<String> mqMotion(2);  //	Motion message queue, When there are too many messages, some messages can be ignored to reduce latency.
DataQueue<String> mqInfo(100);  //  Info message queue, Important, can not ignore.

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nProgram begin ... ");
  bleSetup();
  prefs.begin(NMSPC_STORAGE);
  pca.begin();
  pca.releaseAllServo();
  getServoOffsetFromStorage();
  getLedConfigFromStorage();
  cs.begin();
  setupAdc();
  setupBuzzer();
  setupSonar();
  setupRGBLED();
  setupBuiltInLed();
  setupTouchPad();
  Serial.println("setup finished!\n\n");

  startTask(TASK_MOTION_SERVICE);
  startTask(TASK_SECONDRAY);  // loop2

  if (!cs.isCameraNormal) {
    setMelodyToQueue(MELODY_CAM_FAILURE);
  }
  setMelodyToQueue(MELODY_POWER_UP);
  standUp();
}

void loop() {
  task_CommandService(NULL);
  task_BleUploadService(NULL);
  task_showBuiltInLed(NULL);
  vTaskDelay(20);
  static uint32_t lastT = 0;
  if (millis() - lastT > 1000) {
    // Serial.printf("Total heap: %d\r\n", ESP.getHeapSize());
    // Serial.printf("Free heap: %d\r\n", ESP.getFreeHeap());
    lastT = millis();
  }
}

void loopSecondary(void *pvParameters) {
  while (1) {
    task_BatteryPowerListener(NULL);
    task_showRGBLeds(NULL);
    task_AutoWalking(NULL);
    task_BuzzerService(NULL);

    task_TouchPad(NULL);
    vTaskDelay(50);
  }
  vTaskDelete(xTaskGetCurrentTaskHandle());
}

void serialEventRun() {
  static String serialInputString = "";
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    serialInputString += inChar;
    if (inChar == '\n') {
      enterMessageQueue(serialInputString);
      serialInputString = "";
    }
  }
}

void onBleReceived(BLECharacteristic *pCharacteristic) {
  std::string rxValue = pCharacteristic->getValue();
  static String bleInputString = "";
  if (rxValue.length() > 0) {
    for (int i = 0; i < rxValue.length(); i++) {
      bleInputString += rxValue[i];
      if (rxValue[i] == '\n') {
        enterMessageQueue(bleInputString);
        bleInputString = "";
      }
    }
  }
}

void onWiFiCmdReceived(WiFiClient *client) {
  static String wifiInputString = "";
  while (client->available()) {
    char rv[1024];
    int ret = client->read((uint8_t *)rv, sizeof(rv));
    for (int i = 0; i < ret; i++) {
      wifiInputString += rv[i];
      if (rv[i] == '\n') {
        Serial.print(wifiInputString);
        enterMessageQueue(wifiInputString);
        wifiInputString = "";
      }
    }
  }
}

void onWiFiCmdTrasmit(WiFiClient *client) {
  if (!mqTx.isEmpty()) {
    client->write(mqTx.out().c_str());
  }
}

void enterMessageQueue(String msg) {
  Serial.print("msg : ");
  Serial.print(msg);
  switch (msg.charAt(0)) {
    case ACTION_INSTALLATION:
    case ACTION_CALIBRATE:
    case ACTION_UP_DOWN:
    case ACTION_BODY_HEIGHT:
    case ACTION_MOVE_ANY:
    case ACTION_TWIST:
    case ACTION_DANCING:
      mqMotion.enterForced(msg);
      controlTask(TASK_MOTION_SERVICE, TASK_RESUME);
      break;
    default:
      mqInfo.enterForced(msg);
      break;
  }
}
