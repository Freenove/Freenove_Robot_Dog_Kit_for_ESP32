/**
 * @file TaskCommandService.h
 * @author zhentao lin@freenove (support@freenove.com)
 * @brief Non-motion command execution server.
 * @version v1.2.0
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _TASKCOMMANDSERVICE_h
#define _TASKCOMMANDSERVICE_h

#include "Public.h"


extern bool isLegalController;

void task_CommandService(void *pvParameters);



#endif

