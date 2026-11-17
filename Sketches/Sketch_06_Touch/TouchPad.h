#ifndef _TOUCHPAD_h
#define _TOUCHPAD_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "driver/touch_pad.h"

#define PIN_TOUCH_PAD (TOUCH_PAD_NUM3)
#define TOUCH_THRESH_NO_USE (0)
#define TOUCH_THRESH_PERCENT (80)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

void setupTouchPad(void);

void __attribute__((weak)) isr_touchpad(void *arg);
extern void isr_touchpad(void *arg);
void isr_touchpad();
void task_TouchPad(void *pvParameters);
void task_Touch(void *pvParameters);
int getTouch(void);

#endif