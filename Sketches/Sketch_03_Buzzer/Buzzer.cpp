/**
 * @file Buzzer.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Buzzer service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "Buzzer.h"

#define BUZZER_PIN (33)
#define BUZZER_CHN (0)
#define BUZZER_BIT (10)
#define BUZZER_FREQ (5000)

int melody = -1;

void setupBuzzer() {
  ledcSetup(BUZZER_CHN, BUZZER_FREQ, BUZZER_BIT);  // setup pwm channel
  ledcAttachPin(BUZZER_PIN, BUZZER_CHN);
  ledcWrite(BUZZER_CHN, 0);
  ledcWriteTone(BUZZER_CHN, 0);
}

void setBuzzer(uint16_t freq) {
  if (freq != 0) {
    ledcWrite(BUZZER_CHN, 512);
    ledcWriteTone(BUZZER_CHN, freq);
  } else {
    ledcWrite(BUZZER_CHN, 0);
    ledcWriteTone(BUZZER_CHN, 0);
  }
}

template<int N>  //Non-type template parameters
void playMelody(const int (&tune)[N], const float (&beat)[N], float speed) {
  for (int i = 0; i < N; i++) {
    setBuzzer(tune[i]);
    delay(beat[i] * speed);
    setBuzzer(0);
  }
  setBuzzer(0);
}

bool enableBuzzered = false;
TaskHandle_t taskHandle_Buzzered;

void setMelodyToPlay(int m) {
  melody = m;
  enableBuzzered = true;
  xTaskCreateUniversal(task_Buzzered, "task_Buzzered", 2048, NULL, 1, &taskHandle_Buzzered, 0);
}

void task_Buzzered(void *pvParameters) {
  if (enableBuzzered) {
    switch (melody) {
      case MELODY_POWER_UP:
        playMelody(tunePowerUp, beatPowerUp, 300);
        break;
      case MELODY_LOW_POWER:
        playMelody(tuneLowPower, beatLowPower, 300);
        break;
      case MELODY_NO_POWER:
        playMelody(tuneNoPower, beatNoPower, 500);
        break;
      case MELODY_WIFI_CONNECT_SUCCESS:
        playMelody(tuneWifiSucc, beatWifiSucc, 300);
        break;
      case MELODY_WIFI_CONNECT_FAILED:
        playMelody(tuneWifiFailed, beatWifiFailed, 300);
        break;
      case MELODY_WIFI_DISCONNECT:
        playMelody(tuneWifiDis, beatWifiDis, 300);
        break;
      case MELODY_BLE_CONNECT_SUCCESS:
        playMelody(tuneBleSucc, beatBleSucc, 100);
        break;
      case MELODY_BLE_DISCONNECT:
        playMelody(tuneBleDis, beatBleDis, 100);
        break;
      case MELODY_CAM_CONNECT_SUCCESS:
        playMelody(tuneCamSucc, beatCamSucc, 100);
        break;
      case MELODY_CAM_DISCONNECT:
        playMelody(tuneCamDis, beatCamDis, 100);
        break;
      case MELODY_CAM_FAILURE:
        playMelody(tuneCamFailure, beatCamFailure, 500);
        break;
      case MELODY_BEEP_1:
        playMelody(tuneBeep1, beatBeep1, 300);
        break;
      case MELODY_BEEP_2:
        playMelody(tuneBeep2, beatBeep2, 100);
        break;
      case MELODY_DHTG:
        playMelody(tuneDhtg, beatDhtg, 500);
      case MELODY_BB_CLEAR_1:
        playMelody(tuneBb1, beatBb1, 100);
        break;
      case MELODY_BB_CLEAR_2:
        playMelody(tuneBb2, beatBb2, 100);
        break;
      default:
        break;
    }
  }
  vTaskDelete(xTaskGetCurrentTaskHandle());
}
