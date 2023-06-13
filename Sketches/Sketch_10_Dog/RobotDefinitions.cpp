/**
 * @file RobotDefinitions.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Definition of robot hardware parameters.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "RobotDefinitions.h"



String getRobotId()
{
    uint64_t chipId = ESP.getEfuseMac();
    String chipIdStr = String((uint32_t)chipId, HEX);
    chipIdStr.toUpperCase();
    // String s = String("Freenove-Dog-") + chipIdStr;
    return chipIdStr;
}

