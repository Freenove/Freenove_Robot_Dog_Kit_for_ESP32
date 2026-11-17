/**
 * @file BluetoothService.cpp
 * @author zhentao lin@freenove (support@freenove.com)
 * @brief Bluetooth Gatt Service.
 * @version v1.2.0
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "BluetoothService.h"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
volatile bool isBleConnected = false;
bool oldBleConnected = false;
volatile uint16_t bleConnHandle = 0;
uint8_t txValue = 0;

DataQueue<String> mqTx(100); // Ble message

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "0000ffe0-0000-1000-8000-00805f9b34fb" // UART service UUID
#define CHARACTERISTIC_UUID_RX "0000ffe1-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_TX "0000ffe1-0000-1000-8000-00805f9b34fb"

class MyServerCallbacks : public BLEServerCallbacks
{
	void onConnect(BLEServer *pServer, BLEConnInfo &connInfo)
	{
		bleConnHandle = connInfo.getConnHandle();
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

	void onDisconnect(BLEServer *pServer, BLEConnInfo &connInfo, int reason)
	{
		isBleConnected = false;
		Serial.printf("ble onDisconnect, reason=%d\n", reason);
		// disconnecting
		if (!isBleConnected && oldBleConnected)
		{
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
	void onWrite(BLECharacteristic *pCharacteristic, BLEConnInfo &connInfo)
	{
		onBleReceived(pCharacteristic);
	}
};

static MyServerCallbacks serverCallbacks;
static MyCallbacks characteristicCallbacks;

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

	BLEDevice::init(s.c_str());
	BLEDevice::setMTU(512);
	// BLEDevice::setMTU(50);
	//  Create the BLE Server
	pServer = BLEDevice::createServer();
	pServer->setCallbacks(&serverCallbacks);
	pServer->advertiseOnDisconnect(true);

	// Create the BLE Service
	BLEService *pService = pServer->createService(SERVICE_UUID);

	// Create a BLE Characteristic
	pTxCharacteristic = pService->createCharacteristic(
		CHARACTERISTIC_UUID_TX,
		NIMBLE_PROPERTY::READ |
		NIMBLE_PROPERTY::WRITE |
		NIMBLE_PROPERTY::WRITE_NR |
		NIMBLE_PROPERTY::NOTIFY);

	pTxCharacteristic->setCallbacks(&characteristicCallbacks);

	// Start the service
	pService->start();

	// Start advertising
	pServer->getAdvertising()->start();
	Serial.println("Bluetooth initialization complete, Waiting a client connection to notify...");
}

void bleDisconnectClient()
{
	if (pServer == NULL || !isBleConnected)
	{
		return;
	}
	bool ok = pServer->disconnect(bleConnHandle);
	Serial.printf("ble disconnect connId=%u, rc=%d\n", bleConnHandle, (int)ok);
}

void bleSend(std::string msg)
{
	if (isBleConnected && pTxCharacteristic != NULL)
	{
		pTxCharacteristic->setValue(msg.c_str());
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
