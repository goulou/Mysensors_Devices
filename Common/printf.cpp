/*
 * printf.c
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */


#include <HardwareSerial.h>
#include <printf.hpp>

int serial_putc( char c, FILE * )
{
  Serial.write( c );

  return c;
}

void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}
