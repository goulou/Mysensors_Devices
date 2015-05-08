/*
 * 1w_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */




#include <MySensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#include "printf.hpp"

#define MAX_ATTACHED_DS18B20 16

/**
 * One Wire
 */
#define ONE_WIRE_BUS 3 // Pin where dallase sensor is connected


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float lastTemperature[MAX_ATTACHED_DS18B20];
int numDallasSensors = 0;
// Initialize temperature message
MyMessage msg(0, V_TEMP);

void onewire_node_setup(MySensor& gw)
{
	// Startup OneWire
	sensors.begin();

	// Fetch the number of attached temperature sensors
	numDallasSensors = sensors.getDeviceCount();

	// Present all sensors to controller
	int i = 0;
	for (i = 0; i < numDallasSensors && i < MAX_ATTACHED_DS18B20; i++)
	{
		gw.present(i, S_TEMP);
	}

	DEBUG_PRINT("registered ");
	DEBUG_PRINT(i);
	DEBUG_PRINT_ln(" onewire temperature device(s)");

}

void onewire_node_loop(MySensor& gw)
{
	// Fetch temperatures from Dallas sensors
	sensors.requestTemperatures();

	// Read temperatures and send them to controller
	for (int i = 0; i < numDallasSensors && i < MAX_ATTACHED_DS18B20; i++)
	{

		// Fetch and round temperature to one decimal
		float temperature = static_cast<float>(static_cast<int>((
				gw.getConfig().isMetric ? sensors.getTempCByIndex(i) : sensors.getTempFByIndex(i)) * 10.)) / 10.;

		// Only send data if temperature has changed and no error
		if (lastTemperature[i] != temperature && temperature != -127.00)
		{
			Serial.print("sending temperature for device ");
			Serial.println(i);
			// Send in the new temperature
			gw.send(msg.setSensor(i).set(temperature, 1));
			lastTemperature[i] = temperature;
		}
	}
}
