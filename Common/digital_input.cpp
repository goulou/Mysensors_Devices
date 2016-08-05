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

#include "digital_input.hpp"
#include "serial.hpp"

#ifndef DIGITAL_INPUT_READ_DELAY_MS
#define DIGITAL_INPUT_READ_DELAY_MS	10
#endif

#ifndef MAX_DIGITAL_INPUT
#define MAX_DIGITAL_INPUT 8
#endif
#define BAD_ID	0xFF
#define BAD_PIN	0xFF
boolean safe_send = false;
uint8_t digital_input_count;
uint8_t digital_input_ids[MAX_DIGITAL_INPUT];
uint8_t digital_input_pins[MAX_DIGITAL_INPUT];
mysensor_sensor digital_input_types[MAX_DIGITAL_INPUT];
uint8_t digital_input_values[MAX_DIGITAL_INPUT];

#ifndef DIGITAL_INPUT_NO_DEBOUNCE
unsigned long digital_input_last_change_time[MAX_DIGITAL_INPUT];
unsigned long last_read;

//we need to define debounce time
#ifndef DIGITAL_INPUT_MIN_CHANGE_TIME_MS
#define DIGITAL_INPUT_MIN_CHANGE_TIME_MS 100
#endif //DIGITAL_INPUT_MIN_CHANGE_TIME_MS

#endif //DIGITAL_INPUT_NO_DEBOUNCE

void present_digital_inputs();

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

void setup_digital_input(const uint8_t* pins, const uint8_t* ids, uint8_t count, boolean present, boolean p_safe_send, const mysensor_sensor* types)
{
	wdt_reset();
	if(count > MAX_DIGITAL_INPUT)
	{
		Serial.println(F("TOO MANY DIGITAL INPUT"));
		return;
	}
	digital_input_count = count;
	safe_send = p_safe_send;

	for(int i=0; i<digital_input_count; i++)
	{
		digital_input_ids[i] = pgm_read_byte(&ids[i]);
		digital_input_pins[i] = pgm_read_byte(&pins[i]);
		digital_input_types[i] = types==NULL?S_MOTION:(mysensor_sensor)pgm_read_byte(&types[i]);

//		int sensor = digital_input_ids[i];
		int pin = digital_input_pins[i];
		pinMode(pin, INPUT);
	}


	if(present)
	{
		present_digital_inputs();
	}
}

void present_digital_inputs()
{
	wdt_reset();
	for(int i=0; i<digital_input_count; i++)
	{
		int sensor = digital_input_ids[i];
		int pin = digital_input_pins[i];
		DEBUG_PRINT(F("Presenting digital input "));
		DEBUG_PRINT_ln(sensor);
		present(sensor, S_MOTION);
		pinMode(pin, INPUT);
		digital_input_values[i] = digitalRead(pin);
		MyMessage msg(sensor, V_TRIPPED);
		send(msg.set(digital_input_values[i]));
		wdt_reset();
	}

#ifndef DIGITAL_INPUT_NO_DEBOUNCE
	last_read = millis();
#endif
	loop_digital_inputs();
	wdt_reset();
}

boolean loop_digital_inputs()
{
	boolean retval = false;
	wdt_reset();
#ifndef DIGITAL_INPUT_NO_DEBOUNCE
	if((unsigned long)(millis() - last_read) < DIGITAL_INPUT_READ_DELAY_MS)
	{
		return false;
	}
	else
	{
		last_read = millis();
	}
	sleep(5);
#endif
	//wait that the buttons are settled
	for(int i=0; i<digital_input_count; i++)
	{
		wdt_reset();
		int sensor = digital_input_ids[i];
		int pin = digital_input_pins[i];
//		Serial.print("c");
		int value = digitalRead(pin);
//		Serial.println("d");
#ifdef DIGITAL_INPUT_NO_DEBOUNCE
		if(value != digital_input_values[i])
		{
			retval = true;
			Serial.print("IO:");
			Serial.println(value);
			wait(500);
			Serial.print("e");
			MyMessage msg(sensor, V_TRIPPED);
			send(msg.set(value));
			if(safe_send)
			{
				wait(200);
				Serial.print("f");
				send(msg.set(value));
				wait(200);
				Serial.print("g");
				send(msg.set(value));
			}
			digital_input_values[i] = value;
			Serial.print("h");
		}
#else
		if(value == digital_input_values[i])
		{
			digital_input_last_change_time[i] = millis();
		}
		else if((unsigned long)(millis() - digital_input_last_change_time[i]) > DIGITAL_INPUT_MIN_CHANGE_TIME_MS)
		{
			retval = true;
			MyMessage msg(sensor, V_TRIPPED);
//			Serial.println("send");
			wdt_reset();
			msg.set(value);
			send(msg);
			if(safe_send)
			{
				wait(100);
				wdt_reset();
				send(msg);
				wait(100);
				wdt_reset();
				send(msg);
			}
			wait(100);
			digital_input_values[i] = value;
			digital_input_last_change_time[i] = millis();
		}
#endif
	}

	wdt_reset();
	return retval;
}

