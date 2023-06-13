/**
 * @file Motion.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Code of motion, Including coordinate angle conversion, walking posture control, body length definition and so on.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _MOTION_h
#define _MOTION_h

#include "Public.h"

void setMoveSpeed(int spd);
void move_leg_to_point_directly(float (*startPt)[3], float (*endPt)[3]);
void cooToA_All(float (*Pt)[3], float (*ag)[3]);
void resumeStanding();
void standUp();
// void move_step_by_step(float (*startPt)[3], float (*end__Pt)[3]);
void move_step_by_step(float (*startPt)[3], float (*end__Pt)[3], int spd = 5);
// void move_any(int alpha, float stepLength, int gama);
void move_any(int alpha, float stepLength, int gama, int spd = 5);
// void action_twist(float (*startPt)[3], float (*end__Pt)[3], float tickLength);
void action_twist(float (*startPt)[3], float (*end__Pt)[3], float tickLength, bool isContainedOffset = true);
void twist_any(int alpha, int beta, int gama);
void setBodyHeight(int h);
void setBeforeCalibrationHeight(int h);
void setServoToInstallationPosition();
void setServoToBeforeCalibrationPosition();
void moveOneLegToPointDirectly(uint8_t n, bool b, float(*startPt), float(*end__Pt));
void updateLegx(u8 x);
void updateLegxWithoutOffset(u8 n);
void updateServoAngle(bool b = true);
void cooToA_All(float (*Pt)[3], float (*ag)[3]);
void cooToA(float x, float y, float z, float *abc);
void cooToA(float *xyz, float *abc);
void getServoOffsetFromStorage();

#endif
