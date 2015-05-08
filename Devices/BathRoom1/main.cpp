// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>

#include <printf.hpp>
#include <dht_node.hpp>

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)


/**
 * MySensors
 */
MySensor gw;
boolean receivedConfig = false;
// Initialize temperature message
MyMessage msg(0, V_TEMP);

#define HUMIDITY_SENSOR_DIGITAL_PIN 3

void setup()
{
	printf_begin();

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(NULL, AUTO, true);
	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo("BathRoom1", "1.0");

	delay(200);
	setup_dht(gw, HUMIDITY_SENSOR_DIGITAL_PIN, 10);
}



void loop()
{
	// Process incoming messages (like config from server)
	gw.process();

	loop_dht(gw);

}

