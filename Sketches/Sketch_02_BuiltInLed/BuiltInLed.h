#ifndef _BUILTINLED_h
#define _BUILTINLED_h

#include "Arduino.h"

#define LED_BUILT_IN 2

void setupBuiltInLed();
void setBuiltInLed(bool state);

void task_showBuiltInLed(void *pvParameters);
#endif
