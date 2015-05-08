/*
 * dhd_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */


#include <MySensor.h>
#include <DHT.h>

#include "printf.hpp"

/**
 * DHT
 */
#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1

DHT dht;
float lastTemp;
float lastHum;
boolean metric = true;
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
boolean DHT_registered = false;
int failed_count = 0;

void setup_dht(MySensor& gw, uint8_t pin, int max_try)
{
	dht.setup(pin);

	// Startup OneWire
//	sensors.begin();
	dht.readSensor();
	// Register all sensors to gw (they will be created as child devices)
	failed_count = 0;
	while(isnan(dht.getTemperature()) && (max_try==0 || failed_count < 10))
	{
		DEBUG_PRINT_ln("Temperature is NaN");
		DEBUG_PRINT_ln(dht.getStatusString());
		digitalWrite(13, HIGH);
		delay(200);
		digitalWrite(13, LOW);
		gw.sleep(1000);
		dht.readSensor();
		failed_count ++;
	}


	if (isnan(dht.getTemperature()) == false)
	{
		DEBUG_PRINT_ln("DHT OK : presenting");
		msgHum.setSensor(128);
		gw.present(128, S_HUM);
		msgTemp.setSensor(129);
		gw.present(129, S_TEMP);
		DHT_registered = true;
		failed_count = 0;
	}
	else
	{
		DEBUG_PRINT_ln("DHT NOK : not presenting");
		DHT_registered = false;
	}

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
				gw.send(msgTemp.set(temperature, 1));
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
				gw.send(msgHum.set(humidity, 1));
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
