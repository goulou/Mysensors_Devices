/*
 * dhd_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#include <stdint.h>
#include <MySensor.h>

#include "relay_node.hpp"
#include "serial.hpp"

#define MAX_RELAY_COUNT 32
#define BAD_ID	0xFF
#define BAD_PIN	0xFF
uint8_t relay_count;
uint8_t relay_ids[MAX_RELAY_COUNT];
uint8_t relay_pins[MAX_RELAY_COUNT];
uint8_t relay_on_state;
uint8_t relay_off_state;

void present_relays(MySensor& gw);
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

void setup_relay(MySensor& gw, const uint8_t* pins, const uint8_t* ids, uint8_t count, uint8_t on_state, uint8_t off_state, boolean present)
{
	if(count > MAX_RELAY_COUNT)
	{
		Serial.println("TOO MANY RELAYS");
		return;
	}
	relay_count = count;
	relay_on_state = on_state;
	relay_off_state = off_state;

	for(int i=0; i<relay_count; i++)
	{
		relay_ids[i] = pgm_read_byte(&ids[i]);
		relay_pins[i] = pgm_read_byte(&pins[i]);
	}


	if(present)
	{
		present_relays(gw);
	}
}

void present_relays(MySensor& gw)
{
	for(int i=0; i<relay_count; i++)
	{
		int sensor = relay_ids[i];
		int pin = relay_pins[i];
		bool value = gw.loadState(sensor);
		uint8_t state = get_state_for_value(sensor, value);
		Serial.print("Presenting relay ");
		Serial.println(sensor);
		// Register all sensors to gw (they will be created as child devices)
		gw.present(sensor, V_LIGHT);
		// Then set relay pins in output mode
		pinMode(pin, OUTPUT);
		// Set relay to last known state (using eeprom storage)
		digitalWrite(pin, state);
		// Change to V_LIGHT if you use S_LIGHT in presentation below
		MyMessage msg(sensor, V_TRIPPED);
		gw.send(msg.set(value));
	}

	loop_relays(gw);
}

boolean loop_relays(MySensor& gw)
{

	return true;
}

uint8_t get_state_for_value(uint8_t id, uint8_t value)
{
	return value?relay_on_state : relay_off_state;
}

boolean incoming_message_relay(MySensor& gw, const MyMessage &message)
{
	uint8_t id = message.sensor;
	int pin = get_relay_pin_for_id(id);
	if(pin == 0xFF)
	{
		return false;
	}
	// Change relay state
	bool value = value;
	uint8_t state = get_state_for_value(id, value);
	digitalWrite(pin, state);
	// Store state in eeprom
	gw.saveState(message.sensor, state);
	// Write some debug info
	Serial.print("Incoming change for Relay:");
	Serial.print(message.sensor);
	Serial.print(", New status value=");
	Serial.print(value);
	Serial.print(":");
	Serial.print(state);
	Serial.println();


	return true;
}
