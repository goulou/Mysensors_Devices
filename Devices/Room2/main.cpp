// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>

#include <1w_node.hpp>
#include <battery_monitored_node.hpp>
#include <printf.hpp>

MySensor gw;
unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0

void setup()
{
	Serial.begin(115200);
	Serial.println("launched");
	printf_begin();

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin();
	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo("Room2", "1.0");

	// Startup OneWire
	onewire_node_setup(gw);
	battery_monitored_node_setup(gw, BATTERY_SENSE_PIN);
}

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();

	onewire_node_loop(gw);
	battery_monitored_node_loop(gw);

	gw.sleep(SLEEP_TIME);
}

