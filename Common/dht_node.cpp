/*
 * dhd_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */


#include <MySensor.h>
#include <DHT.h>

#include "printf.hpp"

#include "dht_node.hpp"

/**
 * DHT
 */
#define CHILD_ID_HUM 128
#define CHILD_ID_TEMP 129

static DHT dht;
static float lastTemp;
static float lastHum;
static boolean metric = true;
static boolean DHT_registered = false;
static int failed_count = 0;

void setup_dht(MySensor& gw, uint8_t pin, int max_try, boolean present)
{
	dht.setup(pin);

	// Startup OneWire
//	sensors.begin();
	dht.readSensor();
	// Register all sensors to gw (they will be created as child devices)
	failed_count = 0;
	while(isnan(dht.getTemperature()) && (max_try==0 || failed_count < max_try))
	{
		DEBUG_PRINT_ln("Temperature is NaN");
		DEBUG_PRINT_ln(dht.getStatusString());
		digitalWrite(13, HIGH);
		delay(200);
		digitalWrite(13, LOW);
		delay(1000);
		DEBUG_PRINT_ln("reading");
		dht.readSensor();
		failed_count ++;
	}

	DEBUG_PRINT("temperature OK : ");
	DEBUG_PRINT_ln(dht.getTemperature());
	DEBUG_PRINT("humidity       : ");
	DEBUG_PRINT_ln(dht.getHumidity());

	if(present)
	{
		present_dht(gw);
	}
}

void present_dht(MySensor& gw)
{
	if (isnan(dht.getTemperature()) == false)
	{
		DEBUG_PRINT_ln("DHT OK : presenting");
		gw.present(CHILD_ID_HUM, S_HUM);
		gw.present(CHILD_ID_TEMP, S_TEMP);
		DHT_registered = true;
		failed_count = 0;
	}
	else
	{
		DEBUG_PRINT_ln("DHT NOK : not presenting");
		DHT_registered = false;
	}

	loop_dht(gw);
}

boolean loop_dht(MySensor& gw)
{
	if (DHT_registered)
	{
		dht.readSensor();
		float temperature = dht.getTemperature();
		float humidity = dht.getHumidity();

		if(isnan(temperature) || isnan(humidity))
		{
			failed_count++;
			DEBUG_PRINT_ln("unable to read");
			DEBUG_PRINT_ln(dht.getStatusString());
			if(failed_count >=20)
			{
				while(true)
				{
					digitalWrite(13, LOW);
					gw.sleep(500);
					digitalWrite(13, HIGH);
					gw.sleep(500);
					DEBUG_PRINT_ln("unable to read");
				}
			}
			return false;
		}
		else
		{
			failed_count = 0;

			if (temperature != lastTemp)
			{
				lastTemp = temperature;
				if (!metric)
				{
					temperature = dht.toFahrenheit(temperature);
				}
				MyMessage msg(CHILD_ID_TEMP, V_TEMP);
				gw.send(msg.set(temperature, 1));
				Serial.print("T: ");
				DEBUG_PRINT_ln(temperature);
			}
			else
			{
				DEBUG_PRINT_ln("temperature did not change");
			}

			if (humidity != lastHum)
			{
				lastHum = humidity;
				MyMessage msg(CHILD_ID_HUM, V_HUM);
				gw.send(msg.set(humidity, 1));
				Serial.print("H: ");
				DEBUG_PRINT_ln(humidity);
			}
			else
			{
				DEBUG_PRINT_ln("humidity did not change");
			}

		}
	}

	return true;
}

float dht_get_last_temp()
{
	return lastTemp;
}
