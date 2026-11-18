#include "BluetoothService.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "0000ffe0-0000-1000-8000-00805f9b34fb"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "0000ffe1-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_TX "0000ffe1-0000-1000-8000-00805f9b34fb"

NimBLEServer *pServer = NULL;
NimBLECharacteristic *pTxCharacteristic;
bool isBleConnected = false;
bool oldBleConnected = false;
uint8_t txValue = 0;

class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) {
    isBleConnected = true;
    // connecting
    if (isBleConnected && !oldBleConnected) {
      // do stuff here on connecting
      oldBleConnected = isBleConnected;
      Serial.println("Ble Connect");
    }
  };

  void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) {
    isBleConnected = false;
    // disconnecting
    if (!isBleConnected && oldBleConnected) {
      oldBleConnected = isBleConnected;
      Serial.println("Ble Disconnect");

      delay(500);                   // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising();  // restart advertising
    }
  }
};

class MyCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) {
    onBleReceived(pCharacteristic);
  }
};

void bleSetup() {
  // Create the BLE Device
  const char *deviceName = "Freenove-Dog";

  NimBLEDevice::init(deviceName);
  NimBLEDevice::setMTU(512);
  //  Create the BLE Server
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  NimBLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  // NimBLE automatically creates the 0x2902 CCCD for NOTIFY/INDICATE characteristics
  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    NIMBLE_PROPERTY::NOTIFY);

  NimBLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    NIMBLE_PROPERTY::WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising (setName makes the nickname visible in BLE scans)
  NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setName(deviceName);
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->enableScanResponse(true);
  pAdvertising->start();
  Serial.println("Bluetooth initialization complete, Waiting a client connection to notify...");
}

void bleStop() {
  NimBLEDevice::deinit();
}

void bleSend(String msg) {
  if (isBleConnected) {
    pTxCharacteristic->setValue(msg);
    pTxCharacteristic->notify();
  }
}
