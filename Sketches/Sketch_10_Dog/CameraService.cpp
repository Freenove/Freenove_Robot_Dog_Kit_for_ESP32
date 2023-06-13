/**
 * @file CameraService.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Wifi, Camera, Command Serivice.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "CameraService.h"

#include <iostream>
using namespace std;

bool isOpenning_CameraService_Port = false;
bool isRunning_LoopTask_CameraService = false;
void task_CameraService(void *pvParameters);
bool isRunning_LoopTask_CmdService = false;
void task_CmdService(void *pvParameters);

WiFiServer server_Cmd(CMD_SERVER_PORT);
WiFiServer server_Camera(CAMERA_SERVER_PORT);
CameraService cs = CameraService();

bool startFlag_CameraService = false;

char *ssid_AP; // = "FreenoveDog";
const char *password_AP = "12345678";

// camera_fb_t *fb = NULL;

CameraService::CameraService()
{
}
bool CameraService::begin()
{
	configWifi();
	// Serial.println(ssid_AP);
	return cameraSetup();
}
void CameraService::configWifi()
{
	// Serial.printf("Default: %d", WiFi.getTxPower());
	// WiFi.setTxPower(WIFI_POWER_19_5dBm);
	// esp_wifi_set_max_tx_power(84);
	// Serial.printf("Set Tx Power: %d", WiFi.getTxPower());
	// WiFi.setAutoConnect(true);
	// WiFi.setAutoReconnect(true);
	// WiFi.disconnect();

	String s = "FreenoveDog-" + getRobotId();
	ssid_AP = (char *)malloc(s.length());
	strcpy(ssid_AP, s.c_str());
	// Serial.println(ssid_AP);
}
void CameraService::sendWifiStatus()
{
	String s = "";
	// Serial.printf("WiFi.isConnected(): %d \n", WiFi.isConnected());
	if (WiFi.isConnected())
	{
		s = "N#101#" + String(WiFi.SSID()) + "#" + String(WiFi.localIP().toString()) + "#\n";
	}
	else
	{
		s = "N#102#\n";
	}
	// bleSend(std::string(s.c_str()));
	mqTx.enterForced(s);
	// Serial.print(s);
	printf("%s\n", s.c_str());
	// Serial.printf("wifi.getmode :%d \n",WiFi.getMode());
	if (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA)
	{
		s = "N#301#" + String(ssid_AP) + String("#") + String(WiFi.softAPIP().toString()) + String("#") + String(password_AP) + String("#\n");
	}
	else
	{
		s = "N#302#\n";
	}
	// bleSend(std::string(s.c_str()));
	mqTx.enterForced(s);
	// Serial.print(s);
	printf("%s\n", s.c_str());
}
void CameraService::sendCameraStatus()
{
	String s = "";
	if (isCameraNormal)
	{
		s = String(ACTION_CAMERA) + "#100#\n";
	}
	else
	{
		s = String(ACTION_CAMERA) + "#101#\n";
	}
	// bleSend(std::string(s.c_str()));
	mqTx.enterForced(s);
	Serial.print(s);
}
void sendWifiSSID(String ssid)
{
	String s = String(ACTION_NETWORK) + String("#") + String("0") + String("#") + ssid + String("#\n");
	// bleSend(std::string(s.c_str()));
	mqTx.enterForced(s);
}
void CameraService::scanWifi()
{
	Serial.println("scan start");
	// WiFi.scanNetworks will return the number of networks found
	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0)
	{
		Serial.println("no networks found");
	}
	else
	{
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; ++i)
		{
			// Print SSID and RSSI for each network found
			Serial.print(i + 1);
			Serial.print(": ");
			Serial.print(WiFi.SSID(i));
			Serial.print(" (");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");
			Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
			sendWifiSSID(WiFi.SSID(i));
			delay(10);
		}
	}
	Serial.println("");

	// Wait a bit before scanning again
	delay(5000);
}

void CameraService::establishAP(char *ssid, char *psd)
{
	WiFi.softAP(ssid, psd);
	IPAddress myIP = WiFi.softAPIP();
}

void CameraService::establishAP(String ssid, String psd)
{
	char *c_ssid = (char *)malloc(ssid.length());
	char *c_psd = (char *)malloc(psd.length());
	strcpy(c_ssid, ssid.c_str());
	strcpy(c_psd, psd.c_str());
	establishAP(c_ssid, c_psd);
}

void CameraService::establishSoftAP()
{
	Serial.println(ssid_AP);
	if (WiFi.softAP(ssid_AP, password_AP))
	{
		// WiFi.setTxPower(WIFI_POWER_19_5dBm);
		esp_wifi_set_max_tx_power(84);
		Serial.printf("Get Tx Power: %d", WiFi.getTxPower());
		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
		String s = "N#301#" + String(ssid_AP) + String("#") + String(WiFi.softAPIP().toString()) + String("#") + String(password_AP) + String("#\n");
		Serial.println(s);
		// bleSend(std::string(s.c_str()));
		mqTx.enterForced(s);
		establishCameraServer();
		setMelodyToQueue(MELODY_WIFI_CONNECT_SUCCESS);
	}
	// softAPdisconnect();
}

void CameraService::closeSoftAP()
{
	WiFi.enableAP(false);
	sendWifiStatus();
	setMelodyToQueue(MELODY_WIFI_DISCONNECT);
}

void CameraService::closeSoftSTA()
{
	WiFi.enableSTA(false);
	sendWifiStatus();
	setMelodyToQueue(MELODY_WIFI_DISCONNECT);
}

void CameraService::accessRouter(char *ssid, char *psd)
{
	if (WiFi.isConnected() && String(WiFi.SSID()).equals(ssid))
	{
		String s = "N#103#\n";
		// bleSend(std::string(s.c_str()));
		mqTx.enterForced(s);
		Serial.println(s);
		sendWifiStatus();
		Serial.println("WiFi had connected !");
		setMelodyToQueue(MELODY_WIFI_CONNECT_SUCCESS);
	}
	else
	{
		WiFi.disconnect();
		WiFi.begin(ssid, psd);
		Serial.print("Connecting ");
		Serial.print(ssid);
		Serial.print("  ");
		Serial.println(psd);
		int cnt = 16;
		while (!WiFi.isConnected() && cnt--)
		{
			delay(500);
			Serial.print(".");
			// WiFi.begin(ssid, psd);
		}
		Serial.println("");

		if (WiFi.isConnected())
		{
			// WiFi.setTxPower(WIFI_POWER_19_5dBm);
			esp_wifi_set_max_tx_power(84);
			Serial.printf("Set Tx Power: %d\n", WiFi.getTxPower());
			String s = "N#103#\n";
			// bleSend(std::string(s.c_str()));
			mqTx.enterForced(s);
			Serial.println(s);
			Serial.println("WiFi connect successes!");
			sendWifiStatus();
			setMelodyToQueue(MELODY_WIFI_CONNECT_SUCCESS);
			establishCameraServer();
		}
		else
		{
			String s = "N#104#\n";
			// bleSend(std::string(s.c_str()));
			mqTx.enterForced(s);
			Serial.println("");
			Serial.println("WiFi connect failed!");
			setMelodyToQueue(MELODY_WIFI_CONNECT_FAILED);
		}
	}
}

void CameraService::accessRouter(String ssid, String psd)
{
	char *c_ssid = (char *)malloc(ssid.length());
	char *c_psd = (char *)malloc(psd.length());
	strcpy(c_ssid, ssid.c_str());
	strcpy(c_psd, psd.c_str());
	accessRouter(c_ssid, c_psd);
}

void CameraService::establishCameraServer(u16 port)
{
	if (WiFi.isConnected() || (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA))
	{
		if (!isOpenning_CameraService_Port)
		{
			isOpenning_CameraService_Port = true;
			camServerPort = port;
			server_Camera.begin(port);
		}
		Serial.print("Camera Ready! Use '");
		Serial.print(WiFi.softAPIP());
		Serial.print(" or ");
		Serial.print(WiFi.localIP());
		Serial.println("' to connect in Freenove app.");

		startFlag_CameraService = true;
		startCameraServiceThread();

		startCmdServiceThread();
	}

	// startCmdServiceThread();
	// server_Camera.begin(port);
	// startFlag_CameraService = true;
	// startCameraServiceThread();
}

void CameraService::establishCmdServer(u16 port)
{
	if (WiFi.isConnected())
	{
		cmdServerPort = port;
		server_Cmd.begin(port);
		startCmdServiceThread();
	}
}

bool CameraService::cameraSetup()
{
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sscb_sda = SIOD_GPIO_NUM;
	config.pin_sscb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 20000000;
	config.pixel_format = PIXFORMAT_JPEG; // PIXFORMAT_RGB565; // PIXFORMAT_JPEG;
	config.frame_size = FRAMESIZE_QVGA;
	config.jpeg_quality = 10;
	config.fb_count = 1;
	config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
	// psramFound();
	// camera init
	esp_err_t err = esp_camera_init(&config);
	if (err != ESP_OK)
	{
		isCameraNormal = false;
		Serial.printf("Camera init failed with error 0x%x\n", err);
		return false;
	}
	camera_fb_t *fb = esp_camera_fb_get();
	esp_camera_fb_return(fb);
	// fb = esp_camera_fb_get();
	if (fb != NULL)
	{
		isCameraNormal = true;
		Serial.printf("Camera configuration complete! %d\n", fb->len);
		return true;
	}
	else
	{
		isCameraNormal = false;
		vTaskDelay(10);
		Serial.println("Camera failure. Please check the camera cable!");
		Serial.println("Or you can continue to use the robot, except for the camera function");
		return false;
	}
}

void CameraService::setCameraQuality(int quality)
{
	config.jpeg_quality = quality;
	esp_err_t err = esp_camera_init(&config);
	if (err != ESP_OK)
	{
		Serial.printf("Camera init failed with error 0x%x", err);
		return;
	}
}

void CameraService::setCameraResolution(framesize_t frameSize)
{
	config.frame_size = frameSize;
	esp_err_t err = esp_camera_init(&config);
	if (err != ESP_OK)
	{
		Serial.printf("Camera init failed with error 0x%x", err);
		return;
	}
}

void CameraService::startCameraServiceThread()
{
	if (!isRunning_LoopTask_CameraService)
	{
		Serial.println("isRunning_LoopTask_CameraService = true;");
		isRunning_LoopTask_CameraService = true;
		// xTaskCreateUniversal(task_CameraService, "task_Cam", 81920, NULL, 2, &taskHandle_Camera, 0);
		// xTaskCreateUniversal(task_CameraService, "task_Cam", 8192, NULL, configMAX_PRIORITIES, &taskHandle_Camera, 1);
		startTask(TASK_CAMERA_SERVICE);
		// cs.startCmdServiceThread();
	}
}

void CameraService::startCmdServiceThread()
{
	if (!isRunning_LoopTask_CmdService)
	{
		isRunning_LoopTask_CmdService = true;
		xTaskCreateUniversal(task_CmdService, "task_CmdService", 8192, NULL, 1, NULL, 1); // loopTask_Cmd uses core 1.
	}
}

void task_CameraService(void *pvParameters)
{
	isRunning_LoopTask_CameraService = true;
	Serial.println("Task task_CameraService is starting ... ");
	while (!startFlag_CameraService)
	{
		Serial.print(">");
		vTaskDelay(1000);
	}
	Serial.print("task_CameraService() function running on core: ");
	Serial.println(xPortGetCoreID());
	uint32_t t4 = 0, t0 = 0, t1 = 0, t2 = 0, t3 = 0;
	server_Camera.setNoDelay(true);
	while (true)
	{
		Serial.print("!");
		vTaskDelay(1000);
		WiFiClient client = server_Camera.available(); // listen for incoming clients
		if (client)									   // if you get a client,
		{
			setMelodyToQueue(MELODY_CAM_CONNECT_SUCCESS);
			client.setNoDelay(true);
			Serial.println("Camera Server connected to a client."); // print a message out the serial port
			String currentLine = "";								// make a String to hold incoming data from the client
			camera_fb_t *fb = NULL;
			while (client.connected())
			{
				t0 = millis();
				fb = esp_camera_fb_get();
				// camera_fb_t *fb = esp_camera_fb_get();
				t1 = millis();
				if (fb != NULL)
				{
					size_t fb_size = fb->len;
					uint8_t slen[4];
					slen[0] = fb_size >> 0;
					slen[1] = fb_size >> 8;
					slen[2] = fb_size >> 16;
					slen[3] = fb_size >> 24;
					client.write(slen, 4);
					size_t sz = client.write(fb->buf, fb_size);
					t2 = millis();
					esp_camera_fb_return(fb);
					t3 = millis();
					// client.flush();
					// Serial.printf("slen: %d,%d,%d,%d \n", slen[0], slen[1], slen[2], slen[3]);
					// Serial.printf("sz:%zu, rt:%zu, g:%zu, ts:%zu,\t t: %zu \n", fb_size, sz, t1 - t0, t2 - t1, t3 - t0);
					Serial.printf("sz:%zu, ts:%zu\n", fb_size, t2 - t1);
					//  Serial.printf("fb_size:%ld, fb_get:%ld, tcpSend:%ld, fb_ret:%ld, all1: %ld, All2: %ld \n", fb->len, t1 - t0, t2 - t1, t3 - t2, t3 - t0, t3 - t4);
					// Serial.printf("l:%zu, sz1:%zu, sz:%zu, g:%zu, ts:%zu,\t t: %zu \n", fb->len, fb_size, sz, t1 - t0, t2 - t1, t3 - t0);
					if (sz == 0)
					{
						client.stop();
						// server_Camera.disconnect();
						Serial.printf("Socket send error, please retry ! l:%zu, sz:%zu \n", fb->len, sz);
					}
					// Serial.println("fb_size:" + String(fb->len) + " fb_get:" + String(t1 - t0) + " tcpSend:" + String(t2 - t1) + " fb_ret:" + String(t3 - t2) + " all1:" + String(t3 - t4) + " All2:" + String(t3 - t4));
					// vTaskDelay(5);
				}
				else
				{
					cs.isCameraNormal = false;
					cs.sendCameraStatus();
					Serial.println("Camera failure. Please check the camera cable!");
					Serial.println("Or Whether the buzzer is abnormal ?");
					setMelodyToQueue(MELODY_CAM_FAILURE);
					vTaskDelay(3000);
					// cs.begin();
				}
				t4 = t3;
			}
			// close the connection:
			client.stop();
			Serial.println("Camera Client Disconnected.");
			setMelodyToQueue(MELODY_CAM_DISCONNECT);
		}
	}
}

bool isWifiConnected = false;

void task_CmdService(void *pvParameters)
{
	isRunning_LoopTask_CmdService = true;
	Serial.println("Task task_CmdService is starting ... ");
	Serial.print("task_CmdService() function running on core: ");
	Serial.println(xPortGetCoreID());
	server_Cmd.begin(CMD_SERVER_PORT);
	while (1)
	{
		vTaskDelay(1000);
		WiFiClient client = server_Cmd.available(); // listen for incoming clients
		if (client)
		{															 // if you get a client,
			Serial.println("Command Server connected to a client."); // print a message out the serial port
			client.setTimeout(2000);
			bleStop();
			isWifiConnected = true;
			String currentLine = ""; // make a String to hold incoming data from the client
			while (client.connected())
			{ // loop while the client's connected
				if (client.available())
				{
					onWiFiCmdReceived(&client);
				}
				delay(20);
				onWiFiCmdTrasmit(&client);
			}
			// close the connection:
			client.stop();
			isWifiConnected = false;
			Serial.println("Command Client Disconnected.");
			bleRestart();
		}
	}
}
