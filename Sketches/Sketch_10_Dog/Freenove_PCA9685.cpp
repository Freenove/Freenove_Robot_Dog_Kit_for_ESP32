/**
 * @file Freenove_PCA9685.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief PCA9685 driver. Servo pwm driver.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "Freenove_PCA9685.h"
void detect()
{
	byte error, address;
	int nDevices = 0;

	delay(5000);

	Serial.println("Scanning for I2C devices ...");
	for (address = 0x01; address < 0x7f; address++)
	{
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if (error == 0)
		{
			Serial.printf("I2C device found at address 0x%02X\n", address);
			nDevices++;
		}
		else if (error != 2)
		{
			Serial.printf("Error %d at address 0x%02X\n", error, address);
		}
	}
	if (nDevices == 0)
	{
		Serial.println("No I2C devices found");
	}
}

int Freenove_PCA9685::writeReg(uint8_t reg, u8 *value, u8 size_a)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(value, size_a);
	int error = Wire.endTransmission();
	return error;
}

int Freenove_PCA9685::writeReg(uint8_t reg, u8 value)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(value);
	int error = Wire.endTransmission();
	return error;
}

int Freenove_PCA9685::readRegMore(uint8_t reg, u8 *recv, u16 count)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom(address, count);
	// Wire.requestFrom(I2C_Address, count, cmd, 1, true);
	int i = 0;
	while (Wire.available())
	{
		recv[i++] = Wire.read();
		if (i == count)
		{
			break;
		}
	}
	int error = Wire.endTransmission();
	return error;
}

u8 Freenove_PCA9685::readReg(uint8_t reg)
{
	u8 dataRecv[1];
	readRegMore(reg, dataRecv, 1);
	return dataRecv[0];
}

Freenove_PCA9685::Freenove_PCA9685(u8 i2c_addr)
{
	address = i2c_addr;
}

int Freenove_PCA9685::begin()
{
	// Wire.begin();
	Wire.begin(PIN_SDA, PIN_SCL);
	// detect();
	Wire.setClock(100000);
	writeReg(MODE1, 0x00);
	int error = Wire.endTransmission();

	setFreq(50);
	return error;
}

int Freenove_PCA9685::setFreq(int freq)
{
	freq = constrain(freq, 40, 1000);
	u8 prescaleVal = round(25000000 / 4096 / freq - 1);
	u8 oldMode = readReg(MODE1);
	u8 newMode = (oldMode & 0x7F) | 0x10; // sleep
	writeReg(MODE1, newMode);
	writeReg(PRESCALE, prescaleVal);
	writeReg(MODE1, oldMode);
	delayMicroseconds(500);
	writeReg(MODE1, oldMode | 0x80);
	return 0;
}

void Freenove_PCA9685::setPWM(int chn, int on, int off)
{
	writeReg(LED0_ON_L + 4 * chn, on & 0xFF);
	writeReg(LED0_ON_H + 4 * chn, on >> 8);
	writeReg(LED0_OFF_L + 4 * chn, off & 0xFF);
	writeReg(LED0_OFF_H + 4 * chn, off >> 8);
}

void Freenove_PCA9685::setServoMicroSenconds(int chn, int micros)
{
	micros *= 0.20475; // micros = micros * 4095 / 20000;
	setPWM(chn, 0, micros);
}

void Freenove_PCA9685::setServoAngle(int chn, int angle)
{
	//Serial.printf("angle1: %d \n", angle);
	angle = constrain(angle, 5, 175);
	setServoMicroSenconds(chn, map(angle, 0, 180, SERVO_PULSE_MIN, SERVO_PULSE_MAX));
	//Serial.printf("angle2: %d \n", angle);
}

void Freenove_PCA9685::releaseServo(int chn)
{
	setServoMicroSenconds(chn, 0);
}

void Freenove_PCA9685::releaseAllServo()
{
	for (int i = 0; i < 16; i++)
	{
		releaseServo(i);
	}
}
