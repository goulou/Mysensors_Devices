/*
 * eeprom_reset.cpp
 *
 *  Created on: May 19, 2015
 *      Author: goulou
 */


#include <Arduino.h>
#include <EEPROM.h>

#include "printf.hpp"
#include "eeprom_reset.hpp"

void eeprom_reset_check(uint8_t reset_pin)
{
	pinMode(reset_pin, INPUT);
	unsigned long end = millis()+1000;
	while(millis() < end)
	{
		if(digitalRead(reset_pin) == HIGH)
		{
			DEBUG_PRINT_ln("RESET PIN HIGH : clearing EEPROM");
			for (int i=0;i<512;i++)
			{
				EEPROM.write(i, 0xff);
			}
			break;
		}
	}
	DEBUG_PRINT_ln("Not clearing EEPROM");
}
