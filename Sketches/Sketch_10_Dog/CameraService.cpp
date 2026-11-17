/**
 * @file CameraService.cpp
 * @author zhentao lin@freenove (support@freenove.com)
 * @brief Wifi, Camera, Command Serivice.
 * @version v1.2.0
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "CameraService.h"
#include "img_converters.h"
#include "sensor.h"

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
	ssid_AP = (char *)malloc(s.length() + 1);
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
	establishAP((char *)ssid.c_str(), (char *)psd.c_str());
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
	accessRouter((char *)ssid.c_str(), (char *)psd.c_str());
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

void CameraService::fillCameraPinConfig()
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
	config.pin_sccb_sda = SIOD_GPIO_NUM;
	config.pin_sccb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 10000000;
	config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
	config.fb_location = CAMERA_FB_IN_PSRAM;
	config.fb_count = 1;
}

bool CameraService::initCameraHardware()
{
	fillCameraPinConfig();
	// Prefer hardware JPEG (OV2640 / OV3660). Sensors without JPEG (e.g. GC0308)
	// return ESP_ERR_NOT_SUPPORTED; fall back to RGB565 and convert later with frame2jpg().
	config.pixel_format = PIXFORMAT_JPEG;

	esp_err_t err = esp_camera_init(&config);
	if (err == ESP_ERR_NOT_SUPPORTED)
	{
		Serial.println("JPEG not supported by sensor, fallback to RGB565");
		esp_camera_deinit();
		config.pixel_format = PIXFORMAT_RGB565;
		err = esp_camera_init(&config);
	}
	if (err != ESP_OK)
	{
		Serial.printf("Camera init failed with error 0x%x\n", err);
		return false;
	}

	configureDetectedSensor();
	return true;
}

void CameraService::configureDetectedSensor()
{
	sensor_t *s = esp_camera_sensor_get();
	if (s == NULL)
	{
		Serial.println("Camera sensor not found after init");
		return;
	}

	uint16_t pid = s->id.PID;
	Serial.printf("Camera sensor PID: 0x%x\n", pid);

	if (pid == OV2640_PID)
	{
		Serial.println("Detected OV2640");
		s->set_hmirror(s, 1);
		s->set_vflip(s, 1);
	}
	else if (pid == OV3660_PID)
	{
		Serial.println("Detected OV3660");
		s->set_hmirror(s, 1);
		s->set_vflip(s, 0);
	}
	else if (pid == GC0308_PID)
	{
		Serial.println("Detected GC0308");
		s->set_hmirror(s, 0);
		delay(500);
		s->set_vflip(s, 0);
	}
	else
	{
		Serial.println("Unknown camera sensor, using default mirror/flip");
		s->set_hmirror(s, 1);
		s->set_vflip(s, 0);
	}

	s->set_brightness(s, 1);
	s->set_saturation(s, 0);
	if (s->set_ae_level)
	{
		s->set_ae_level(s, -3);
	}
}

bool CameraService::cameraSetup()
{
	config = {};
	config.frame_size = FRAMESIZE_QVGA;
	config.jpeg_quality = 12;

	if (!initCameraHardware())
	{
		isCameraNormal = false;
		return false;
	}

	camera_fb_t *fb = esp_camera_fb_get();
	if (fb != NULL)
	{
		Serial.printf("Camera configuration complete! format=%d len=%d\n", (int)fb->format, fb->len);
		esp_camera_fb_return(fb);
		isCameraNormal = true;
		return true;
	}

	isCameraNormal = false;
	vTaskDelay(10);
	Serial.println("Camera failure. Please check the camera cable!");
	Serial.println("Or you can continue to use the robot, except for the camera function");
	return false;
}

void CameraService::setCameraQuality(int quality)
{
	config.jpeg_quality = quality;
	// Caller must ensure task_CameraService is idle: esp_camera_deinit() releases the driver the capture task runs on.
	esp_camera_deinit();
	if (!initCameraHardware())
	{
		isCameraNormal = false;
	}
}

void CameraService::setCameraResolution(framesize_t frameSize)
{
	config.frame_size = frameSize;
	// Caller must ensure task_CameraService is idle: esp_camera_deinit() releases the driver the capture task runs on.
	esp_camera_deinit();
	if (!initCameraHardware())
	{
		isCameraNormal = false;
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
	uint32_t t1 = 0, t2 = 0;
	server_Camera.setNoDelay(true);
	while (true)
	{
		//Serial.print("!");
		vTaskDelay(1000);
		WiFiClient client = server_Camera.accept(); // listen for incoming clients
		if (client)									   // if you get a client,
		{
			setMelodyToQueue(MELODY_CAM_CONNECT_SUCCESS);
			client.setNoDelay(true);
			Serial.println("Camera Server connected to a client."); // print a message out the serial port
			Serial.printf("[DIAG] cam fd=%d, heap=%u\r\n", client.fd(), (unsigned)ESP.getFreeHeap());
			String currentLine = "";								// make a String to hold incoming data from the client
			camera_fb_t *fb = NULL;
			while (client.connected())
			{
				fb = esp_camera_fb_get();
				t1 = millis();
				if (fb != NULL)
				{
					uint8_t *jpg_buf = NULL;
					size_t jpg_len = 0;
					bool jpg_converted = false;

					if (fb->format != PIXFORMAT_JPEG)
					{
						// GC0308 etc.: capture RGB565 then compress to JPEG for the existing client protocol.
						jpg_converted = frame2jpg(fb, cs.rgb2jpgQuality, &jpg_buf, &jpg_len);
						esp_camera_fb_return(fb);
						fb = NULL;
						if (!jpg_converted || jpg_buf == NULL)
						{
							Serial.println("frame2jpg failed");
							continue;
						}
					}
					else
					{
						jpg_buf = fb->buf;
						jpg_len = fb->len;
					}

					uint8_t slen[4];
					slen[0] = jpg_len >> 0;
					slen[1] = jpg_len >> 8;
					slen[2] = jpg_len >> 16;
					slen[3] = jpg_len >> 24;
					client.write(slen, 4);
					size_t sz = client.write(jpg_buf, jpg_len);
					t2 = millis();

					if (fb != NULL)
					{
						esp_camera_fb_return(fb);
						fb = NULL;
					}
					if (jpg_converted && jpg_buf != NULL)
					{
						free(jpg_buf);
						jpg_buf = NULL;
					}

					Serial.printf("sz:%zu, ts:%zu\n", jpg_len, (size_t)(t2 - t1));
					if (sz == 0)
					{
						client.stop();
						Serial.printf("Socket send error, please retry ! l:%zu, sz:%zu \n", jpg_len, sz);
					}
				}
				else
				{
					cs.isCameraNormal = false;
					cs.sendCameraStatus();
					Serial.println("Camera failure. Please check the camera cable!");
					Serial.println("Or Whether the buzzer is abnormal ?");
					setMelodyToQueue(MELODY_CAM_FAILURE);
					vTaskDelay(3000);
				}
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
		WiFiClient client = server_Cmd.accept(); // listen for incoming clients
		if (client)
		{															 // if you get a client,
			Serial.println("Command Server connected to a client."); // print a message out the serial port
			Serial.printf("[DIAG] cmd fd=%d, heap=%u\r\n", client.fd(), (unsigned)ESP.getFreeHeap());
			client.setTimeout(2000);
			bleDisconnectClient();
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
			clearRxLineBuffers();
		}
	}
}
