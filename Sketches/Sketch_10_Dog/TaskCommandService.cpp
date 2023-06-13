/**
 * @file TaskCommandService.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Non-motion command execution server.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "TaskCommandService.h"

// extern "C" void wifi_init_sta();

MessageParser mpi = MessageParser(); // message parser info
bool isLegalController = false;

void task_CommandService(void *pvParameters)
{
	// Serial.print("task_CommandService() function running on core: ");
	// Serial.println(xPortGetCoreID());
	//  Serial.print("1 pvParameters: ");
	//  Serial.printf("%d\n", *(bool *)pvParameters);
	//  while (pvParameters)
	// while (1)
	{
		if (!mqInfo.isEmpty())
		{
			// Serial.print("mqInfo.length : ");
			// Serial.print(mqInfo.length());
			mpi.parser(mqInfo.out());

			switch (mpi.commandChar)
			{
			case ACTION_TEST:
				// danceDancing();
				break;
			case ID_CHECK: //
				if (mpi.paramterCount >= 1)
				{
					String s = "";
					switch (mpi.paramters[1])
					{
					case 0: // W#0#  get all infomation
						s = String(ID_CHECK) + "#0#" + SW_VERSION + "#" + ROBOT_NAME + "#\n";
						mqTx.enterForced(s);
						break;
					case 1: // W#1#  get firmware version
						s = String(ID_CHECK) + "#1#" + SW_VERSION + "#\n";
						mqTx.enterForced(s);
						break;
					case 2: // W#2# get robot name
						s = String(ID_CHECK) + "#2#" + ROBOT_NAME + "#\n";
						mqTx.enterForced(s);
						break;
					case 3: // W#3# get internal code
						s = String(ID_CHECK) + "#3#" + INTERNAL_CODE + "#\n";
						mqTx.enterForced(s);
						break;
					case 4: // W#4#FREENOVE# Check the identity of the controller
						if (strcmp(FREENOVE_STR, mpi.inputCommandArray[2].c_str()) == 0)
						{
							isLegalController = true;
							s = "Controller is legal.";
						}
						else
						{
							s = "Controller is llegal.";
						}
						break;
					default:
						break;
					}
					Serial.println(s);
				}
				break;

			case ACTION_RGB:
				if (mpi.paramterCount >= 3)
				{
					setRGBLED(mpi.paramters[1], mpi.paramters[2], mpi.paramters[3], mpi.paramters[4]);
				}
				break;
			case ACTION_ULTRASONIC:
				if (mpi.paramterCount >= 0)
				{
					Serial.printf("getSonar : %.2f cm\n", getSonar());
				}
				break;
			case ACTION_BUZZER:
				if (mpi.paramterCount >= 1)
				{
					setBuzzer(mpi.paramters[1]);
				}
				break;
			case ACTION_GET_VOLTAGE:
				if (mpi.paramterCount >= 0)
				{
					Serial.printf("Battery Voltage : %d \n", getBatteryVoltage() * 4);
				}
				break;
			case ACTION_SET_ROBOT:
				if (mpi.paramterCount >= 1)
				{
					setMoveSpeed(mpi.paramters[1]);
				}
				break;
			case ACTION_SET_NVS:
				if (mpi.paramterCount >= 1)
				{
					switch (mpi.paramters[1])
					{
					case 1: // save rgbled display mode.
						saveLedConfigToStorage();
						break;
					case 2: // clear all nvs data
						Serial.println("nvs_clear_all_data() ... !!!");
						nvs_clear_all_data();
						break;
					default:
						break;
					}
					break;
				}
			case ACTION_AUTO_WALKING:
				if (mpi.paramterCount >= 1)
				{
					switch (mpi.paramters[1])
					{
					case 0:
						setEnableAutoWalking(false);
						enterMessageQueue(String(ACTION_MOVE_ANY) + "#0#0#0#");
						break;
					case 1:
						setEnableAutoWalking(true);
						// controlTask(TASK_AUTO_WALKING, TASK_RESUME);
						break;
					default:
						break;
					}
				}
				break;
			case ACTION_NETWORK:
				if (mpi.paramterCount >= 1)
				{
					switch (mpi.paramters[1])
					{
					case 0: // scan
						cs.scanWifi();
						break;
					case 1: // connect to router, open sta mode
						cs.accessRouter(mpi.inputCommandArray[2], mpi.inputCommandArray[3]);
						// wifi_init_sta();
						// cs.establishCameraServer();
						break;
					case 2: // disconnect from router, colse sta mode
						cs.closeSoftSTA();
						break;
					case 3: // open ap mode .
						cs.establishSoftAP();
						break;
					case 4: // clost ap mode.
						cs.closeSoftAP();
						break;
					case 5: // status
						cs.sendWifiStatus();
						cs.sendCameraStatus();
						break;
					default:
						break;
					}
				}
				break;
			default:
				// Serial.println("default");
				break;
			}
			// Serial.println("Clear paramters 1 ");
			mpi.clearParameters();
		}
	}
}