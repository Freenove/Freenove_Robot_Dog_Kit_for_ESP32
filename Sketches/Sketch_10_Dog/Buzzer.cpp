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
#include <iostream>
using namespace std;
/**********************************************************************
  Filename    : Alertor
  Description : Using IDF-SDK function to generate PWM control buzzer.
  Auther      : www.freenove.com
  Modification: 2020/07/11
**********************************************************************/

#define BUZZER_PIN (PIN_BUZZER)
#define LEDC_GPIO (BUZZER_PIN)

#define LEDC_TIMER LEDC_TIMER_1
#define LEDC_CHANNEL LEDC_CHANNEL_1

#define LEDC_MODE LEDC_HIGH_SPEED_MODE

bool isAlerting = false;
int melody = -1;

void setupBuzzer()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE, // timer mode
        {
            .duty_resolution = LEDC_TIMER_10_BIT // resolution of PWM duty
        },
        .timer_num = LEDC_TIMER, // timer index
        .freq_hz = 5000,         // frequency of PWM signal
        .clk_cfg = LEDC_AUTO_CLK};

    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = LEDC_GPIO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0};

    ledc_channel_config(&ledc_channel);

    ledc_fade_func_install(0);

    // ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL);       // attach the led pin to pwm channel
    ledcSetup(LEDC_CHANNEL, 10, LEDC_TIMER_10_BIT); // setup pwm channel
    // ledcWriteTone(LEDC_CHANNEL, 0);
}

void setBuzzer(uint16_t freq)
{
    // ledcWriteTone(LEDC_CHANNEL, freq);

    if (freq != 0)
    {
        // setupBuzzer();
        ledc_set_freq(LEDC_MODE, LEDC_TIMER, freq);
        // ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 511);
        // ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        ledc_set_duty_and_update(LEDC_MODE, LEDC_CHANNEL, 512, 0);
    }
    else
    {
        ledc_set_duty_and_update(LEDC_MODE, LEDC_CHANNEL, 512, 0);
        ledc_stop(LEDC_MODE, LEDC_CHANNEL, LOW);
        // ledcDetachPin(LEDC_GPIO);
        // pinMode(LEDC_GPIO, OUTPUT);
        // digitalWrite(LEDC_GPIO, LOW);

        // ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
        // ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        //  ledc_stop(LEDC_MODE, LEDC_CHANNEL, LOW);
    }
}

template <int N> //Non-type template parameters
void playMelody(const int (&tune)[N], const float (&beat)[N], float speed)
{
    for (int i = 0; i < N; i++)
    {
        setBuzzer(tune[i]);
        vTaskDelay(beat[i] * speed);
        setBuzzer(0);
    }
    setBuzzer(0);
}

bool enableBuzzered = false;
extern TaskHandle_t taskHandle_Buzzered;
DataQueue<int> mqBz(20); // Buzzer queue

void setMelodyToQueue(int m)
{
    mqBz.enterForced(m);
    // Serial.printf("setMelodyToQueue %d \n", m);
}

void setMelodyToPlay(int m)
{
    melody = m;
    enableBuzzered = true;
    // controlTask(TASK_BUZZERED, TASK_RESUME);
    //  task_SuspendOrResume(TASK_BUZZERED, true);
    startTask(TASK_BUZZERED);
}

void task_Buzzered(void *pvParameters)
{
    // Serial.print("task_Buzzered() function running on core: ");
    // Serial.println(xPortGetCoreID());
    //   while (1)
    {
        if (enableBuzzered)
        {
            switch (melody)
            {
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
            // Serial.printf("buzzered : %d \n", cnt++);
        }
        // Serial.printf("taskHandle_Buzzered2 : %d \n", eTaskGetState(taskHandle_Buzzered));
        // cout << "taskHandle_Buzzered2 : " << eTaskGetState(taskHandle_Buzzered) << " " << taskHandle_Buzzered;
    }
    vTaskDelete(xTaskGetCurrentTaskHandle());
}

void task_BuzzerService(void *pvParameters)
{
    if (!mqBz.isEmpty())
    {
        if (taskHandle_Buzzered != NULL)
        {
            // Serial.printf("taskHandle_Buzzered1 : %d, %d \n", eTaskGetState(taskHandle_Buzzered), taskHandle_Buzzered);
            // cout << "taskHandle_Buzzered1 : " << eTaskGetState(taskHandle_Buzzered) << " " << taskHandle_Buzzered;
            if (eTaskGetState(taskHandle_Buzzered) == eReady || eTaskGetState(taskHandle_Buzzered) == eDeleted)
            {
                setMelodyToPlay(mqBz.out());
            }
        }
        else
        {
            // Serial.printf("taskHandle_Buzzered0 : %d ,new task\n", taskHandle_Buzzered);
            // cout << "taskHandle_Buzzered0 : ,new task : "  << " " << taskHandle_Buzzered;
            setMelodyToPlay(mqBz.out());
        }
    }
}