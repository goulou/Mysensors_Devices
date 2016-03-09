/*
 * dhd_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */


#include <MySensor.h>
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

void setup_si7021(MySensor& gw, int max_try, boolean present, boolean debug)
{
	wdt_reset();
	print_debug = debug;
	delay(500);
	wdt_reset();
	DEBUG_PRINT_ln("openning SI7021");
	si7021_sensor.begin();

	// Register all sensors to gw (they will be created as child devices)
	failed_count = 0;
	while(si7021_sensor.sensorExists()==false && (max_try==0 || failed_count < max_try))
	{
		wdt_reset();
		DEBUG_PRINT_ln("SI7021 not available");
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
		present_si7021(gw);
	}
	else if(si7021_sensor.sensorExists())
	{
		DEBUG_PRINT_ln("SI7021 available");
	}
	wdt_reset();

}

void present_si7021(MySensor& gw)
{
	if (si7021_sensor.sensorExists() == true)
	{
		DEBUG_PRINT_ln("SI7021 OK : presenting");
		gw.present(CHILD_ID_HUM, S_HUM);
		gw.present(CHILD_ID_TEMP, S_TEMP);
		registered = true;
		failed_count = 0;
	}
	else
	{
		DEBUG_PRINT_ln("SI7021 NOK : not presenting");
		registered = false;
	}

}

boolean loop_si7021(MySensor& gw)
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

		if(isnan(temperature) || isnan(humidity))
		{
			failed_count++;
			DEBUG_PRINT_ln("unable to read si7021");
			if(failed_count >=20)
			{
				while(true)
				{
					digitalWrite(13, LOW);
					gw.sleep(500);
					digitalWrite(13, HIGH);
					gw.sleep(500);
					DEBUG_PRINT_ln("unable to read si7021");
				}
			}
			return false;
		}
		else
		{
			failed_count = 0;

			if (abs(temperature - lastTemp) > MIN_TEMP_DELTA)
			{
				lastTemp = temperature;
				MyMessage msg(CHILD_ID_TEMP, V_TEMP);
				gw.send(msg.set(temperature, 1));
		//		Serial.print("T: ");
		//		DEBUGln(temperature);
			}
			else if(print_debug)
			{
				DEBUG_PRINT_ln("temperature did not change");
			}

			if (abs(humidity - lastHum) > MIN_HUMIDITY_DELTA )
			{
				lastHum = humidity;
				MyMessage msg(CHILD_ID_HUM, V_HUM);
				gw.send(msg.set(humidity, 1));
		//		Serial.print("H: ");
		//		DEBUGln(humidity);
			}
			else if(print_debug)
			{
				DEBUG_PRINT_ln("humidity did not change");
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
