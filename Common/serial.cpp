/*
 * printf.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */


#include "serial.hpp"

#include <HardwareSerial.h>
#include <avr/wdt.h>
#include "eeprom_reset.hpp"

int serial_putc( char c, FILE * )
{
  Serial.write( c );

  return c;
}



int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup_serial(bool enable_watchdog)
{

	fdevopen( &serial_putc, 0 );
	if(enable_watchdog)
	{
		wdt_enable(WDTO_8S);
		wdt_reset();
	}

	Serial.print(F("launched, "));
	Serial.print(freeRam());
	Serial.println(F(" bytes free"));

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
				Serial.println(F("Clearing EEPROM"));
				clear_eeprom();
			case 'r':
				Serial.println(F("Resetting"));
				wdt_enable(WDTO_120MS);
				while(true)	{}
				break;
		}
	}
	wdt_reset();
}

