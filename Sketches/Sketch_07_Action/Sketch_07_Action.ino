#include "Motion.h"

/*
Please note that the following functions are based on the premise that the robot dog has been calibrated.
If your robot dog has not been calibrated yet, calibrate before learning.
Otherwise, the steering gear may be damaged, or the machine action cannot achieve the desired purpose.
*/

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nProgram begin ... ");
  prefs.begin(NMSPC_STORAGE);
  pca.begin();
  pca.releaseAllServo();
  getServoOffsetFromStorage();

  standUp();
  delay(3000);

  const uint8_t repeatCounts = 3;
  const int8_t acts[][2][3] = {
    { { 0, 0, 10 }, { 0, 0, -10 } },  //Rotate horizontally clockwise, counterclockwise horizontal rotation.
    { { 0, 10, 0 }, { 0, -10, 0 } },  //High on the left and low on the right, low on the left and high on the right.
    { { 10, 0, 0 }, { -10, 0, 0 } }   //High at the front and low at the back, low at the front and high at the back.
  };
  for (uint8_t i = 0; i < sizeof(acts) / sizeof(acts[0]); i++) {
    for (int k = 0; k < repeatCounts; k++) {
      for (int j = 0; j < 2; j++) {
        twist_any(acts[i][j][0], acts[i][j][1], acts[i][j][2]);
        delay(20);
      }
    }
  }

  //Rotate horizontally clockwise, and high at the front and low at the back, low at the front and high at the back.
  int8_t x = 0, y = 0, z = 0;
  for (uint8_t j = 0; j < repeatCounts; j++) {
    for (int i = 0; i < 360; i += 30) {
      x = 10 * sin((double)i * PI / 180);
      z = 10 * cos((double)i * PI / 180);
      twist_any(x, y, z);
    }
  }
  twist_any(0, 0, 0);  //Normal standing

  //The whole body goes up and down.
  for (int i = 0; i < repeatCounts; i++) {
    setBodyHeight(BODY_HEIGHT_MIN);
    delay(200);
    setBodyHeight(BODY_HEIGHT_MAX);
    delay(200);
  }

  //Move forward (10*10)mm in the positive direction of the x axis
  for (int i = 0; i < 10; i++) {
    move_any(0, 10, 0);
  }
  //Move back (10*10)mm in the positive direction of the x axis
  for (int i = 0; i < 10; i++) {
    move_any(0, -10, 0);
  }
  
  //Control the robot dog to sit down and disable all servoes to reduce power consumption
  setBodyHeight(BODY_HEIGHT_MIN);
  pca.releaseAllServo();
}

void loop() {
  delay(10000);
}