/**
 * @file CameraService.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Wifi, Camera, Command Serivice.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _CAMERASERVICE_h
#define _CAMERASERVICE_h

#include "esp_camera.h"
#include "BluetoothService.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "BluetoothOrders.h"
#include "Public.h"

#define CAMERA_MODEL_WROVER_KIT
#if defined(CAMERA_MODEL_WROVER_KIT)
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 21
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 19
#define Y4_GPIO_NUM 18
#define Y3_GPIO_NUM 5
#define Y2_GPIO_NUM 4
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#endif

#define CMD_SERVER_PORT 5000
#define CAMERA_SERVER_PORT 8000

// const char* ssid_Router = "FYI_2.4G";
// const char* password_Router = "freenove208";

class CameraService
{
private:
	u16 camServerPort = 0;
	u16 cmdServerPort = 0;
	camera_config_t config;

public:
	bool isCameraNormal = true;
	CameraService();
	bool begin();
	void configWifi();
	void sendWifiStatus();
	void sendCameraStatus();
	void scanWifi();

	void establishAP(char *ssid, char *psd);
	void establishAP(String ssid, String psd);
	void establishSoftAP();
	void closeSoftAP();

	void accessRouter(char *ssid, char *psd);
	void accessRouter(String ssid, String psd);
	void closeSoftSTA();

	void establishCameraServer(u16 port = CAMERA_SERVER_PORT);
	void establishCmdServer(u16 port = CMD_SERVER_PORT);
	bool cameraSetup();
	void setCameraQuality(int quality);
	void setCameraResolution(framesize_t frameSize);

	void startCameraServiceThread();
	void startCmdServiceThread();

protected:
};

extern bool isWifiConnected;
extern CameraService cs;
extern WiFiServer server_Camera;
void  __attribute__((weak)) onWiFiCmdReceived(WiFiClient *client);
extern void onWiFiCmdReceived(WiFiClient *client);
void  __attribute__((weak)) onWiFiCmdTrasmit(WiFiClient *client);
extern void onWiFiCmdTrasmit(WiFiClient *client);



#endif
