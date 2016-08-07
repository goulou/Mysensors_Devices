/*
 * dhd_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#include "digital_output.hpp"

#include <Arduino.h>
#include <stdint.h>

#include "serial.hpp"

#ifndef MAX_RELAY_COUNT
#define MAX_RELAY_COUNT 32
#endif
#define BAD_ID	0xFF
#define BAD_PIN	0xFF
uint8_t relay_count;
uint8_t relay_ids[MAX_RELAY_COUNT];
uint8_t relay_pins[MAX_RELAY_COUNT];
uint8_t relay_on_state;
uint8_t relay_off_state;

uint8_t get_state_for_value(uint8_t id, uint8_t value);


uint8_t get_relay_id_for_pin(uint8_t pin)
{
	for(int i=0; i<relay_count; i++)
	{
		if(relay_pins[i] == pin)
		{
			return relay_ids[i];
		}
	}

	return BAD_ID;
}

uint8_t get_relay_pin_for_id(uint8_t id)
{
	for(int i=0; i<relay_count; i++)
	{
		if(relay_ids[i] == id)
		{
			return relay_pins[i];
		}
	}

	return BAD_ID;
}

void setup_digital_output(const uint8_t* pins, const uint8_t* ids, uint8_t count, uint8_t on_state, uint8_t off_state, boolean present)
{
	wdt_reset();
	if(count > MAX_RELAY_COUNT)
	{
		Serial.println(F("TOO MANY RELAYS"));
		return;
	}
	relay_count = count;
	relay_on_state = on_state;
	relay_off_state = off_state;

	for(int i=0; i<relay_count; i++)
	{
		relay_ids[i] = pgm_read_byte(&ids[i]);
		relay_pins[i] = pgm_read_byte(&pins[i]);
		int sensor = relay_ids[i];
		int pin = relay_pins[i];
		bool value = loadState(sensor);
		uint8_t state = get_state_for_value(sensor, value);
		// Then set relay pins in output mode
		pinMode(pin, OUTPUT);
		// Set relay to last known state (using eeprom storage)
		digitalWrite(pin, state);
	}


	if(present)
	{
		present_digital_output();
	}
}

void present_digital_output()
{
	wdt_reset();
	for(int i=0; i<relay_count; i++)
	{
		int sensor = relay_ids[i];
		bool value = loadState(sensor);
		DEBUG_PRINT(F("Presenting relay "));
		DEBUG_PRINT_ln(sensor);
		// Register all sensors to gw (they will be created as child devices)
		present(sensor, V_LIGHT, "relay", false);
		// Change to V_LIGHT if you use S_LIGHT in presentation below
		MyMessage msg(sensor, V_TRIPPED);
		send(msg.set(value));
		wdt_reset();
	}

	loop_digital_output();
	wdt_reset();
}

boolean loop_digital_output()
{

	return true;
}

uint8_t get_state_for_value(uint8_t id, uint8_t value)
{
	return value?relay_on_state : relay_off_state;
}

boolean incoming_message_digital_output(const MyMessage &message)
{
	uint8_t id = message.sensor;
	int pin = get_relay_pin_for_id(id);
	if(pin == 0xFF)
	{
		return false;
	}
	// Change relay state
	bool value = message.getBool();
	uint8_t state = get_state_for_value(id, value);
	digitalWrite(pin, state);
	// Store state in eeprom
	saveState(message.sensor, state);
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