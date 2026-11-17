/**
 * @file BuiltInLed.h
 * @author zhentao lin@freenove (support@freenove.com)
 * @brief Built in led service.
 * @version v1.2.0
 * @date 2026-11-17
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
