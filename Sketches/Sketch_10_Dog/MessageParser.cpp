/**
 * @file MessageParser.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Message parser class.
 * @version v1.0.0
 * @date 2022-04-15
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "MessageParser.h"

MessageParser::MessageParser()
{
}

void MessageParser::parser(String msg)
{
	clearParameters();
	//Serial.print("mqMotion.length : ");
	//Serial.print(mqMotion.length());
	inputStringTemp = msg; //mqMotion.out();
	//Serial.print(" inputStringTemp : ");
	//Serial.println(inputStringTemp);
	for (u8 i = 0; i < COMMANDS_COUNT_MAX; i++)
	{ //The incoming command parameter ends with a # sign, otherwise the last one will not be recognized.
		int index = inputStringTemp.indexOf(INTERVAL_CHAR);
		//Serial.println(index);
		if (index < 0)
		{
			break;
		}
		paramterCount = i; //
		inputCommandArray[i] = inputStringTemp.substring(0, index);
		inputStringTemp = inputStringTemp.substring(index + 1);
		paramters[i] = inputCommandArray[i].toInt();
	}
	//Serial.print(inputCommandArray[0]);
	//Serial.print(" , paramterCount : ");
	//Serial.println(paramterCount);
	//Serial.println(String(paramters[1]) + String(" ") + String(paramters[2]) + String(" ") + String(paramters[3]));
	commandChar = inputCommandArray[0].charAt(0);
	isNewMessage = true;
}

void MessageParser::clearParameters()
{
	//Serial.print(isNewMessage);
	if (isNewMessage)
	{
		isNewMessage = false;
		commandChar = 0;
		//Serial.println("Clear paramters 2 ");
		for (int i = 0; i < COMMANDS_COUNT_MAX; i++)
		{ //Clear parameter table.
			inputCommandArray[i] = "";
			paramters[i] = 0;
		}
	}
}
