/**
 * @file Buzzer.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Buzzer service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _BUZZER_h
#define _BUZZER_h

#include "NoteFrequency.h"
#include "Public.h"
#include "driver/ledc.h"

#define MELODY_POWER_UP 0
#define MELODY_LOW_POWER 1
#define MELODY_NO_POWER 2
#define MELODY_WIFI_CONNECT_SUCCESS 3
#define MELODY_WIFI_CONNECT_FAILED 4
#define MELODY_WIFI_DISCONNECT 5
#define MELODY_BLE_CONNECT_SUCCESS 6
#define MELODY_BLE_DISCONNECT 7
#define MELODY_CAM_CONNECT_SUCCESS 8
#define MELODY_CAM_DISCONNECT 9
#define MELODY_CAM_FAILURE 10
#define MELODY_BEEP_1 11
#define MELODY_BEEP_2 12
#define MELODY_DHTG 13
#define MELODY_BB_CLEAR_1 14
#define MELODY_BB_CLEAR_2 15

const int tunePowerUp[] = {CH1, EH1, CH7, EH7};
const float beatPowerUp[] = {HALF, HALF, HALF, WHOLE};

const int tuneLowPower[] = {EM1, CM1, AL1};
const float beatLowPower[] = {HALF, HALF, HALF};

const int tuneNoPower[] = {EM1, CM1, AM1, CH1};
const float beatNoPower[] = {HALF, HALF, HALF, WHOLE};

const int tuneWifiSucc[] = {GH7, EH7, GH7};
const float beatWifiSucc[] = {HALF, HALF, HALF};

const int tuneWifiFailed[] = {CH7, EH7, CH7};
const float beatWifiFailed[] = {HALF, HALF, HALF};

const int tuneWifiDis[] = {EH7, GH7, EH7};
const float beatWifiDis[] = {HALF, HALF, HALF};

const int tuneBleSucc[] = {CH4, NF0, CH4, NF0, EH4, NF0, EH4, NF0, GH4};
const float beatBleSucc[] = {HALF, HALF, HALF, HALF, HALF, HALF, HALF, HALF, WHOLE};

const int tuneBleDis[] = {GH4, NF0, GH4, NF0, EH4, NF0, EH4, NF0, CH4};
const float beatBleDis[] = {HALF, HALF, HALF, HALF, HALF, HALF, HALF, HALF, WHOLE};

const int tuneCamSucc[] = {CH7, NF0, CH7, NF0, EH7, NF0, GH7};
const float beatCamSucc[] = {HALF, HALF, HALF, HALF, HALF, HALF, WHOLE};

const int tuneCamDis[] = {GH7, NF0, GH7, NF0, EH7, NF0, CH7};
const float beatCamDis[] = {HALF, HALF, HALF, HALF, HALF, HALF, WHOLE};

const int tuneCamFailure[] = {EL1, NF0, EL1, NF0, EL1, NF0, EL1};
const float beatCamFailure[] = {WHOLE, WHOLE, WHOLE, WHOLE, WHOLE, WHOLE, WHOLE};

const int tuneBeep1[] = {CH1};
const float beatBeep1[] = {WHOLE};

const int tuneBeep2[] = {EH1, NF0, EH1, NF0};
const float beatBeep2[] = {HALF, HALF, HALF, HALF};

const int tuneDhtg[] = {DM5, DM5, DM5, DM6, DM5, DM1, DM2, DM3, DM1, DL5};
const float beatDhtg[] = {1, 1, 0.5, 0.25, 1, 0.5, 0.25, 0.5, 0.5, 1};

const int tuneBb1[] = {2000};
const float beatBb1[] = {WHOLE};

const int tuneBb2[] = {2000, NF0, 2000, NF0};
const float beatBb2[] = {WHOLE, WHOLE, WHOLE, WHOLE};


void setupBuzzer();
void setBuzzer(uint16_t freq);

void setMelodyToQueue(int m);
void setMelodyToPlay(int m);

void task_Buzzered(void *pvParameters);
void task_BuzzerService(void *pvParameters);
#endif
