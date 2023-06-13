/**
 * @file MessageParser.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Message parser class.
 * @version v1.0.0
 * @date 2022-04-15
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#ifndef _MESSAGEPARSER_h
#define _MESSAGEPARSER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define COMMANDS_COUNT_MAX 8		//The maximum size of the command + parameter contained in the message.
#define INTERVAL_CHAR '#'

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

class MessageParser
{
private:


	
public:
	String inputStringTemp = "";
	String inputCommandArray[COMMANDS_COUNT_MAX];
	int paramters[COMMANDS_COUNT_MAX], paramterCount = 0; //The number of parameters not including the command word.
	char commandChar;
	bool isNewMessage = true;

	MessageParser();
	void parser(String msg);
	void clearParameters();
};

#endif
