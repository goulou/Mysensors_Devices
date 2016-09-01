/*
 * dhd_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */


#include <Arduino.h>
#include <dht.h>

#include "serial.hpp"

/**
 * DHT
 */
#define CHILD_ID_HUM 128
#define CHILD_ID_TEMP 129

static dht dht_device;
static float dht_lastTemp;
static float dht_lastHum;
static boolean DHT_registered = false;
static int dht_failed_count = 0;
static uint8_t dht_pin;

void setup_dht_device(uint8_t pin, int max_try, boolean present)
{
	dht_pin = pin;
	wdt_reset();
	DEBUG_PRINT(F("Waiting for DHT on pin "));
	DEBUG_PRINT_ln(pin);
	//You should wait at least 1000ms if a DHT11 is detected
//	sensors.begin();
	int8_t ret = dht_device.read22(pin);
	// Register all sensors to gw (they will be created as child devices)
	dht_failed_count = 0;
	while((isnan(dht_device.temperature)  || ret != DHTLIB_OK) && (max_try==0 || dht_failed_count < max_try) )
	{
		wdt_reset();
		DEBUG_PRINT(F("Temperature is NaN, dht_code="));
		DEBUG_PRINT_ln(ret);
		digitalWrite(13, HIGH);
		delay(200);
		wdt_reset();
		digitalWrite(13, LOW);
		delay(1000);
		DEBUG_PRINT_ln(F("reading"));
		ret = dht_device.read22(dht_pin);
		dht_failed_count ++;
	}

	DEBUG_PRINT(F("temperature OK : "));
	DEBUG_PRINT_ln(dht_device.temperature);
	DEBUG_PRINT(F("humidity       : "));
	DEBUG_PRINT_ln(dht_device.humidity);

	wdt_reset();
	if(present)
	{
		present_dht_device();
	}
	wdt_reset();
}

void present_dht_device()
{
	if (isnan(dht_device.temperature) == false)
	{
		DEBUG_PRINT_ln(F("DHT OK : presenting"));
		present(CHILD_ID_HUM, S_HUM);
		present(CHILD_ID_TEMP, S_TEMP);
		DHT_registered = true;
		dht_failed_count = 0;
	}
	else
	{
		DEBUG_PRINT_ln(F("DHT NOK : not presenting"));
		DHT_registered = false;
	}
}

boolean loop_dht_device()
{
	if (DHT_registered)
	{
		int8_t ret = dht_device.read22(dht_pin);
		float temperature = dht_device.temperature;
		float humidity = dht_device.humidity;

		if(isnan(temperature) || isnan(humidity) || ret != DHTLIB_OK)
		{
			dht_failed_count++;
			DEBUG_PRINT_ln(F("unable to read"));
			DEBUG_PRINT(ret);
			if(dht_failed_count >=20)
			{
				while(true)
				{
					digitalWrite(13, LOW);
					sleep(500);
					digitalWrite(13, HIGH);
					sleep(500);
					DEBUG_PRINT_ln(F("unable to read"));
				}
			}
			return false;
		}
		else
		{
			dht_failed_count = 0;

			if (abs(temperature - dht_lastTemp) > 0.2 || abs(temperature - dht_lastTemp) > 0.2)
			{
				dht_lastTemp = temperature;
				MyMessage msg1(CHILD_ID_TEMP, V_TEMP);
				send(msg1.set(temperature, 1));
				Serial.print(F("T: "));
				DEBUG_PRINT_ln(temperature);

				dht_lastHum = humidity;
				MyMessage msg2(CHILD_ID_HUM, V_HUM);
				send(msg2.set(humidity, 1));
				Serial.print(F("H: "));
				DEBUG_PRINT_ln(humidity);
			}
			else
			{
				DEBUG_PRINT_ln(F("temperature or humidity did not change"));
			}

		}
	}

	return true;
}

float dht_device_get_last_temp()
{
	return dht_lastTemp;
}
