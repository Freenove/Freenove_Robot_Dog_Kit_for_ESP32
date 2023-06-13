/**
 * @file DanceMovements.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Robot dancing code.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "DanceMovements.h"

void danceDancing()
{
	const uint8_t repeatCounts = 3;
	const int8_t acts[][2][3] = {
		{{0, 0, 10}, {0, 0, -10}},
		{{0, 10, 0}, {0, -10, 0}},
		{{10, 0, 0}, {-10, 0, 0}}};
	// Serial.printf("sizeof(acts) / sizeof(acts[0]) : %d \n", sizeof(acts) / sizeof(acts[0]));
	for (uint8_t i = 0; i < sizeof(acts) / sizeof(acts[0]); i++)
	{
		for (int k = 0; k < repeatCounts; k++)
		{
			for (int j = 0; j < 2; j++)
			{
				twist_any(acts[i][j][0], acts[i][j][1], acts[i][j][2]);
				vTaskDelay(20);
			}
		}
	}

	int8_t x = 0, y = 0, z = 0;
	for (uint8_t j = 0; j < repeatCounts; j++)
	{
		for (int i = 0; i < 360; i += 30)
		{
			x = 10 * sin((double)i * PI / 180);
			z = 10 * cos((double)i * PI / 180);
			twist_any(x, y, z);
		}
	}

	for (int i = 0; i < repeatCounts; i++)
	{
		setBodyHeight(BODY_HEIGHT_MIN);
		vTaskDelay(200);
		setBodyHeight(BODY_HEIGHT_MAX);
		vTaskDelay(200);
	}
	twist_any(0, 0, 0);
}
void danceSitDown()
{
	const float spd = 2;
	float newPt[4][3] = {{calibratePosition[0][0], calibratePosition[0][1] + 30, calibratePosition[0][2]},
						 {calibratePosition[1][0], calibratePosition[1][1] - 30, calibratePosition[1][2]},
						 {calibratePosition[2][0], calibratePosition[2][1] - 30, calibratePosition[2][2]},
						 {calibratePosition[3][0], calibratePosition[3][1] + 30, calibratePosition[3][2]}};
	action_twist(lastPt, newPt, spd);
	vTaskDelay(3000);
	action_twist(lastPt, calibratePosition, spd);
	vTaskDelay(200);
	standUp();
}
void danceTurnAround()
{
	int repeats = 10;
	for (int i = 0; i < repeats; i++)
	{
		move_any(0, 0, 5);
	}
	for (int i = 0; i < repeats; i++)
	{
		move_any(0, 0, -5);
	}
	move_any(0, 0, 0);
}
void danceStretchSelf()
{
	const float spd = 2;
	float newPt[4][3] = {{calibratePosition[0][0] + 95, calibratePosition[0][1] - 56, calibratePosition[0][2]},
						 {calibratePosition[1][0], calibratePosition[1][1] + 30, calibratePosition[1][2]},
						 {calibratePosition[2][0], calibratePosition[2][1] + 30, calibratePosition[2][2]},
						 {calibratePosition[3][0] + 95, calibratePosition[3][1] - 56, calibratePosition[3][2]}};
	action_twist(lastPt, newPt, spd);
	vTaskDelay(2000);

	action_twist(lastPt, calibratePosition, spd);
	vTaskDelay(200);
	standUp();
}
void dancePushUp()
{
	int repeats = 5;
	const float spd = 2;
	float newPt[4][3] = {{calibratePosition[0][0], calibratePosition[0][1] + 20, calibratePosition[0][2]},
						 {calibratePosition[1][0] - 80, calibratePosition[1][1] + 10, calibratePosition[1][2]},
						 {calibratePosition[2][0] - 80, calibratePosition[2][1] + 10, calibratePosition[2][2]},
						 {calibratePosition[3][0], calibratePosition[3][1] + 20, calibratePosition[3][2]}};
	action_twist(lastPt, newPt, spd);
	vTaskDelay(200);

	for (int i = 0; i < repeats; i++)
	{
		newPt[0][1] = calibratePosition[0][1] - 20;
		newPt[3][1] = calibratePosition[3][1] - 20;
		action_twist(lastPt, newPt, spd);
		vTaskDelay(200);
		newPt[0][1] = calibratePosition[0][1] + 20;
		newPt[3][1] = calibratePosition[3][1] + 20;
		action_twist(lastPt, newPt, spd);
		vTaskDelay(200);
	}

	action_twist(lastPt, calibratePosition, spd);
}

void danceSayHello()
{
	int repeats = 3;
	const float spd = 3;
	float newPt[4][3] = {{calibratePosition[0][0], calibratePosition[0][1] + 20, calibratePosition[0][2] - 20},
						 {calibratePosition[1][0], calibratePosition[1][1] - 20, calibratePosition[1][2] + 20},
						 {calibratePosition[2][0], calibratePosition[2][1] - 20, calibratePosition[2][2] - 20},
						 {calibratePosition[3][0], calibratePosition[3][1] + 20, calibratePosition[3][2] + 20}};
	action_twist(lastPt, newPt, spd);
	newPt[3][0] = calibratePosition[3][0] + 45;
	newPt[3][1] = calibratePosition[3][1] - 90;
	newPt[3][2] = calibratePosition[3][2] + 10;
	action_twist(lastPt, newPt, spd);
	vTaskDelay(200);
	for (int i = 0; i < repeats; i++)
	{
		newPt[3][2] = calibratePosition[3][2] + 5;
		action_twist(lastPt, newPt, spd);
		newPt[3][2] = calibratePosition[3][2] + 0;
		action_twist(lastPt, newPt, spd);
		newPt[3][2] = calibratePosition[3][2] - 5;
		action_twist(lastPt, newPt, spd);
		vTaskDelay(200);

		newPt[3][2] = calibratePosition[3][2] + 0;
		action_twist(lastPt, newPt, spd);
		newPt[3][2] = calibratePosition[3][2] + 5;
		action_twist(lastPt, newPt, spd);
		newPt[3][2] = calibratePosition[3][2] + 10;
		action_twist(lastPt, newPt, spd);
		vTaskDelay(200);
	}

	newPt[3][0] = calibratePosition[3][0];
	newPt[3][1] = calibratePosition[3][1] + 20;
	newPt[3][2] = calibratePosition[3][2] + 20;
	action_twist(lastPt, newPt, spd);

	action_twist(lastPt, calibratePosition, spd);
}