// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>

#include <eeprom_reset.hpp>
#include <1w_node.hpp>
#include <dht_node.hpp>
#include <battery_monitored_node.hpp>
#include <printf.hpp>

#include <si7021_node.hpp>

MySensor gw;
unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0



void setup()
{
	Serial.begin(BAUD_RATE);
	int i=0;
	Serial.println("launched");
	printf_begin();

//	eeprom_reset_check(3);

	Serial.println("begin");
	setup_si7021(gw, 10, false);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(NULL, 20);
//	Serial.println("setPALevel");
//	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	Serial.println("sendSketchInfo");
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	Serial.println("setup1w");
	// Startup OneWire
	onewire_node_setup(gw);
	present_si7021(gw);
	battery_monitored_node_setup(gw, BATTERY_SENSE_PIN);
}

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();

	onewire_node_loop(gw);
	loop_si7021(gw);
	battery_monitored_node_loop(gw);

	gw.sleep(SLEEP_TIME);
}

