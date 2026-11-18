#ifndef __BLUETOOTHSERVICE_H
#define __BLUETOOTHSERVICE_H

#include "Arduino.h"

#include <NimBLEDevice.h>

void bleSetup();
void bleStop();
void bleSend(String msg);
extern void onBleReceived(NimBLECharacteristic *pCharacteristic);

#endif
