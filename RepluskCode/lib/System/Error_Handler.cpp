/*
 * Error_Handler.cpp
 *
 *  Created on: 07.10.2019
 *      Author: harald
 */

#include <Error_Handler.h>
#include <Arduino.h>

void error_handler(const char* file, uint32_t line)
{
#ifdef __x86_64
	printf("Exception in %s, line: %u\n", file, line);
#else
	Serial.printf("Exception in %s, line: %lu\n", file, line);
#endif

	while (1) { }; // infinite loop
}

