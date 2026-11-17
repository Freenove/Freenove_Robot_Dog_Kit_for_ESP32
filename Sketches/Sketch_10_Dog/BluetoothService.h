
/**
 * @file BluetoothService.h
 * @author zhentao lin@freenove (support@freenove.com)
 * @brief Bluetooth Gatt Service.
 * @version v1.2.0
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */ 

#ifndef _BLUETOOTHSERVICE_h
#define _BLUETOOTHSERVICE_h

#include <string>
//using namespace std;

#define CONFIG_NIMBLE_CPP_LOG_LEVEL 0
#include <NimBLEDevice.h>
#include "MassageQueue.h"
#include "Buzzer.h"

#include "Arduino.h"
//#include "Public.h"

extern volatile bool isBleConnected;

void bleSetup();
void bleDisconnectClient();
void bleSend(std::string msg);
void bleSend(String msg);
void task_BleUploadService(void *pvParameters);

extern void onBleReceived(BLECharacteristic *pCharacteristic);
void  __attribute__((weak)) onBleReceived(BLECharacteristic *pCharacteristic);

#endif

