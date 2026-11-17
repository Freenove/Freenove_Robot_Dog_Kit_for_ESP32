/**
 * @file BatteryPower.cpp
 * @author zhentao lin@freenove (support@freenove.com)
 * @brief Read, monitor and upload battery power.
 * @version v1.2.0
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "BatteryPower.h"

/**********************************************************************
  Filename    : ADC_DAC
  Description : Using IDF-SDK function to read ADC of ESP32.
  Auther      : www.freenove.com
  Modification: 2020/07/11
**********************************************************************/
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#define PIN_ANALOG_IN PIN_BATT // ADC1:GPIO32

#define DEFAULT_VREF 1100 // Default vref
#define NUM_OF_SAMPLES 64 // 64 // Multisampling, and get average

adc_channel_t channel = ADC_CHANNEL_4; // ADC1:GPIO32
adc_unit_t unit = ADC_UNIT_1;          // ADC1
adc_atten_t atten = ADC_ATTEN_DB_12;   // Full scale 0-3.9V, precision range 150mV-2450mV

static adc_oneshot_unit_handle_t adc_handle = NULL;
static adc_cali_handle_t adc_cali_handle = NULL;
static bool adc_cali_ready = false;

void setupAdc()
{
    adc_oneshot_unit_init_cfg_t unit_cfg = {};
    unit_cfg.unit_id = unit;
    unit_cfg.ulp_mode = ADC_ULP_MODE_DISABLE;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel, &chan_cfg));

    adc_cali_line_fitting_config_t cali_cfg = {};
    cali_cfg.unit_id = unit;
    cali_cfg.atten = atten;
    cali_cfg.bitwidth = ADC_BITWIDTH_DEFAULT;
    adc_cali_ready = (adc_cali_create_scheme_line_fitting(&cali_cfg, &adc_cali_handle) == ESP_OK);
}

uint32_t getBatteryVoltage()
{
    uint32_t adc_reading = 0;
    int raw = 0;
    pthread_mutex_lock(&mutexPin33); //lock
    for (int i = 0; i < NUM_OF_SAMPLES; i++)
    { //  Multisampling
        adc_oneshot_read(adc_handle, channel, &raw);
        adc_reading += raw;
    }
    adc_reading /= NUM_OF_SAMPLES;
    // Convert adc_reading to voltage in mV
    int voltage = 0;
    if (adc_cali_ready)
    {
        adc_cali_raw_to_voltage(adc_cali_handle, (int)adc_reading, &voltage);
    }
    else
    {
        voltage = (int)(adc_reading * DEFAULT_VREF / 4095.0);
    }
    // printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
    pthread_mutex_unlock(&mutexPin33); //unlock
    return voltage * VOLATAGE_RATIO;
}

uint32_t batteryVoltage = 0; // unit: mV
uint8_t batteryPercent = 0;
unsigned long lastWaringTime = 0;
DataQueue<uint16_t> batVal(10, true);
unsigned long lastReadBatteryTime = 0;
unsigned long lastBleUploadTime = 0;
uint8_t lowPowerWarningCount = 0;
extern Freenove_PCA9685 pca;
void bleUploadBatteryInfo()
{
    if (isBleConnected || isWifiConnected)
    {
        if (millis() - lastBleUploadTime > UPLOAD_VOL_TIME)
        {
            String s = "I#";
            s = s + batteryVoltage + "#" + batteryPercent + "#\n";
            if (isBleConnected || isWifiConnected)
            {
                // bleSend(s);
                mqTx.enterForced(s);
            }
            // Serial.println(s);
            lastBleUploadTime = millis();
        }
    }
}

void task_BatteryPowerListener(void *pvParameters)
{
    // Serial.print("task_BatteryPowerListener() function running on core: ");
    // Serial.println(xPortGetCoreID());

    // while (1)
    {
        if (millis() - lastReadBatteryTime > READ_BAT_VOL_INTERVAL_TIME)
        {
            if (!isRobotMoving)
            {
                batteryVoltage = getBatteryVoltage();
                batVal.enterForced(batteryVoltage);
                batteryPercent = constrain((100 * (batVal.getMax() - BAT_VOL_PCT_0) / (BAT_VOL_PCT_100 - BAT_VOL_PCT_0)), 0, 100);
            }
            bleUploadBatteryInfo();
            if (batteryVoltage > 3000 && batteryVoltage < BAT_VOL_UNLOAD && millis() - lastWaringTime > LOW_POWER_WARNING_INTERVAL_TIME)
            {
                lowPowerWarningCount++;
                if (lowPowerWarningCount > 10)
                {
                    //lowPowerWarningCount = 0;
                    pca.releaseAllServo();
                    setMelodyToQueue(MELODY_NO_POWER);
                }
                else
                {
                    setMelodyToQueue(MELODY_LOW_POWER);
                }
                Serial.printf("Low Power : %lu ,%d\n", batteryVoltage, batteryPercent);
                lastWaringTime = millis();
            }
            lastReadBatteryTime = millis();
        }
    }
}
