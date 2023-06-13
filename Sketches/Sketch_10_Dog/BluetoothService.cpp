/**
 * @file BluetoothService.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Bluetooth Gatt Service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "BluetoothService.h"

class BLEServer;
class BLECharacteristic;
class BLEServerCallbacks;

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool isBleConnected = false;
bool oldBleConnected = false;
uint8_t txValue = 0;

DataQueue<String> mqTx(100); // Ble message

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "0000ffe0-0000-1000-8000-00805f9b34fb" // UART service UUID
#define CHARACTERISTIC_UUID_RX "0000ffe1-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_TX "0000ffe1-0000-1000-8000-00805f9b34fb"

class MyServerCallbacks : public BLEServerCallbacks
{
	void onConnect(BLEServer *pServer)
	{
		isBleConnected = true;
		// connecting
		if (isBleConnected && !oldBleConnected)
		{
			// do stuff here on connecting
			oldBleConnected = isBleConnected;
			Serial.println("ble onConnect");
			setMelodyToQueue(MELODY_BLE_CONNECT_SUCCESS);
			enterMessageQueue(String(ACTION_UP_DOWN) + "#1#\n");
		}
	};

	void onDisconnect(BLEServer *pServer)
	{
		isBleConnected = false;
		// disconnecting
		if (!isBleConnected && oldBleConnected)
		{
			delay(500);					 // give the bluetooth stack the chance to get things ready
			pServer->startAdvertising(); // restart advertising
			Serial.println("start advertising");
			oldBleConnected = isBleConnected;

			enterMessageQueue(String(ACTION_UP_DOWN) + "#2#\n");
			setMelodyToQueue(MELODY_BLE_DISCONNECT);
			// server_Camera.close();
			// server_Camera.end();
		}
	}
};

class MyCallbacks : public BLECharacteristicCallbacks
{
	void onWrite(BLECharacteristic *pCharacteristic)
	{
		onBleReceived(pCharacteristic);
	}
};

// void  __attribute__((weak)) onBleReceived(BLECharacteristic *pCharacteristic) {
// 	std::string rxValue = pCharacteristic->getValue();

// 	if (rxValue.length() > 0) {
// 		Serial.println("*********");
// 		Serial.print("Received Value: ");
// 		for (int i = 0; i < rxValue.length(); i++)
// 			Serial.print(rxValue[i]);

// 		Serial.println();
// 		Serial.println("*********");
// 	}
// }

void bleSetup()
{

	// Create the BLE Device
	String s = String("Freenove-Dog-") + getRobotId();

	BLEDevice::init(std::string(s.c_str()));
	BLEDevice::setMTU(512);
	// BLEDevice::setMTU(50);
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

void bleRestart()
{
	// pServer->start();
	//  Start advertising
	// pServer->getAdvertising()->start();
	//BLEDevice::init();
	bleSetup();
}

void bleStop()
{
	BLEDevice::deinit();
	// pServer->disconnect();
	// pServer->getAdvertising()->stop();
}

void bleSend(std::string msg)
{
	if (isBleConnected)
	{
		pTxCharacteristic->setValue(msg);
		pTxCharacteristic->notify();
	}
}

void bleSend(String msg)
{
	bleSend(std::string(msg.c_str()));
}

void task_BleUploadService(void *pvParameters)
{
	if (!mqTx.isEmpty() && isBleConnected)
	{
		bleSend(mqTx.out());
	}
}

void bleTest()
{

	if (isBleConnected)
	{
		pTxCharacteristic->setValue(&txValue, 1);
		pTxCharacteristic->notify();
		txValue++;
		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}

	// disconnecting
	if (!isBleConnected && oldBleConnected)
	{
		delay(500);					 // give the bluetooth stack the chance to get things ready
		pServer->startAdvertising(); // restart advertising
		Serial.println("start advertising");
		oldBleConnected = isBleConnected;
	}
	// connecting
	if (isBleConnected && !oldBleConnected)
	{
		// do stuff here on connecting
		oldBleConnected = isBleConnected;
	}
}
