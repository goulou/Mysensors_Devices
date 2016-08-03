/*
 * battery_monitored_node.cpp
 *
 *  Created on: May 8, 2015
 *      Author: goulou
 */

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
void battery_monitored_node_setup(uint8_t pin)
{
	battery_pin = pin;
	analogReference(REFERENCE);

	battery_monitored_node_loop();
}

void battery_monitored_node_loop()
{
	wdt_reset();
	Serial.print("3");
	// get the battery Voltage
	pinMode(battery_pin, INPUT);
	Serial.print("4");
	int sensorValue = analogRead(battery_pin);
	Serial.print("5");
	// 1M, 470K divider across battery and using internal ADC ref of 1.1V
	// Sense point is bypassed with 0.1 uF cap to reduce noise at that point
	// ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
	// 3.44/1023 = Volts per bit = 0.003363075
	float batteryV = sensorValue * 0.003363075;

	int MIN_V = 1800;
	int MAX_V = 3000;
	int batteryPcnt = min(map(1000*batteryV, MIN_V, MAX_V, 0, 100), 100);

	Serial.print("6");
	DEBUG_PRINT(F("Batt: "));
	DEBUG_PRINT(batteryV);
	DEBUG_PRINT(" V,");
	wait(50);
	DEBUG_PRINT(batteryPcnt);
	DEBUG_PRINT_ln("%");
	wait(500);
	if (oldBatteryPcnt != batteryPcnt)
	{
		if(abs(batteryPcnt-lastSentBatteryPctn) > 1)
		{
			DEBUG_PRINT_ln(F("rfrsh"));
			wait(500);
			sendBatteryLevel(batteryPcnt);
			lastSentBatteryPctn = batteryPcnt;
		}
		oldBatteryPcnt = batteryPcnt;
	}
	wdt_reset();
}
