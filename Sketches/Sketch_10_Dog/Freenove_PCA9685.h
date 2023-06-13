/**
 * @file Freenove_PCA9685.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief PCA9685 driver. Servo pwm driver.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _FREENOVE_PCA9685_h
#define _FREENOVE_PCA9685_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <math.h>

#define PIN_SDA 13         //Define SDA pins
#define PIN_SCL 14         //Define SCL pins

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

//#define Registers / etc.
#define MODE1			 0x00
#define MODE2			 0x01
#define SUBADR1			 0x02
#define SUBADR2			 0x03
#define SUBADR3			 0x04
#define PRESCALE		 0xFE
#define LED0_ON_L		 0x06
#define LED0_ON_H		 0x07
#define LED0_OFF_L		 0x08
#define LED0_OFF_H		 0x09
#define ALLLED_ON_L		 0xFA
#define ALLLED_ON_H		 0xFB
#define ALLLED_OFF_L	 0xFC
#define ALLLED_OFF_H	 0xFD

#define SERVO_PULSE_MIN		500
#define SERVO_PULSE_MAX		2500

class Freenove_PCA9685 {
private:
	u8 address = 0x40;		//default i2c address
	int writeReg(uint8_t reg, u8 *value, u8 size_a);
	int writeReg(uint8_t reg, u8 value);
	int readRegMore(uint8_t reg, u8 *recv, u16 count);
	u8 readReg(uint8_t reg);
public:
	Freenove_PCA9685(u8 i2c_addr = 0x40);
	int begin();
	int setFreq(int freq);
	void setPWM(int chn, int on, int off);
	void setServoMicroSenconds(int chn, int micros);
	void setServoAngle(int chn, int angle);
	void releaseServo(int chn);
	void releaseAllServo();
};

#endif

