#include "BluetoothService.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "0000ffe0-0000-1000-8000-00805f9b34fb"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "0000ffe1-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_TX "0000ffe1-0000-1000-8000-00805f9b34fb"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool isBleConnected = false;
bool oldBleConnected = false;
uint8_t txValue = 0;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    isBleConnected = true;
    // connecting
    if (isBleConnected && !oldBleConnected) {
      // do stuff here on connecting
      oldBleConnected = isBleConnected;
      Serial.println("Ble Connect");
    }
  };

  void onDisconnect(BLEServer *pServer) {
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

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    onBleReceived(pCharacteristic);
  }
};

void bleSetup() {
  // Create the BLE Device
  String s = String("Freenove-Dog");

  BLEDevice::init(std::string(s.c_str()));
  BLEDevice::setMTU(512);
  //  Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Bluetooth initialization complete, Waiting a client connection to notify...");
}

void bleStop() {
  BLEDevice::deinit();
}

void bleSend(std::string msg) {
  if (isBleConnected) {
    pTxCharacteristic->setValue(msg);
    pTxCharacteristic->notify();
  }
}
