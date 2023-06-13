/**
 * @file BatteryPower.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Read, monitor and upload battery power.
 * @version v1.0.0
 * @date 2022-04-13
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
#include "esp_adc_cal.h"
#define PIN_ANALOG_IN PIN_BATT // A10, ADC2_CHANNEL_0

#define DEFAULT_VREF 1100 // Default vref
#define NUM_OF_SAMPLES 64 // 64 // Multisampling, and get average

adc_channel_t channel = ADC_CHANNEL_4; // ADC1:GPIO32, ADC2:GPIO13
adc_unit_t unit = ADC_UNIT_1;          // ADC1
adc_atten_t atten = ADC_ATTEN_DB_11;   // Full scale 0-3.9V, precision range 150mV-2450mV

esp_adc_cal_characteristics_t *adc_chars;

esp_adc_cal_value_t val_type;

void check_efuse();
void print_char_val_type(esp_adc_cal_value_t val_type);
void setupAdc()
{
    // Check if Two Point or Vref are burned into eFuse
    // check_efuse();
    // Configure ADC
    if (unit == ADC_UNIT_1)
    { // ADC1 and ADC2 are initialized differently
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten((adc1_channel_t)channel, atten);
    }
    else
    {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }
    // Characterize ADC
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    // print_char_val_type(val_type);
}

uint32_t getBatteryVoltage()
{
    uint32_t adc_reading = 0;
    pthread_mutex_lock(&mutexPin33); //lock
    for (int i = 0; i < NUM_OF_SAMPLES; i++)
    { //  Multisampling
        if (unit == ADC_UNIT_1)
        {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        }
        else
        {
            int raw;
            adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
            adc_reading += raw;
        }
    }
    adc_reading /= NUM_OF_SAMPLES;
    // Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    // printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
    pthread_mutex_unlock(&mutexPin33); //unlock
    return voltage * VOLATAGE_RATIO;
}

void check_efuse()
{
    // Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
    {
        printf("eFuse Two Point: Supported\n");
    }
    else
    {
        printf("eFuse Two Point: NOT supported\n");
    }

    // Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
    {
        printf("eFuse Vref: Supported\n");
    }
    else
    {
        printf("eFuse Vref: NOT supported\n");
    }
}

void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
    {
        printf("Characterized using Two Point Value\n");
    }
    else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        printf("Characterized using eFuse Vref\n");
    }
    else
    {
        printf("Characterized using Default Vref\n");
    }
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
                Serial.printf("Low Power : %d ,%d\n", batteryVoltage, batteryPercent);                
                lastWaringTime = millis();
            }
            lastReadBatteryTime = millis();
        }
    }
}
