/**
 * @file TaskMotionService.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Motion command execution server.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "TaskMotionService.h"

MessageParser mpm = MessageParser(); // message parser motion

void task_MotionService(void *pvParameters)
{
	Serial.print("task_MotionService() function running on core: ");
	Serial.println(xPortGetCoreID());
	// Serial.print("2 pvParameters: ");
	// Serial.printf("%d\n", pvParameters);
	// while (pvParameters)
	while (1)
	{
		if (!mqMotion.isEmpty())
		{
			// Serial.print("mqMotion.length : ");
			// Serial.print(mqMotion.length());
			mpm.parser(mqMotion.out());
		}
		// Serial.printf("motion(self): %d \n", getTaskState(taskHandle_Motion_Service));
		switch (mpm.commandChar)
		{
		case ACTION_INSTALLATION:
			switch (mpm.paramters[1])
			{
			case 1:
				Serial.println("Installation position ... ");
				setServoToInstallationPosition();
				break;
			case 2:
				setServoToBeforeCalibrationPosition();
				break;
			default:
				break;
			}
			break;
		case ACTION_CALIBRATE: // J# Legn # Set1 / Confirm2 #x#y#z
			// Serial.println("ACTION_CALIBRATE");
			if (mpm.paramterCount != 5)
			{
				break;
			}
			u8 n;				  // leg index:0,1,2,3
			float ofs[2][3];	  // Save the actual angle and theoretical angle of the calibration position respectively, and the subtraction is the offset.
			float tarPt[3];		  // Target position
			n = mpm.paramters[1]; // The definition and assignment here must be written separately, otherwise the switch branch will not be able to enter.
			switch (mpm.paramters[2])
			{
			case 1: //Set legs without saving data.
				tarPt[0] = mpm.paramters[3] + calibratePosition[n][0];
				tarPt[1] = mpm.paramters[4] + calibratePosition[n][1];
				tarPt[2] = mpm.paramters[5] + calibratePosition[n][2];
				printf("tarPt: %f, %f, %f \n", tarPt[0], tarPt[1], tarPt[2]);
				moveOneLegToPointDirectly(n, false, lastPt[n], tarPt);
				setMelodyToQueue(MELODY_BB_CLEAR_1);
				break;
			case 2: //Save data.
				cooToA(mpm.paramters[3] + calibratePosition[n][0],
					   mpm.paramters[4] + calibratePosition[n][1],
					   mpm.paramters[5] + calibratePosition[n][2], ofs[1]); //Calculate the angle of the actual position of the nth leg.
				cooToA(calibratePosition[n], ofs[0]);						//Calculate the angle of the calibrated position of the nth leg.
				servoOffset[n][0] = ofs[1][0] - ofs[0][0];
				servoOffset[n][1] = ofs[1][1] - ofs[0][1];
				servoOffset[n][2] = ofs[1][2] - ofs[0][2];
				prefs.put(KEY_SERVO_OFFSET, servoOffset);
				Serial.println(String(ofs[0][0]) + String(" ") + String(ofs[0][1]) + String(" ") + String(ofs[0][2]));
				Serial.println(String(ofs[1][0]) + String(" ") + String(ofs[1][1]) + String(" ") + String(ofs[1][2]));
				Serial.println(String(ofs[1][0] - ofs[0][0]) + String(" ") + String(ofs[1][1] - ofs[0][1]) + String(" ") + String(ofs[1][2] - ofs[0][2]));
				setMelodyToQueue(MELODY_BB_CLEAR_2);
				break;
			case 3:
				setBeforeCalibrationHeight(BODY_HEIGHT_MIN);
				pca.releaseAllServo();
				setMelodyToQueue(MELODY_BB_CLEAR_1);
				break;
			default:
				break;
			}
			break;
		case ACTION_UP_DOWN:
			switch (mpm.paramters[1])
			{
			case 0:
				if (isRobotStanding)
				{
					setBodyHeight(BODY_HEIGHT_MIN);
					pca.releaseAllServo();
					isRobotStanding = false;
				}
				else
				{
					setBodyHeight(BODY_HEIGHT_DEFAULT);
					standUp();
					isRobotStanding = true;
				}
				break;
			case 1:
				setBodyHeight(BODY_HEIGHT_DEFAULT);
				standUp();
				break;
			case 2:
				setBodyHeight(BODY_HEIGHT_MIN);
				pca.releaseAllServo();
				isRobotStanding = false;
				break;
			default:
				break;
			}
			break;
		case ACTION_BODY_HEIGHT:
			if (mpm.paramterCount >= 1)
			{
				setBodyHeight(mpm.paramters[1]);
			}
			break;
		case ACTION_TWIST:
			if (mpm.paramterCount >= 0)
			{
				resumeStanding();
				twist_any(mpm.paramters[1], mpm.paramters[2], mpm.paramters[3]);
			}
			break;
		case ACTION_MOVE_ANY:
			if (mpm.paramterCount >= 3)
			{
				resumeStanding();
				move_any(mpm.paramters[1], mpm.paramters[2], mpm.paramters[3], mpm.paramters[4]);
			}
			break;
		case ACTION_DANCING:
			if (mpm.paramterCount >= 0)
			{
				switch (mpm.paramters[1])
				{
				case DANCE_SAY_HELLO:
					danceSayHello();
					break;
				case DANCE_PUSH_UP:
					dancePushUp();
					break;
				case DANCE_STRETCH_SELF:
					danceStretchSelf();
					break;
				case DANCE_TURN_AROUND:
					danceTurnAround();
					break;
				case DANCE_SIT_DOWN:
					danceSitDown();
					break;
				case DANCE_DANCING:
					danceDancing();
					break;
				default:
					break;
				}
			}
			break;
		default:
			// suspend self task.
			controlTask(TASK_MOTION_SERVICE, TASK_SUSPEND);
			break;
		}
		//Determine whether it is a one-time command, only the movement command is issued once and keeps going.
		if (mpm.commandChar == ACTION_MOVE_ANY)
		{
			if (mpm.paramters[1] == 0 && mpm.paramters[2] == 0 && mpm.paramters[3] == 0)
			{
				mpm.clearParameters();
			}
		}
		else
		{
			// Serial.println("Clear paramters 1 ");
			mpm.clearParameters();
			// vTaskDelay(30);
		}
	}
}
