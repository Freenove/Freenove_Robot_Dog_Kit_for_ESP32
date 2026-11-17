/**
 * @file Sketch_10_Dog.cpp
 * @author zhentao lin@freenove (support@freenove.com)
 * @brief Program entry.
 * @version v1.2.0
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "Public.h"
#include "esp_heap_caps.h"

DataQueue<String> mqMotion(5);  //	Motion message queue, When there are too many messages, some messages can be ignored to reduce latency.
DataQueue<String> mqInfo(100);  //  Info message queue, Important, can not ignore.

#define RX_LINE_MAX_LEN 128
#define MOTION_STOP_MSG "F#0#0#0#8#\n"

String bleInputString = "";
String wifiInputString = "";

void clearRxLineBuffers() {
  bleInputString = "";
  wifiInputString = "";
}

// The stop command is kept out of the queue until every queued motion has been taken,
// so that stopping is always the last motion action executed.
static volatile bool motionStopPending = false;
static uint32_t motionDropCount = 0;

static bool isStopMotionMsg(String msg) {
  if (msg.charAt(0) != ACTION_MOVE_ANY) {
    return false;
  }
  int p1 = msg.indexOf('#');
  int p2 = msg.indexOf('#', p1 + 1);
  int p3 = msg.indexOf('#', p2 + 1);
  int p4 = msg.indexOf('#', p3 + 1);
  if (p1 < 0 || p2 < 0 || p3 < 0) {
    return false;
  }
  if (p4 < 0) {
    p4 = msg.length();
  }
  return msg.substring(p1 + 1, p2).toInt() == 0 &&
         msg.substring(p2 + 1, p3).toInt() == 0 &&
         msg.substring(p3 + 1, p4).toInt() == 0;
}

static void enqueueMotion(String msg) {
  if (mqMotion.isFull()) {
    String dropped = mqMotion.out();
    motionDropCount++;
    Serial.print("mqMotion full, drop oldest: ");
    Serial.print(dropped);
    Serial.printf("drop count: %u\r\n", (unsigned)motionDropCount);
  }
  mqMotion.enter(msg);
  controlTask(TASK_MOTION_SERVICE, TASK_RESUME);
}

void flushPendingMotionStop() {
  if (motionStopPending && mqMotion.isEmpty()) {
    motionStopPending = false;
    Serial.println("motion stop enqueue");
    enqueueMotion(String(MOTION_STOP_MSG));
  }
}

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
  flushPendingMotionStop();
  vTaskDelay(20);
  /*
  static uint32_t lastT = 0;
  if (millis() - lastT > 5000) {
    lastT = millis();
    Serial.printf("[HEAP] int:%u/%u psram:%u/%u\r\n",
                  (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                  (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL),
                  (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
                  (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
  }
  */
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
  String rxValue = pCharacteristic->getValue().c_str();
  if (rxValue.length() > 0) {
    for (int i = 0; i < rxValue.length(); i++) {
      bleInputString += rxValue[i];
      if (rxValue[i] == '\n') {
        enterMessageQueue(bleInputString);
        bleInputString = "";
      }
    }
    if (bleInputString.length() > RX_LINE_MAX_LEN) {
      Serial.println("ble rx overflow, buffer reset");
      bleInputString = "";
    }
  }
}

void onWiFiCmdReceived(WiFiClient *client) {
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
    if (wifiInputString.length() > RX_LINE_MAX_LEN) {
      Serial.println("wifi rx overflow, buffer reset");
      wifiInputString = "";
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
    case ACTION_TWIST:
    case ACTION_DANCING:
      enqueueMotion(msg);
      break;
    case ACTION_MOVE_ANY:
      if (isStopMotionMsg(msg)) {
        motionStopPending = true;
        flushPendingMotionStop();
      } else {
        enqueueMotion(msg);
      }
      break;
    default:
      mqInfo.enterForced(msg);
      break;
  }
}
