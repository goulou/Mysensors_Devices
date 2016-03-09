/*
 * printf.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */


#include "serial.hpp"

#include <HardwareSerial.h>
#include <wdt.h>
#include "eeprom_reset.hpp"

int serial_putc( char c, FILE * )
{
  Serial.write( c );

  return c;
}


void setup_serial()
{
	while(Serial.available()) Serial.read();
	fdevopen( &serial_putc, 0 );
	wdt_enable(WDTO_8S);
	wdt_reset();
}

void loop_serial()
{
	wdt_reset();
	if(Serial.available())
	{
		int c = Serial.read();
		switch(c)
		{
			case -1:
				return;
			case 'e':
				Serial.println("Clearing EEPROM");
				clear_eeprom();
			case 'r':
				Serial.println("Resetting");
				wdt_enable(WDTO_120MS);
				while(true)	{}
				break;
		}
	}
	wdt_reset();
}

