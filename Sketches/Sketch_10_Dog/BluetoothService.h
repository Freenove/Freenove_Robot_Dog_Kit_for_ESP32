
/**
 * @file BluetoothService.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Bluetooth Gatt Service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */ 

#ifndef _BLUETOOTHSERVICE_h
#define _BLUETOOTHSERVICE_h

#include <string>
//using namespace std;

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "MassageQueue.h"
#include "Buzzer.h"

#include "Arduino.h"
//#include "Public.h"

extern bool isBleConnected;

void bleSetup();
void bleRestart();
void bleStop();
void bleSend(std::string msg);
void bleSend(String msg);
void task_BleUploadService(void *pvParameters);

extern void onBleReceived(BLECharacteristic *pCharacteristic);
void  __attribute__((weak)) onBleReceived(BLECharacteristic *pCharacteristic);


#endif

