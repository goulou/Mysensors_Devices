/*
 * dhd_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */


#include <MySensors.h>
#include <SI7021.h>

#include "si7021_node.hpp"
#include "serial.hpp"

/**
 * Temp+Hum
 */
#define CHILD_ID_HUM 128
#define CHILD_ID_TEMP 129

SI7021 si7021_sensor;

static float lastTemp;
static float lastHum;
static boolean metric = true;
static boolean registered = false;
static int failed_count = 0;
static int print_debug = true;

#ifndef MIN_TEMP_DELTA
#define MIN_TEMP_DELTA 0.2
#endif
#ifndef MIN_HUMIDITY_DELTA
#define MIN_HUMIDITY_DELTA 1
#endif

void setup_si7021(int max_try, boolean present, boolean debug)
{
	wdt_reset();
	print_debug = debug;
	delay(500);
	wdt_reset();
	DEBUG_PRINT_ln(F("openning SI7021"));
	si7021_sensor.begin();

	// Register all sensors to gw (they will be created as child devices)
	failed_count = 0;
	while(si7021_sensor.sensorExists()==false && (max_try==0 || failed_count < max_try))
	{
		wdt_reset();
		DEBUG_PRINT_ln(F("SI7021 not available"));
		digitalWrite(13, HIGH);
		delay(200);
		digitalWrite(13, LOW);
		delay(1000);
		wdt_reset();

		si7021_sensor.begin();
		failed_count ++;
	}


	if(present)
	{
		present_si7021();
	}
	else if(si7021_sensor.sensorExists())
	{
		DEBUG_PRINT_ln(F("SI7021 available"));
	}
	wdt_reset();

}

void present_si7021()
{
	wdt_reset();
	if (si7021_sensor.sensorExists() == true)
	{
		DEBUG_PRINT_ln(F("SI7021 OK : presenting"));
		present(CHILD_ID_TEMP, S_TEMP);
		wdt_reset();
		present(CHILD_ID_HUM, S_HUM);
		registered = true;
		failed_count = 0;
	}
	else
	{
		DEBUG_PRINT_ln(F("SI7021 NOK : not presenting"));
		registered = false;
	}
	wdt_reset();
}

boolean loop_si7021(bool force_send)
{
	wdt_reset();
	if (registered)
	{
		float temperature;
		if(metric)
		{
			temperature = si7021_sensor.getCelsiusHundredths()/100.0;
		}
		else
		{
			temperature = si7021_sensor.getFahrenheitHundredths()/100.0;
		}
		float humidity = si7021_sensor.getHumidityPercent();
		humidity = max(0, min(humidity, 100.0));
		if(isnan(temperature) || isnan(humidity))
		{
			failed_count++;
			DEBUG_PRINT_ln(F("unable to read si7021"));
			if(failed_count >=20)
			{
				while(true)
				{
					wdt_reset();
					digitalWrite(13, LOW);
					sleep(500);
					digitalWrite(13, HIGH);
					sleep(500);
					DEBUG_PRINT_ln(F("unable to read si7021"));
				}
			}
			return false;
		}
		else
		{
			failed_count = 0;

			if (force_send || abs(temperature - lastTemp) > MIN_TEMP_DELTA || abs(humidity - lastHum) > MIN_HUMIDITY_DELTA )
			{
				lastTemp = temperature;
				lastHum = humidity;

				wdt_reset();
				MyMessage msg1(CHILD_ID_TEMP, V_TEMP);
				send(msg1.set(temperature, 1));

				wdt_reset();
				MyMessage msg2(CHILD_ID_HUM, V_HUM);
				send(msg2.set(humidity, 1));
			}
			else if(print_debug)
			{
				DEBUG_PRINT_ln(F("temp&hum not sent"));
			}
		}
	}

	wdt_reset();
	return true;
}

float si7021_get_last_temp()
{
	return lastTemp;
}
