/*
 * digital_input.cpp
 *
 *  Created on: Mar 9, 2016
 *      Author: goulou
 */




/*
 * battery_monitored_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#include <stdint.h>
#include <MySensor.h>

#include "digital_input.hpp"
#include "serial.hpp"

#ifndef DIGITAL_INPUT_READ_DELAY_MS
#define DIGITAL_INPUT_READ_DELAY_MS	10
#endif

#define MAX_DIGITAL_INPUT 8
#define BAD_ID	0xFF
#define BAD_PIN	0xFF
uint8_t digital_input_count;
uint8_t digital_input_ids[MAX_DIGITAL_INPUT];
uint8_t digital_input_pins[MAX_DIGITAL_INPUT];
uint8_t digital_input_values[MAX_DIGITAL_INPUT];

#ifndef DIGITAL_INPUT_NO_DEBOUNCE
unsigned long digital_input_last_change_time[MAX_DIGITAL_INPUT];
unsigned long last_read;

//we need to define debounce time
#ifndef DIGITAL_INPUT_MIN_CHANGE_TIME_MS
#define DIGITAL_INPUT_MIN_CHANGE_TIME_MS 100
#endif //DIGITAL_INPUT_MIN_CHANGE_TIME_MS

#endif //DIGITAL_INPUT_NO_DEBOUNCE

void present_digital_inputs(MySensor& gw);

uint8_t get_digital_input_id_for_pin(uint8_t pin)
{
	for(int i=0; i<digital_input_count; i++)
	{
		if(digital_input_pins[i] == pin)
		{
			return digital_input_ids[i];
		}
	}

	return BAD_ID;
}

uint8_t get_digital_input_pin_for_id(uint8_t id)
{
	for(int i=0; i<digital_input_count; i++)
	{
		if(digital_input_ids[i] == id)
		{
			return digital_input_pins[i];
		}
	}

	return BAD_ID;
}

void setup_digital_input(MySensor& gw, const uint8_t* pins, const uint8_t* ids, uint8_t count, boolean present)
{
	if(count > MAX_DIGITAL_INPUT)
	{
		Serial.println("TOO MANY DIGITAL INPUT");
		return;
	}
	digital_input_count = count;

	for(int i=0; i<digital_input_count; i++)
	{
		digital_input_ids[i] = pgm_read_byte(&ids[i]);
		digital_input_pins[i] = pgm_read_byte(&pins[i]);
	}


	if(present)
	{
		present_digital_inputs(gw);
	}
}

void present_digital_inputs(MySensor& gw)
{
	for(int i=0; i<digital_input_count; i++)
	{
		int sensor = digital_input_ids[i];
		int pin = digital_input_pins[i];
		Serial.print("Presenting digital input ");
		Serial.println(sensor);
		gw.present(sensor, S_MOTION);
		pinMode(pin, INPUT);
		digital_input_values[i] = digitalRead(pin);
		MyMessage msg(sensor, V_TRIPPED);
		gw.send(msg.set(digital_input_values[i]));
	}

	last_read = millis();
	loop_digital_inputs(gw);
}

boolean loop_digital_inputs(MySensor& gw)
{
#ifndef DIGITAL_INPUT_NO_DEBOUNCE
	if((unsigned long)(millis() - last_read) < DIGITAL_INPUT_READ_DELAY_MS)
	{
		return false;
	}
	else
	{
		last_read = millis();
	}
#endif

	for(int i=0; i<digital_input_count; i++)
	{
		int sensor = digital_input_ids[i];
		int pin = digital_input_pins[i];
		int value = digitalRead(pin);
#ifdef DIGITAL_INPUT_NO_DEBOUNCE
		if(value != digital_input_values[i])
		{
			MyMessage msg(sensor, V_TRIPPED);
			gw.send(msg.set(value));
			digital_input_values[i] = value;
		}
#else
		if(value == digital_input_values[i])
		{
			digital_input_last_change_time[i] = millis();
		}
		else if((unsigned long)(millis() - digital_input_last_change_time[i]) > DIGITAL_INPUT_MIN_CHANGE_TIME_MS)
		{
			MyMessage msg(sensor, V_TRIPPED);
			gw.send(msg.set(value));

			digital_input_values[i] = value;
			digital_input_last_change_time[i] = millis();
		}
#endif
	}

	return true;
}

