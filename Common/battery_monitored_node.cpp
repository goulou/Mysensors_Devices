/*
 * battery_monitored_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

#include <MySensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#include "battery_monitored_node.hpp"
#include "serial.hpp"


#ifdef INTERNAL1V1
#define REFERENCE INTERNAL1V1
#elif defined(INTERNAL)
#define REFERENCE INTERNAL
#else
#error "No battery reference available"
#endif

/*
 * Battery measure
 */
int oldBatteryPcnt = 0;
int lastSentBatteryPctn = 0;

uint8_t battery_pin;
void battery_monitored_node_setup(MySensor& gw, uint8_t pin)
{
	battery_pin = pin;
	analogReference(REFERENCE);

	battery_monitored_node_loop(gw);
}

void battery_monitored_node_loop(MySensor& gw)
{
	wdt_reset();
	// get the battery Voltage
	int sensorValue = analogRead(battery_pin);

	// 1M, 470K divider across battery and using internal ADC ref of 1.1V
	// Sense point is bypassed with 0.1 uF cap to reduce noise at that point
	// ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
	// 3.44/1023 = Volts per bit = 0.003363075
	float batteryV = sensorValue * 0.003363075;
	int batteryPcnt = sensorValue / 10;

	DEBUG_PRINT("Battery Voltage: ");
	DEBUG_PRINT(batteryV);
	DEBUG_PRINT_ln(" V");

	DEBUG_PRINT("Battery percent: ");
	DEBUG_PRINT(batteryPcnt);
	DEBUG_PRINT_ln(" %");

	if (oldBatteryPcnt != batteryPcnt)
	{
		if(abs(batteryPcnt-lastSentBatteryPctn) > 1)
		{
			gw.sendBatteryLevel(batteryPcnt);
			lastSentBatteryPctn = batteryPcnt;
		}
		oldBatteryPcnt = batteryPcnt;
	}
	wdt_reset();
}
