#ifndef __BLUETOOTHSERVICE_H
#define __BLUETOOTHSERVICE_H

#include "Arduino.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class BLEServer;
class BLECharacteristic;
class BLEServerCallbacks;

void bleSetup();
void bleStop();
void bleSend(std::string msg);
extern void onBleReceived(BLECharacteristic *pCharacteristic);

#endif
