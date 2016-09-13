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
		#ifndef BOARD_MEGA
			#define BOARD_MEGA
		#endif
	#endif
#endif


#ifdef BOARD_V1

  #define EEPROM_RESET_PIN 4

  #ifdef WITH_ONEWIRE
    #define ONEWIRE_BUS	PD5
    #define DHT_PIN PD7

    #define OUTPUT_PINS_DEFAULT A3, A2
    #define OUTPUT_PINS_COUNT 2
    #define OUTPUT_PINS_IDS 16, 17

  #else //!WITH ONEWIRE

    #define OUTPUT_PINS_DEFAULT 7, 5, A3, A2
    #define OUTPUT_PINS_COUNT 4
    #define OUTPUT_PINS_IDS 16, 17, 18, 19

  #endif // WITH_ONEWIRE

  #define INPUT_PINS_DEFAULT 3, 6, 8, A1
  #define INPUT_PINS_COUNT 4
  #define INPUT_PINS_IDS 64, 65, 66, 67

  #define INPUT_PINS_TYPES S_DOOR, S_MOTION, S_MOTION, S_MOTION

#elif defined(BOARD_V2)

  #define EEPROM_RESET_PIN A3

  #ifdef WITH_ONEWIRE

    #define ONEWIRE_BUS	PD5
    #undef DHT_PIN

    #define INPUT_PINS_DEFAULT 2, 3, 6
    #define INPUT_PINS_COUNT 3
    #define INPUT_PINS_IDS 64, 65, 66

    #define INPUT_PINS_TYPES S_DOOR, S_MOTION, S_MOTION

  #else //!WITH ONEWIRE

    #define INPUT_PINS_DEFAULT 2, 3, 6, 5
    #define INPUT_PINS_COUNT 4
    #define INPUT_PINS_IDS 64, 65, 66, 67
    #define INPUT_PINS_TYPES S_DOOR, S_MOTION, S_MOTION, S_MOTION


  #endif //WITH ONEWIRE

  #define OUTPUT_PINS_DEFAULT A1, 8, 7, A2
  #define OUTPUT_PINS_COUNT 4
  #define OUTPUT_PINS_IDS 16, 17, 18, 19

#elif defined(BOARD_MEGA)

#define EEPROM_RESET_PIN	13

#define INPUT_PINS_DEFAULT 38, 40, 42, 44, 39, 41, 43, 45, 46, 48, 47, 49
#define INPUT_PINS_COUNT 12
#define INPUT_PINS_IDS 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75
#define INPUT_PINS_TYPES S_BINARY, S_BINARY, S_BINARY, S_BINARY, S_BINARY, S_BINARY, S_BINARY, S_BINARY, S_BINARY, S_BINARY, S_BINARY, S_BINARY

#define OUTPUT_PINS_DEFAULT 22, 24, 26, 28, 23, 25, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37
#define OUTPUT_PINS_COUNT 16
#define OUTPUT_PINS_IDS 16, 17, 18, 19, 20, 21, 22, 23, 32, 33, 34, 35, 36, 37, 38, 39

#endif

#endif //NO_DEFAULT_MAPPING
