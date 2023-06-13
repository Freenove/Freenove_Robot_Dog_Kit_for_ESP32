/**
 * @file BuiltInLed.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Built in led service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _BUILTINLED_h
#define _BUILTINLED_h

#include "Public.h"

void setupBuiltInLed();
void setBuiltInLed(bool state);

void task_showBuiltInLed(void *pvParameters);
#endif
