/*
 * 1w_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */




#include <OneWire.h>
#include <DallasTemperature.h>


#include "serial.hpp"

#include "1w_node.hpp"

#define MAX_ATTACHED_DS18B20 16
#define MIN_TEMPERATURE_DELTA 0.2
/**
 * One Wire
 */
#ifndef ONE_WIRE_BUS
#define ONE_WIRE_BUS 3 // Pin where dallase sensor is connected
#else
#endif

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float lastTemperature[MAX_ATTACHED_DS18B20];
int numDallasSensors = 0;
// Initialize temperature message
MyMessage RainMsg(0, V_TEMP);

void setup_onewire(bool present)
{
	wdt_reset();
	// Startup OneWire
	sensors.begin();
	sensors.setResolution(9);
	wdt_reset();

	// Fetch the number of attached temperature sensors
	numDallasSensors = sensors.getDeviceCount();

	DEBUG_PRINT("found ");
	DEBUG_PRINT(numDallasSensors);
	DEBUG_PRINT(F(" onewire temperature device(s) on bus "));
	DEBUG_PRINT_ln(ONE_WIRE_BUS);

	if(present){
		present_onewire();
	}

	wdt_reset();
}

void present_onewire()
{
	wdt_reset();
	// Present all sensors to controller
	int i = 0;
	uint8_t nodeId_orig = _nc.nodeId;
	for (i = 0; i < numDallasSensors && i < MAX_ATTACHED_DS18B20; i++)
	{
		_nc.nodeId = nodeId_orig + 1;
		present(i, S_TEMP);
		wdt_reset();
	}
	_nc.nodeId = nodeId_orig;
}

void loop_onewire()
{
	wdt_reset();
	// Fetch temperatures from Dallas sensors
	sensors.requestTemperatures();

	uint8_t nodeId_orig = _nc.nodeId;

	// Read temperatures and send them to controller
	for (int i = 0; i < numDallasSensors && i < MAX_ATTACHED_DS18B20; i++)
	{

		// Fetch and round temperature to one decimal
		float temperature = static_cast<float>(static_cast<int>((
				getConfig().isMetric ? sensors.getTempCByIndex(i) : sensors.getTempFByIndex(i)) * 10.)) / 10.;

		// Only send data if temperature has changed and no error
		if (abs(lastTemperature[i] - temperature) > MIN_TEMPERATURE_DELTA && temperature != -127.00)
		{
			Serial.print("sending temperature for device ");
			Serial.println(i);
			// Send in the new temperature
			_nc.nodeId = nodeId_orig + 1;
			send(RainMsg.setSensor(i).set(temperature, 1));
			lastTemperature[i] = temperature;
		}
		_nc.nodeId = nodeId_orig;
	}

	_nc.nodeId = nodeId_orig;
}
