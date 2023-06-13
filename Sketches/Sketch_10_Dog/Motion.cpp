/**
 * @file Motion.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Code of motion, Including coordinate angle conversion, walking posture control, body length definition and so on.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "Motion.h"

#define TICK_MS 10 //The time length of each tick, the unit is ms, and the coordinate point is updated every tick.
#define STEP_HEIGHT 15
// define leg length
// L1: Root
#define L1 23.0
// L2: Thigh length
#define L2 55.0
// L3: Calf length ：55+4
#define L3 59.0
// Define active radius, DR = L2+L3-10
#define DR 100.0

/**
 * @brief define the body length and width , between adjacent servo.
 * @LEN_BD: body length / 2 = 136.4 / 2.
 * @WID_BD: body width / 2 = 80 / 2.
 */
constexpr float LEN_BD = 68.2, WID_BD = 40;

/**
 * @brief define the hypotenuse
 *
 */
constexpr float v = sqrt(square(LEN_BD) + square(WID_BD));

/**
 * @brief define the theta
 *
 */
constexpr float theta = atan(WID_BD / LEN_BD);

// The movement speed of the leg end point. Too fast speed will damage the servo. Unit: x mm / 10ms
#define SPEED_MIN 1
#define SPEED_MAX 8
int moveSpeed = 5;

// Height
int bodyHeight = BODY_HEIGHT_DEFAULT;

Freenove_PCA9685 pca = Freenove_PCA9685();

bool isRobotStanding = false;
bool isRobotMoving = false;
// last point legs
float lastPt[4][3];

// legs index: 0: Left front, 1: Left rear, 2: Right rear, 3: Right front.
// float calibratePosition[4][3] = {{0, 99, 10}, {0, 99, 10}, {0, 99, -10}, {0, 99, -10}};
// The default coordinate of quadruped is also the calibration coordinate point
float calibratePosition[4][3] = {{10, 99, 10}, {10, 99, 10}, {10, 99, -10}, {10, 99, -10}};
// Used to save the servo offset, in radians
float servoOffset[4][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
// legAngles ,global variable, Used to save the servo angle ,== abc
float las[4][3];
// The robot origin point position when moving. It will be changed based on the calibration point.
float movingOriginPos[4][3];

void setMoveSpeed(int spd)
{
	moveSpeed = constrain(spd, SPEED_MIN, SPEED_MAX);
}

/**
 * @brief //Move the leg 0, 2, 1, 3 to the target point without moving the body.
 * 
 * @param startPt 
 * @param end__Pt 
 */
void move_leg_to_point_directly(float (*startPt)[3], float (*end__Pt)[3])
{ 
	//Triangle track. The parameters represent the coordinates of the foot point.
	float lenghtP2P = 0;
	for (int i = 0; i < 4; i++)
	{
		float tmpLen = sqrt(square(end__Pt[i][0] - startPt[i][0]) + square(end__Pt[i][1] - startPt[i][1]) + square(end__Pt[i][2] - startPt[i][2]));
		lenghtP2P = lenghtP2P > tmpLen ? lenghtP2P : tmpLen;
	}
	int stepTicks = round(lenghtP2P / moveSpeed) + 2;

	float k = -2 * STEP_HEIGHT / stepTicks; //, v = STEP_LENGTH / TICKS; //H = kT / 2, S = vT
	float trackPt[4][3];
	u8 a = 0, c = 2; //, b = 1, d = 3
	for (int i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			a = 0;
			// b = 1;
			c = 2;
			// d = 3;
		}
		else
		{
			a = 1;
			// b = 0;
			c = 3;
			// d = 2;
		}
		//First move leg 0 and 2. Then move leg 1 and 3.
		for (int t = 0; t <= stepTicks; t++)
		{
			for (int j = 0; j < 4; j++)
			{
				trackPt[j][0] = startPt[j][0] + t * (end__Pt[j][0] - startPt[j][0]) / stepTicks;
				if (t < stepTicks / 2)
				{
					trackPt[j][1] = startPt[j][1] + k * t;
				}
				else
				{
					trackPt[j][1] = startPt[j][1] - k * t + k * t;
				}
				trackPt[j][2] = startPt[j][2] + t * (end__Pt[j][2] - startPt[j][2]) / stepTicks;
			}
			cooToA(trackPt[a], las[a]);
			cooToA(trackPt[c], las[c]);
			updateLegx(a);
			updateLegx(c);
			delay(TICK_MS);
		}
	}
	for (int j = 0; j < 4; j++)
	{
		lastPt[j][0] = trackPt[j][0];
		lastPt[j][1] = trackPt[j][1];
		lastPt[j][2] = trackPt[j][2];
		// Serial.println(">>>>>");
		// Serial.println(String("lastPt: ") + String(lastPt[j][0]) + String(" ") + String(lastPt[j][1]) + String(" ") + String(lastPt[j][2]));
		// Serial.println("<<<<<");
	}
}
void resumeStanding()
{
	if (!isRobotStanding)
	{
		setBodyHeight(BODY_HEIGHT_DEFAULT);
		standUp();
		isRobotStanding = true;
	}
}
void standUp()
{
	for (int j = 0; j < 4; j++)
	{
		lastPt[j][0] = calibratePosition[j][0];
		lastPt[j][1] = calibratePosition[j][1];
		lastPt[j][2] = calibratePosition[j][2];
	}
	move_leg_to_point_directly(lastPt, calibratePosition);
	isRobotStanding = true;
}

/**
 * @brief The body has displacement. The starting point is any position. Leg 0, 2, 1, 3 takes one step.
 * 
 * @param startPt 
 * @param end__Pt 
 * @param spd 
 */
void move_step_by_step(float (*startPt)[3], float (*end__Pt)[3], int spd)
{
	//Trapezoidal quadrilateral track. The parameters represent the coordinates of the foot point. Walking gait.
	float lenghtP2P = 0;
	//Judge the two legs.
	for (int i = 0; i < 2; i++)
	{
		float tmpLen = sqrt(square(end__Pt[i][0] - startPt[i][0]) + square(end__Pt[i][1] - startPt[i][1]) + square(end__Pt[i][2] - startPt[i][2]));
		lenghtP2P = lenghtP2P > tmpLen ? lenghtP2P : tmpLen;
	}
	spd = constrain(spd, SPEED_MIN, SPEED_MAX);
	int stepTicks = round(lenghtP2P / spd) + 3;
	// Serial.println(String("stepTicks: ") + String(stepTicks));
	float k = -3 * STEP_HEIGHT / stepTicks; //, v = STEP_LENGTH / stepTicks; //H = kT / 2, S = vT
	float trackPt[4][3];
	float tmpEnd__Pt[4][3]; // tmpStartPt[4][3],
	// float startPt[4][3] = { {-20,99,10},{0,99,10},{-20,99,-10},{0,99,-10} };
	// float endPt[4][3] = { {20,99,10},{0,99,10},{20,99,-10},{0,99,-10} };
	//First move leg 0 and 2, p1->p2, forward. Then move leg 1 and 3, p2->p1, reverse.
	u8 a = 0, b = 1, c = 2, d = 3;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			tmpEnd__Pt[i][j] = end__Pt[i][j];
		}
	}
	// Serial.println(">====>");
	// for (u8 e = 0; e < 4; e++)
	// {
	// 	Serial.println(String("startPt: ") + String(startPt[e][0]) + String(" ") + String(startPt[e][1]) + String(" ") + String(startPt[e][2]));
	// }
	// Serial.println("====");
	// for (u8 e = 0; e < 4; e++)
	// {
	// 	Serial.println(String("tmpEnd__Pt: ") + String(tmpEnd__Pt[e][0]) + String(" ") + String(tmpEnd__Pt[e][1]) + String(" ") + String(tmpEnd__Pt[e][2]));
	// }
	// Serial.println("<====<");
	// TICKS;
	for (int i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			a = 0;
			b = 1;
			c = 2;
			d = 3;
		}
		else
		{
			a = 1;
			b = 0;
			c = 3;
			d = 2;
		}
		for (u8 i = 0; i < 3; i++)
		{
			if (i == 1)
				continue;
			end__Pt[a][i] = tmpEnd__Pt[a][i];
			end__Pt[c][i] = tmpEnd__Pt[c][i];
			end__Pt[b][i] = 2 * movingOriginPos[b][i] - tmpEnd__Pt[b][i]; // End point of the reverse motion leg.
			end__Pt[d][i] = 2 * movingOriginPos[d][i] - tmpEnd__Pt[d][i]; // End point of the reverse motion leg.
		}
		// Serial.println(">---->");
		// for (u8 e = 0; e < 4; e++) {
		//	Serial.println(String("startPt: ") + String(startPt[e][0]) + String(" ") + String(startPt[e][1]) + String(" ") + String(startPt[e][2]));
		// }
		// Serial.println("----");
		// for (u8 e = 0; e < 4; e++) {
		//	Serial.println(String("end__Pt: ") + String(end__Pt[e][0]) + String(" ") + String(end__Pt[e][1]) + String(" ") + String(end__Pt[e][2]));
		// }
		// Serial.println("<----<");

		for (int t = 0; t <= stepTicks; t++)
		{
			//First calculate the X-axis coordinates of the four points, of which leg 0 and 2 moves forward, and 1 3 and 0 2 move relatively.
			trackPt[a][0] = startPt[a][0] + (end__Pt[a][0] - startPt[a][0]) * t / stepTicks; //
			trackPt[c][0] = startPt[c][0] + (end__Pt[c][0] - startPt[c][0]) * t / stepTicks; //
			trackPt[b][0] = startPt[b][0] + (end__Pt[b][0] - startPt[b][0]) * t / stepTicks; // Legs 1 and 0 are reversed to each other.
			trackPt[d][0] = startPt[d][0] + (end__Pt[d][0] - startPt[d][0]) * t / stepTicks; // Legs 2 and 3 are reversed to each other.
			//Calculate the Y-axis coordinates, in which leg 0 2 lifts and leg 1 3 does not.
			if (t < stepTicks / 3)
			{
				trackPt[a][1] = end__Pt[a][1] + k * t;
				trackPt[c][1] = end__Pt[c][1] + k * t;
			}
			else if (t < stepTicks * 2 / 3)
			{
			}
			else
			{
				trackPt[a][1] = end__Pt[a][1] - k * t + k * stepTicks;
				trackPt[c][1] = end__Pt[c][1] - k * t + k * stepTicks;
			}
			trackPt[b][1] = end__Pt[b][1];
			trackPt[d][1] = end__Pt[d][1];

			// The Z axis calculation method is the same as the X axis.
			trackPt[a][2] = startPt[a][2] + (end__Pt[a][2] - startPt[a][2]) * t / stepTicks; //
			trackPt[c][2] = startPt[c][2] + (end__Pt[c][2] - startPt[c][2]) * t / stepTicks; //
			trackPt[b][2] = startPt[b][2] + (end__Pt[b][2] - startPt[b][2]) * t / stepTicks; // Legs 1 and 0 are reversed to each other.
			trackPt[d][2] = startPt[d][2] + (end__Pt[d][2] - startPt[d][2]) * t / stepTicks; // Legs 2 and 3 are reversed to each other.
			cooToA_All(trackPt, las);
			updateServoAngle();
			delay(TICK_MS);
			// for (int j = 0; j < 4; j++) {
			//	Serial.println(String("trackPt: ") + String(trackPt[j][0]) + String(" ") + String(trackPt[j][1]) + String(" ") + String(trackPt[j][2]));
			// }
			// Serial.println();
		}
		// delay(1000);

		for (u8 i = 0; i < 4; i++)
		{
			for (u8 j = 0; j < 3; j++)
			{
				// end__Pt[i][j] = -tmpEnd__Pt[i][j];//tmpStartPt[i][j];
				startPt[i][j] = trackPt[i][j];
			}
		}

		// Serial.println(">>>>>>");
		// for (u8 e = 0; e < 4; e++) {
		//	Serial.println(String("startPt: ") + String(startPt[e][0]) + String(" ") + String(startPt[e][1]) + String(" ") + String(startPt[e][2]));
		// }
		// Serial.println("----");
		// for (u8 e = 0; e < 4; e++) {
		//	Serial.println(String("end__Pt: ") + String(end__Pt[e][0]) + String(" ") + String(end__Pt[e][1]) + String(" ") + String(end__Pt[e][2]));
		// }
		// Serial.println("<<<<<<");
	}
	// Serial.println(">>>>>");
	for (int j = 0; j < 4; j++)
	{
		lastPt[j][0] = trackPt[j][0];
		lastPt[j][1] = trackPt[j][1];
		lastPt[j][2] = trackPt[j][2];
		// Serial.println(String("lastPt: ") + String(lastPt[j][0]) + String(" ") + String(lastPt[j][1]) + String(" ") + String(lastPt[j][2]));
	}
	// Serial.println("<<<<<");
}

// alpha is the moving direction, with the x-axis direction as 0 degrees, counterclockwise as positive, clockwise as negative. The unit is radians.
// StepLength is step distance.
/**
 * @brief Walk command, any direction, any step length, any spin angle, any speed
 *
 * @param alpha is the moving direction, with the x-axis direction as 0 degrees, counterclockwise as positive, clockwise as negative, and the unit is angle, [0-360]. The x direction is the direction of forward movement.
 * @param stepLength The length of each step (<=20).
 * @param gama Spin angle, in-situ rotation, positive counterclockwise, negative clockwise, in degrees. [0-360].
 * @param spd Movement speed, unit：mm / 10ms.  [1,8]
 */
void move_any(int alpha, float stepLength, int gama, int spd)
{
	stepLength /= 2;
	float newPt[4][3];
	float delta_x, delta_z;
	delta_x = stepLength * cos(alpha * PI / 180); //Portrait
	delta_z = stepLength * sin(alpha * PI / 180); //Horizontal
	float pt[] = {delta_x, 0, delta_z};			  //Target body coordinates. After each action ends, the body coordinates are regarded as the origin.

	// memcpy(movingOriginPos, calibratePosition, sizeof(calibratePosition));
	// Body translation coefficient
	const float kx_t = 0.3, kz_t = 0.15;
	// Body inclination coefficient
	const float kx_p = 0.34, kz_p = 0.2;
	//Calculate the center point of each foot when moving. Based on the calibration point, the center of gravity is offset to the moving direction, and the body is inclined to the moving direction. When moving, the landing point of the step is symmetrical to the center point.
	for (int j = 0; j < 4; j++)
	{
		movingOriginPos[j][0] = calibratePosition[j][0] - delta_x * kx_t;
		movingOriginPos[j][2] = calibratePosition[j][2] - delta_z * kz_t;
	}
	movingOriginPos[0][1] = calibratePosition[0][1] - delta_x * kx_p - delta_z * kz_p;
	movingOriginPos[1][1] = calibratePosition[1][1] + delta_x * kx_p - delta_z * kz_p;
	movingOriginPos[2][1] = calibratePosition[2][1] + delta_x * kx_p + delta_z * kz_p;
	movingOriginPos[3][1] = calibratePosition[3][1] - delta_x * kx_p + delta_z * kz_p;

	float c = gama * PI / 180;
	float v_x_sin_theta_p_c = v * sin(theta + c);
	float v_x_sin_theta_m_c = v * sin(theta - c);
	float v_x_cos_theta_p_c = v * cos(theta + c);
	float v_x_cos_theta_m_c = v * cos(theta - c);

	newPt[0][0] = movingOriginPos[0][0] + pt[0] + v_x_cos_theta_p_c - LEN_BD;
	newPt[1][0] = movingOriginPos[1][0] + pt[0] - v_x_cos_theta_m_c + LEN_BD;
	newPt[2][0] = movingOriginPos[2][0] + pt[0] - v_x_cos_theta_p_c + LEN_BD;
	newPt[3][0] = movingOriginPos[3][0] + pt[0] + v_x_cos_theta_m_c - LEN_BD;

	newPt[0][1] = movingOriginPos[0][1];
	newPt[1][1] = movingOriginPos[1][1];
	newPt[2][1] = movingOriginPos[2][1];
	newPt[3][1] = movingOriginPos[3][1];

	newPt[0][2] = movingOriginPos[0][2] + pt[2] + v_x_sin_theta_p_c - WID_BD;
	newPt[1][2] = movingOriginPos[1][2] + pt[2] + v_x_sin_theta_m_c - WID_BD;
	newPt[2][2] = movingOriginPos[2][2] + pt[2] - v_x_sin_theta_p_c + WID_BD;
	newPt[3][2] = movingOriginPos[3][2] + pt[2] - v_x_sin_theta_m_c + WID_BD;

	move_step_by_step(lastPt, newPt, spd);

	// memcpy(calibratePosition, tmpCalibrationPos, sizeof(calibratePosition));
}

/**
 * @brief //Twist the body without moving the body.
 *
 * @param startPt Starting point
 * @param end__Pt Target point
 * @param tickLength The longest distance of each tick. The larger the distance, the faster the movement speed. The default is 5 and the unit is mm.
 * @param isContainedOffset Whether to include calibration information, the default is included.
 */
void action_twist(float (*startPt)[3], float (*end__Pt)[3], float tickLength, bool isContainedOffset)
{
	//The parameter represents the foot point coordinates.
	float trackPt[4][3];
	float lenghtP2P = 0;
	for (int i = 0; i < 4; i++)
	{
		float tmpLen = sqrt(square(end__Pt[i][0] - startPt[i][0]) + square(end__Pt[i][1] - startPt[i][1]) + square(end__Pt[i][2] - startPt[i][2]));
		lenghtP2P = lenghtP2P > tmpLen ? lenghtP2P : tmpLen;
	}
	int stepTicks = round(lenghtP2P / tickLength) + 2;
	//The four legs move together starting from the calibration position.
	for (int t = 0; t <= stepTicks; t++)
	{
		for (int j = 0; j < 4; j++)
		{
			trackPt[j][0] = startPt[j][0] + t * (end__Pt[j][0] - startPt[j][0]) / stepTicks;
			trackPt[j][1] = startPt[j][1] + t * (end__Pt[j][1] - startPt[j][1]) / stepTicks;
			trackPt[j][2] = startPt[j][2] + t * (end__Pt[j][2] - startPt[j][2]) / stepTicks;
			// Serial.println(">---->");
			// for (int j = 0; j < 4; j++) {
			// Serial.println(String("trackPt: ") + String(trackPt[j][0]) + String(" ") + String(trackPt[j][1]) + String(" ") + String(trackPt[j][2]));
			// }
			// Serial.println();
			// Serial.println("<----<");
		}
		cooToA_All(trackPt, las);
		updateServoAngle(isContainedOffset);
		vTaskDelay(TICK_MS);
	}

	for (int j = 0; j < 4; j++)
	{
		lastPt[j][0] = trackPt[j][0];
		lastPt[j][1] = trackPt[j][1];
		lastPt[j][2] = trackPt[j][2];
		// Serial.println(">>>>>");
		// Serial.println(String("lastPt: ") + String(lastPt[j][0]) + String(" ") + String(lastPt[j][1]) + String(" ") + String(lastPt[j][2]));
		// Serial.println("<<<<<");
	}
}

void twist_any(int alpha, int beta, int gama)
{
	float a = alpha * PI / 180;
	float b = beta * PI / 180;
	float c = gama * PI / 180;

	float v_x_sin_theta_p_c = v * sin(theta + c);
	float v_x_sin_theta_m_c = v * sin(theta - c);
	float v_x_cos_theta_p_c = v * cos(theta + c);
	float v_x_cos_theta_m_c = v * cos(theta - c);

	float l_x_sin_a = LEN_BD * sin(a);
	float w_x_sin_b = WID_BD * sin(b);

	float newPt[4][3];

	newPt[0][0] = calibratePosition[0][0] - v_x_cos_theta_p_c + LEN_BD;
	newPt[1][0] = calibratePosition[1][0] + v_x_cos_theta_m_c - LEN_BD;
	newPt[2][0] = calibratePosition[2][0] + v_x_cos_theta_p_c - LEN_BD;
	newPt[3][0] = calibratePosition[3][0] - v_x_cos_theta_m_c + LEN_BD;

	newPt[0][1] = calibratePosition[0][1] + l_x_sin_a + w_x_sin_b;
	newPt[1][1] = calibratePosition[1][1] - l_x_sin_a + w_x_sin_b;
	newPt[2][1] = calibratePosition[2][1] - l_x_sin_a - w_x_sin_b;
	newPt[3][1] = calibratePosition[3][1] + l_x_sin_a - w_x_sin_b;

	newPt[0][2] = calibratePosition[0][2] - v_x_sin_theta_p_c + WID_BD;
	newPt[1][2] = calibratePosition[1][2] - v_x_sin_theta_m_c + WID_BD;
	newPt[2][2] = calibratePosition[2][2] + v_x_sin_theta_p_c - WID_BD;
	newPt[3][2] = calibratePosition[3][2] + v_x_sin_theta_m_c - WID_BD;

	// Serial.println(">>>>>");
	// for (int j = 0; j < 4; j++) {
	//	Serial.println(String("newPt: ") + String(newPt[j][0]) + String(" ") + String(newPt[j][1]) + String(" ") + String(newPt[j][2]));
	// }
	// Serial.println("<<<<<");
	action_twist(lastPt, newPt, 5);
	// move_body_step_by_step(lastPt, newPt);
}
void setBodyHeight(int h)
{
	float newPt[4][3];
	h = constrain(h, BODY_HEIGHT_MIN, BODY_HEIGHT_MAX);
	bodyHeight = h;
	newPt[0][0] = calibratePosition[0][0];
	newPt[1][0] = calibratePosition[1][0];
	newPt[2][0] = calibratePosition[2][0];
	newPt[3][0] = calibratePosition[3][0];

	newPt[0][1] = h;
	newPt[1][1] = h;
	newPt[2][1] = h;
	newPt[3][1] = h;

	newPt[0][2] = calibratePosition[0][2];
	newPt[1][2] = calibratePosition[1][2];
	newPt[2][2] = calibratePosition[2][2];
	newPt[3][2] = calibratePosition[3][2];
	action_twist(lastPt, newPt, 5);
}

void setBeforeCalibrationHeight(int h)
{
	float newPt[4][3];
	h = constrain(h, BODY_HEIGHT_MIN, BODY_HEIGHT_MAX);
	newPt[0][0] = calibratePosition[0][0];
	newPt[1][0] = calibratePosition[1][0];
	newPt[2][0] = calibratePosition[2][0];
	newPt[3][0] = calibratePosition[3][0];

	newPt[0][1] = h;
	newPt[1][1] = h;
	newPt[2][1] = h;
	newPt[3][1] = h;

	newPt[0][2] = calibratePosition[0][2];
	newPt[1][2] = calibratePosition[1][2];
	newPt[2][2] = calibratePosition[2][2];
	newPt[3][2] = calibratePosition[3][2];
	action_twist(lastPt, newPt, 5, false);
}

void setServoToInstallationPosition()
{
	for (int i = 0; i < 16; i++)
	{
		pca.setServoAngle(i, 90);
	}
}
void setServoToBeforeCalibrationPosition()
{
	float ofs[3];
	for (int n = 0; n < 4; n++)
	{
		cooToA(calibratePosition[n][0], calibratePosition[n][1], calibratePosition[n][2], ofs);
		las[n][0] = ofs[0];
		las[n][1] = ofs[1];
		las[n][2] = ofs[2];
		updateLegxWithoutOffset(n);
	}
}

void moveOneLegToPointDirectly(uint8_t n, bool b, float(*startPt), float(*end__Pt))
{
	//Calculate the distance between two points.
	float lenghtP2P = sqrt(square(end__Pt[0] - startPt[0]) + square(end__Pt[1] - startPt[1]) + square(end__Pt[2] - startPt[2]));
	//Calculate the required motion periods, ticks.
	int stepTicks = round(lenghtP2P / 5) + 2;
	// Serial.println(String("stepTicks: ") + String(stepTicks));
	float trackPt[3];
	//Move according to the track
	for (int t = 0; t <= stepTicks; t++)
	{
		for (int j = 0; j < 3; j++)
		{
			trackPt[j] = startPt[j] + t * (end__Pt[j] - startPt[j]) / stepTicks;
		}
		cooToA(trackPt, las[n]);
		b ? updateLegx(n) : updateLegxWithoutOffset(n);
		delay(TICK_MS);
	}
	//Record end point position.
	for (int j = 0; j < 3; j++)
	{
		lastPt[n][j] = trackPt[j];
		// Serial.println(">>>>>");
		// Serial.println(String("lastPt: ") + String(lastPt[j][0]) + String(" ") + String(lastPt[j][1]) + String(" ") + String(lastPt[j][2]));
		// Serial.println("<<<<<");
	}
}

void moveAllLegToPointDirectly(bool b, float (*startPt)[3], float (*end__Pt)[3])
{
	float lenghtP2P = 0;
	for (int i = 0; i < 4; i++)
	{
		float tmpLen = sqrt(square(end__Pt[i][0] - startPt[i][0]) + square(end__Pt[i][1] - startPt[i][1]) + square(end__Pt[i][2] - startPt[i][2]));
		lenghtP2P = lenghtP2P > tmpLen ? lenghtP2P : tmpLen;
	}
	int stepTicks = round(lenghtP2P / 5) + 2;

	float trackPt[4][3];

	for (int t = 0; t <= stepTicks; t++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 3; i++)
			{
				trackPt[j][i] = startPt[j][i] + t * (end__Pt[j][i] - startPt[j][i]) / stepTicks;
			}
		}
		cooToA_All(trackPt, las);
		updateServoAngle();
		delay(TICK_MS);
	}
	for (int j = 0; j < 4; j++)
	{
		lastPt[j][0] = trackPt[j][0];
		lastPt[j][1] = trackPt[j][1];
		lastPt[j][2] = trackPt[j][2];
		// Serial.println(">>>>>");
		// Serial.println(String("lastPt: ") + String(lastPt[j][0]) + String(" ") + String(lastPt[j][1]) + String(" ") + String(lastPt[j][2]));
		// Serial.println("<<<<<");
	}
}

void updateLegx(u8 x)
{
	switch (x)
	{
	case 0:
		pca.setServoAngle(0, las[0][0] + servoOffset[0][0]);
		pca.setServoAngle(1, las[0][1] + servoOffset[0][1]);
		pca.setServoAngle(2, las[0][2] + servoOffset[0][2]);
		// Serial.println(String("las0: ") + String(las[0][0] + servoOffset[0][0]) + String(" ") + String(las[0][1] + servoOffset[0][1]) + String(" ") + String(las[0][2] + servoOffset[0][2]));
		break;
	case 1:
		pca.setServoAngle(7, las[1][0] + servoOffset[1][0]);
		pca.setServoAngle(6, las[1][1] + servoOffset[1][1]);
		pca.setServoAngle(5, las[1][2] + servoOffset[1][2]);
		// Serial.println(String("las1: ") + String(las[1][0] + servoOffset[1][0]) + String(" ") + String(las[1][1] + servoOffset[1][1]) + String(" ") + String(las[1][2] + servoOffset[1][2]));
		break;
	case 2:
		pca.setServoAngle(8, las[2][0] + servoOffset[2][0]);
		pca.setServoAngle(9, 180 - las[2][1] - servoOffset[2][1]);
		pca.setServoAngle(10, 180 - las[2][2] - servoOffset[2][2]);
		// Serial.println(String("las2: ") + String(las[2][0] + servoOffset[2][0]) + String(" ") + String(180 - las[2][1] - servoOffset[2][1]) + String(" ") + String(180 - las[2][2] - servoOffset[2][2]));
		break;
	case 3:
		pca.setServoAngle(15, las[3][0] + servoOffset[3][0]);
		pca.setServoAngle(14, 180 - las[3][1] - servoOffset[3][1]);
		pca.setServoAngle(13, 180 - las[3][2] - servoOffset[3][2]);
		// Serial.println(String("las3: ") + String(las[3][0] + servoOffset[3][0]) + String(" ") + String(180 - las[3][1] - servoOffset[3][1]) + String(" ") + String(180 - las[3][2] - servoOffset[3][2]));
		break;
	default:
		break;
	}
}

void updateLegxWithoutOffset(u8 n)
{
	switch (n)
	{
	case 0:
		pca.setServoAngle(0, las[0][0]);
		pca.setServoAngle(1, las[0][1]);
		pca.setServoAngle(2, las[0][2]);
		// Serial.println(">>>1:");
		// Serial.println(String(las[0][0]) + String(" ") + String(las[0][1]) + String(" ") + String(las[0][2]));
		// Serial.println(">>>2:");
		// Serial.println(String(las[0][0] + servoOffset[0][0]) + String(" ") + String(las[0][1] + servoOffset[0][1]) + String(" ") + String(las[0][2] + servoOffset[0][2]));
		break;
	case 1:
		pca.setServoAngle(7, las[1][0]);
		pca.setServoAngle(6, las[1][1]);
		pca.setServoAngle(5, las[1][2]);
		// Serial.println(">>>1:");
		// Serial.println(String(las[1][0]) + String(" ") + String(las[1][1]) + String(" ") + String(las[1][2]));
		// Serial.println(">>>2:");
		// Serial.println(String(las[1][0] + servoOffset[1][0]) + String(" ") + String(las[1][1] + servoOffset[1][1]) + String(" ") + String(las[1][2] + servoOffset[1][2]));
		break;
	case 2:
		pca.setServoAngle(8, las[2][0]);
		pca.setServoAngle(9, 180 - las[2][1]);
		pca.setServoAngle(10, 180 - las[2][2]);
		// Serial.println(">>>1:");
		// Serial.println(String(las[2][0]) + String(" ") + String(180 - las[2][1]) + String(" ") + String(180 - las[2][2]));
		// Serial.println(">>>2:");
		// Serial.println(String(las[2][0] + servoOffset[2][0]) + String(" ") + String(180 - las[2][1] - servoOffset[2][1]) + String(" ") + String(180 - las[2][2] - servoOffset[2][2]));
		break;
	case 3:
		pca.setServoAngle(15, las[3][0]);
		pca.setServoAngle(14, 180 - las[3][1]);
		pca.setServoAngle(13, 180 - las[3][2]);
		// Serial.println(">>>1:");
		// Serial.println(String(las[3][0]) + String(" ") + String(180 - las[3][1]) + String(" ") + String(180 - las[3][2]));
		// Serial.println(">>>2:");
		// Serial.println(String(las[3][0] + servoOffset[3][0]) + String(" ") + String(180 - las[3][1] - servoOffset[3][1]) + String(" ") + String(180 - las[3][2] - servoOffset[3][2]));
		break;
	default:
		break;
	}
}

void updateServoAngle(bool b)
{
	if (b)
	{
		updateLegx(0);
		updateLegx(1);
		updateLegx(2);
		updateLegx(3);
	}
	else
	{
		updateLegxWithoutOffset(0);
		updateLegxWithoutOffset(1);
		updateLegxWithoutOffset(2);
		updateLegxWithoutOffset(3);
	}
}

void cooToA_All(float (*Pt)[3], float (*ag)[3])
{
	// for (u8 i = 0; i < 4; i++) {
	//	cooToA(Pt[i], ag[i]);
	// }
	cooToA(Pt[0], ag[0]);
	cooToA(Pt[1], ag[1]);
	cooToA(Pt[2], ag[2]);
	cooToA(Pt[3], ag[3]);
}

void cooToA(float x, float y, float z, float *abc)
{
	float xyz[] = {x, y, z};
	cooToA(xyz, abc);
}

void cooToA(float *xyz, float *abc)
{
	float a, b, c, d, x, y, z, k;

	x = xyz[0];
	y = xyz[1];
	z = xyz[2];

	// Serial.println(String(x) + String(" ") + String(y) + String(" ") + String(z));

	float x_3 = 0, x_4, x_5 = 0, l23 = 0, w = 0, v = 0;
	a = PI / 2 - atan2(z, y);
	x_3 = 0;		   // Intersection point of L1 and L2: x
	x_4 = L1 * sin(a); // Intersection point of L1 and L2: y
	x_5 = L1 * cos(a); // Intersection point of L1 and L2: z

	d = sqrt(pow((z - x_5), 2) + pow((y - x_4), 2) + pow((x - x_3), 2));
	if (d > DR)
	{
		k = DR / d;
		x = k * (x - x_3) + x_3;
		y = k * (y - x_4) + x_4;
		z = k * (z - x_5) + x_5;
	}

	l23 = sqrt(pow((z - x_5), 2) + pow((y - x_4), 2) + pow((x - x_3), 2));
	// l23 = constrain(l23, 0, L2 + L3);
	w = (x - x_3) / l23;
	v = (L2 * L2 + l23 * l23 - L3 * L3) / (2 * L2 * l23);
	b = asin(w) - acos(v);
	c = PI - acos((pow(L2, 2) + pow(L3, 2) - pow(l23, 2)) / (2 * L3 * L2));
	a = a / PI * 180;
	b = b / PI * 180;
	c = c / PI * 180;
	b = map(b, -90, 90, 180, 0);
	abc[0] = a;
	abc[1] = b;
	abc[2] = c;
	// printf("abc: %f, %f, %f, %f, %f, %f, %f \n", a, b, c, x, y, z, l23);
	//   Serial.println(String(a) + String(" ") + String(b) + String(" ") + String(c));
}
extern PreferencesPro prefs;
void getServoOffsetFromStorage()
{

	// If there is a problem with the read length, it indicates the board is new and has never been calibrated. Then set all offsets to 0.0.
	// Serial.printf("len : %d, offset size: %d \n\n", prefs.getBytesLength(KEY_SERVO_OFFSET), sizeof(servoOffset));
	if (prefs.getBytesLength(KEY_SERVO_OFFSET) != sizeof(servoOffset))
	{
		prefs.put(KEY_SERVO_OFFSET, servoOffset);
		// Serial.println("new borad! init the servo offset.");
	}
	else
	{
		prefs.get(KEY_SERVO_OFFSET, servoOffset);
		// for (int i = 0; i < 4; i++)
		// {
		// 	for (int j = 0; j < 3; j++)
		// 	{
		// 		Serial.print(" ");
		// 		Serial.print(servoOffset[i][j]);
		// 	}
		// 	Serial.println(" ");
		// }
	}
}
