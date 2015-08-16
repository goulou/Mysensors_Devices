/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file printf.h
 *
 * Setup necessary to direct stdout to the Arduino Serial library, which
 * enables 'printf'
 */

#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <HardwareSerial.h>

int serial_putc( char c, FILE * );

void printf_begin(void);

#define xstr(s) str(s)
#define str(s) #s

#ifdef DEBUG
#define DEBUG_PRINT_ln(...) do{Serial.println(__VA_ARGS__);Serial.flush();}while(0)
#define DEBUG_PRINT(...)    do{Serial.print(__VA_ARGS__);Serial.flush();}while(0)
#else
#define DEBUG_PRINT_ln(...)
#define DEBUG_PRINT(...)
#endif


#endif // __PRINTF_H__
