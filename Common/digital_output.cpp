/*
 * dhd_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#pragma once

#include "digital_output.hpp"

#include <Arduino.h>
#include <stdint.h>

#include "serial.hpp"

#define BAD_ID	0xFF
#define BAD_PIN	0xFF
const uint8_t relay_pins[OUTPUT_PINS_COUNT] PROGMEM = {OUTPUT_PINS_DEFAULT};
const uint8_t relay_ids [OUTPUT_PINS_COUNT] PROGMEM = {OUTPUT_PINS_IDS};


uint8_t get_state_for_value(uint8_t id, uint8_t value);


uint8_t get_relay_id_for_pin(uint8_t pin)
{
	for(int i=0; i<OUTPUT_PINS_COUNT; i++)
	{
		if(pgm_read_byte(&relay_pins[i]) == pin)
		{
			return pgm_read_byte(&relay_ids[i]);
		}
	}

	return BAD_ID;
}

uint8_t get_index_for_sensor_id(uint8_t id)
{
	for(int i=0; i<OUTPUT_PINS_COUNT; i++)
	{
		if(pgm_read_byte(&relay_ids[i]) == id)
		{
			return i;
		}
	}

	return BAD_ID;
}

uint8_t get_relay_pin_for_id(uint8_t id)
{
	for(int i=0; i<OUTPUT_PINS_COUNT; i++)
	{
		if(pgm_read_byte(&relay_ids[i]) == id)
		{
			return pgm_read_byte(&relay_pins[i]);
		}
	}

	return BAD_ID;
}

void setup_digital_output()
{
	wdt_reset();

	for(int i=0; i<OUTPUT_PINS_COUNT; i++)
	{
		int sensor = pgm_read_byte(&relay_ids[i]);
		int pin = pgm_read_byte(&relay_pins[i]);
		uint8_t state = loadState(sensor);
		// Then set relay pins in output mode
		pinMode(pin, OUTPUT);
		// Set relay to last known state (using eeprom storage)
		digitalWrite(pin, state);
	}
}

void present_digital_output()
{
	wdt_reset();
	for(int i=0; i<OUTPUT_PINS_COUNT; i++)
	{
		int sensor = pgm_read_byte(&relay_ids[i]);
		DEBUG_PRINT(F("Presenting relay "));
		DEBUG_PRINT_ln(sensor);
		// Register all sensors to gw (they will be created as child devices)
		present(sensor, S_BINARY, "relay", false);
		wdt_reset();
	}

}

boolean loop_digital_output()
{
	return true;
}


#ifdef INVERT_OUT
uint8_t INVERT_OUT_ARRAY[INVERT_OUT_COUNT] = {INVERT_OUT};
#endif


uint8_t get_state_for_value(uint8_t index, uint8_t value)
{
#ifdef INVERT_OUT
		for(uint8_t i=0; i<INVERT_OUT_COUNT; i++)
		{
			if(index==INVERT_OUT_ARRAY[i])
			{
				value = value?0:1;
			}
		}
#endif
	return value;
}

uint8_t get_value_for_state(uint8_t index, uint8_t state)
{
#ifdef INVERT_OUT
		for(uint8_t i=0; i<INVERT_OUT_COUNT; i++)
		{
			if(index==INVERT_OUT_ARRAY[i])
			{
				state = state?0:1;
			}
		}
#endif
	return state;
}

boolean incoming_message_digital_output(const MyMessage &message)
{
	uint8_t sensor_id = message.sensor;
	int index = get_index_for_sensor_id(sensor_id);
	int pin = get_relay_pin_for_id(sensor_id);
	if(pin == 0xFF)
	{
		return false;
	}
	// Change relay state
	bool value = message.getBool();
	uint8_t state = get_state_for_value(index, value);
	digitalWrite(pin, state);
	// Store state in eeprom
	saveState(sensor_id, state);
	// Write some debug info
	DEBUG_PRINT(F("Incoming change for Relay:"));
	DEBUG_PRINT(message.sensor);
	DEBUG_PRINT(F(" on pin "));
	DEBUG_PRINT(pin);
	DEBUG_PRINT(F(", New status value="));
	DEBUG_PRINT(value);
	DEBUG_PRINT(F(":"));
	DEBUG_PRINT(state);
	DEBUG_PRINT_ln();


	return true;
}

uint8_t change_digital_output(uint8_t index)
{
	DEBUG_PRINT_ln("Changing output state");
	uint8_t sensor_id = pgm_read_byte(&relay_ids[index]);

	uint8_t state = loadState(sensor_id);
	state = state==0?1:0;

	uint8_t pin = pgm_read_byte(&relay_pins[index]);
	// Change relay state
	digitalWrite(pin, state);
	// Store state in eeprom
	saveState(sensor_id, state);
	MyMessage msg(sensor_id, V_STATUS);
//			Serial.println("send");
	msg.set(get_value_for_state(sensor_id, state));
	wdt_reset();
	send(msg);
	wdt_reset();

	return state;

}

bool set_digital_output(uint8_t index, uint8_t value)
{
	uint8_t sensor_id = pgm_read_byte(&relay_ids[index]);

	uint8_t current_state = loadState(sensor_id);
	uint8_t state = get_state_for_value(index, value);
	if(state == current_state)
		return false;
	DEBUG_PRINT("Setting output state:");
	DEBUG_PRINT(sensor_id);
	DEBUG_PRINT(":");
	DEBUG_PRINT_ln(state);

	uint8_t pin = pgm_read_byte(&relay_pins[index]);
	// Change relay state
	digitalWrite(pin, state);
	// Store state in eeprom
	saveState(sensor_id, state);
	MyMessage msg(sensor_id, V_STATUS);
//			Serial.println("send");
	msg.set(get_value_for_state(sensor_id, state));
	wdt_reset();
	send(msg);
	wdt_reset();

	return true;

}

