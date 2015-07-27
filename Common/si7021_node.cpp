/*
 * dhd_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */


#include <MySensor.h>
#include <SI7021.h>

#include "printf.hpp"
#include "si7021_node.hpp"

/**
 * Temp+Hum
 */
#define CHILD_ID_HUM 128
#define CHILD_ID_TEMP 129

SI7021 si7021_sensor;

float lastTemp;
float lastHum;
boolean metric = true;
boolean registered = false;
int failed_count = 0;

void setup_si7021(MySensor& gw, int max_try, boolean present)
{
	si7021_sensor.begin();

	// Register all sensors to gw (they will be created as child devices)
	failed_count = 0;
	while(isnan(si7021_sensor.sensorExists()) && (max_try==0 || failed_count < 10))
	{
		DEBUG_PRINT_ln("SI7021 not available");
		digitalWrite(13, HIGH);
		delay(200);
		digitalWrite(13, LOW);
		delay(1000);

		si7021_sensor.begin();
		failed_count ++;
	}

	if(present)
	{
		present_si7021(gw);
	}
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

			if (temperature != lastTemp)
			{
				lastTemp = temperature;
				MyMessage msg(CHILD_ID_TEMP, V_TEMP);
				gw.send(msg.set(temperature, 1));
		//		Serial.print("T: ");
		//		DEBUGln(temperature);
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
		//		Serial.print("H: ");
		//		DEBUGln(humidity);
			}
			else
			{
				DEBUG_PRINT_ln("humidity did not change");
			}

		}
	}

	return true;
}

float get_last_temp()
{
	return lastTemp;
}