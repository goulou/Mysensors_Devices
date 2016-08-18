/*
 * default_mapping.hpp
 *
 *  Created on: Aug 7, 2016
 *      Author: goulou
 */

#pragma once

#ifndef NO_DEFAULT_MAPPING
//DEFAULT VALUES
#if !(defined(BOARD_V1) || defined(BOARD_V2))
	#ifdef __AVR_ATmega328P__
		#define BOARD_V1
	#elif defined(__AVR_ATmega2560__)
		#define BOARD_MEGA
	#endif
#endif


#ifdef BOARD_V1

#define EEPROM_RESET_PIN 4
#define INPUT_PINS_DEFAULT 3, 6, 8, A1
#define OUTPUT_PINS_DEFAULT 7, 5, A3, A2

#elif defined(BOARD_V2)

#define INPUT_PINS_DEFAULT 2, 3, 6, 5
#define OUTPUT_PINS_DEFAULT A1, 8, 7, A2
#define EEPROM_RESET_PIN A3

#elif defined(BOARD_MEGA)

#define EEPROM_RESET_PIN	13

#endif

#endif //NO_DEFAULT_MAPPING
