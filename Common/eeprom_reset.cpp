/*
 * eeprom_reset.cpp
 *
 *  Created on: May 19, 2015
 *      Author: goulou
 */


#include <Arduino.h>
#include <EEPROM.h>

#include "eeprom_reset.hpp"
#include "serial.hpp"



void eeprom_reset_check(uint8_t reset_pin)
{
	digitalWrite(reset_pin, LOW);
	pinMode(reset_pin, INPUT);
	unsigned long end = millis()+1000;
	while(millis() < end)
	{
		if(digitalRead(reset_pin) == HIGH)
		{
			unsigned long end2 = millis()+200;
			bool ok = true;
			while(millis() < end2)
			{
				if(digitalRead(reset_pin) == LOW)
				{
					break;
					ok = false;
				}
			}
			if(ok)
			{
				DEBUG_PRINT_ln("RESET PIN HIGH : clearing EEPROM");
				clear_eeprom();
			}
			break;
		}
	}

	DEBUG_PRINT_ln("Not clearing EEPROM");
}

void clear_eeprom()
{
	for (int i=0;i<EEPROM.length();i++)
	{
		EEPROM.write(i, 0xff);
	}
}
