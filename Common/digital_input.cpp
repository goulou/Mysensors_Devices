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
#define DIGITAL_INPUT_READ_DELAY_MS	0
#endif

#define BAD_ID	0xFF
#define BAD_PIN	0xFF

const uint8_t digital_input_pins[INPUT_PINS_COUNT] PROGMEM = {INPUT_PINS_DEFAULT};
const uint8_t digital_input_ids [INPUT_PINS_COUNT] PROGMEM = {INPUT_PINS_IDS};
const mysensor_sensor digital_input_types [INPUT_PINS_COUNT] PROGMEM = {INPUT_PINS_TYPES};
uint8_t digital_input_values[INPUT_PINS_COUNT];

#ifdef INPUT_DEBOUNCE
//unsigned long digital_input_last_change_time[INPUT_PINS_COUNT];
//unsigned long last_read;

//we need to define debounce time
#ifndef DIGITAL_INPUT_MIN_CHANGE_TIME_MS
#define DIGITAL_INPUT_MIN_CHANGE_TIME_MS 100
#endif //DIGITAL_INPUT_MIN_CHANGE_TIME_MS

#endif //DIGITAL_INPUT_NO_DEBOUNCE

#ifdef PULLUPS
uint8_t PULLUPS_ARRAY[PULLUPS_COUNT] = {PULLUPS};
#endif

#ifdef INVERT_IN
uint8_t INVERT_IN_ARRAY[INVERT_IN_COUNT] = {INVERT_IN};
#endif

#ifdef ONCHANGE_IN
uint8_t ONCHANGE_IN_ARRAY[ONCHANGE_COUNT] = {ONCHANGE_IN};
uint8_t ONCHANGE_OUT_ARRAY[ONCHANGE_COUNT] = {ONCHANGE_OUT};
#include "digital_output.hpp"
#endif

#ifdef ONSET_IN
uint8_t ONSET_IN_ARRAY[ONSET_COUNT] =  {ONSET_IN};
uint8_t ONSET_OUT_ARRAY[ONSET_COUNT] = {ONSET_OUT};
#include "digital_output.hpp"
#endif

void present_digital_inputs();

uint8_t get_digital_input_id_for_pin(uint8_t pin)
{
	for(int i=0; i<INPUT_PINS_COUNT; i++)
	{
		if(pgm_read_byte(&digital_input_pins[i]) == pin)
		{
			return pgm_read_byte(&digital_input_ids[i]);
		}
	}

	return BAD_ID;
}

uint8_t get_digital_input_pin_for_id(uint8_t id)
{
	for(int i=0; i<INPUT_PINS_COUNT; i++)
	{
		if(pgm_read_byte(&digital_input_ids[i]) == id)
		{
			return pgm_read_byte(&digital_input_pins[i]);
		}
	}

	return BAD_ID;
}

void setup_digital_input()
{
	wdt_reset();

	for(int i=0; i<INPUT_PINS_COUNT; i++)
	{
		int pin = pgm_read_byte(&digital_input_pins[i]);
		bool pullup = false;
#ifdef PULLUPS
		pullup = false;
		for(uint8_t pup_id=0; pup_id<PULLUPS_COUNT; pup_id++)
		{
			if(i==PULLUPS_ARRAY[pup_id])
			{
				pullup = true;
			}
		}
#endif
		if(pullup)
		{
			pinMode(pin, INPUT_PULLUP);
		}
		else
		{
			pinMode(pin, INPUT);
		}
	}
}

void present_digital_inputs()
{
	wdt_reset();
	for(int i=0; i<INPUT_PINS_COUNT; i++)
	{
		int sensor = pgm_read_byte(&digital_input_ids[i]);
		DEBUG_PRINT(F("Presenting digital input "));
		DEBUG_PRINT_ln(sensor);
		present(sensor, S_MOTION);
		wdt_reset();
	}

#ifndef DIGITAL_INPUT_NO_DEBOUNCE
//	last_read = millis() - (DIGITAL_INPUT_READ_DELAY_MS+1);
#endif
	wdt_reset();
}

void value_changed(uint8_t idx, uint8_t value)
{
	uint8_t sensor_id = pgm_read_byte(&digital_input_ids[idx]);
	MyMessage msg(sensor_id, V_TRIPPED);
//			Serial.println("send");
	wdt_reset();
	msg.set(value);
	send(msg);
#ifdef INPUT_SAFESEND
		wait(100);
		wdt_reset();
		send(msg);
		wait(100);
		wdt_reset();
		send(msg);
#endif
//			wait(100);
	digital_input_values[idx] = value;

#ifdef ONCHANGE_IN
		for(uint8_t i=0; i<ONCHANGE_COUNT; i++)
		{
			if(idx==ONCHANGE_IN_ARRAY[i])
			{
				change_digital_output(ONCHANGE_OUT_ARRAY[i]);
			}
		}
#endif
#ifdef ONSET_IN
		for(uint8_t i=0; i<ONSET_COUNT; i++)
		{
			if(idx==ONSET_IN_ARRAY[i])
			{
				set_digital_output(ONSET_OUT_ARRAY[i], value);
			}
		}
#endif

}

uint8_t get_input_value(uint8_t index)
{
	int pin = pgm_read_byte(&digital_input_pins[index]);
	int value = digitalRead(pin);
#ifdef INVERT_IN
		for(uint8_t i=0; i<INVERT_IN_COUNT; i++)
		{
			if(index==INVERT_IN_ARRAY[i])
			{
				value = value?0:1;
			}
		}
#endif
	return value;
}


#ifdef INPUT_DEBOUNCE
uint8_t get_input_value_debounced(uint8_t index, uint8_t previous_value)
{
	uint8_t value = get_input_value(index);
	if(value == previous_value)
	{
//		digital_input_last_change_time[index] = millis();
		return value;
	}
	else
	{
		unsigned long start = millis();
		while((unsigned long)(millis() - start) < DIGITAL_INPUT_MIN_CHANGE_TIME_MS)
		{
			value = get_input_value(index);
			if(value == previous_value)
			{
				return value;
			}
			wdt_reset();
		}
		return value;
	}
//	if(value == digital_input_values[i])
//	{
//		digital_input_last_change_time[i] = millis();
//	}
//	else if((unsigned long)(millis() - digital_input_last_change_time[i]) > DIGITAL_INPUT_MIN_CHANGE_TIME_MS)
//	{
//		value_changed(i, value);
//		retval = true;
//		digital_input_last_change_time[i] = millis();
//	}

}
#endif

boolean loop_digital_inputs()
{
	boolean retval = false;
	wdt_reset();
#ifdef INPUT_DEBOUNCE
//	if((unsigned long)(millis() - last_read) < DIGITAL_INPUT_READ_DELAY_MS)
//	{
//		return false;
//	}
//	else
//	{
//		last_read = millis();
//	}
#ifdef INPUT_SAFESEND
	sleep(5);
#endif

#endif
	//wait that the buttons are settled
	for(int i=0; i<INPUT_PINS_COUNT; i++)
	{
		wdt_reset();
//		Serial.print("c");
#ifdef INPUT_DEBOUNCE
		int value = get_input_value_debounced(i, digital_input_values[i]);
#else
		int value = get_input_value(i);
#endif
		if(value != digital_input_values[i])
		{
			Serial.print("IO:");
			Serial.println(value);
			value_changed(i, value);

			digital_input_values[i] = value;
		}

		//		Serial.println("d");
//#ifdef INPUT_DEBOUNCE
//		if(value == digital_input_values[i])
//		{
//			digital_input_last_change_time[i] = millis();
//		}
//		else if((unsigned long)(millis() - digital_input_last_change_time[i]) > DIGITAL_INPUT_MIN_CHANGE_TIME_MS)
//		{
//			value_changed(i, value);
//			retval = true;
//			digital_input_last_change_time[i] = millis();
//		}
//#else
//		if(value != digital_input_values[i])
//		{
//			retval = true;
//			Serial.print("IO:");
//			Serial.println(value);
//			value_changed(i, value);
//
//			digital_input_values[i] = value;
//			Serial.print("h");
//		}
//#endif
	}

	wdt_reset();
	return retval;
}

