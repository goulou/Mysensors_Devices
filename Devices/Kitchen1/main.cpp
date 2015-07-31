// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

#include <printf.hpp>
#include <dht_node.hpp>
#include <1w_node.hpp>

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

#define HUMIDITY_SENSOR_DIGITAL_PIN 4

MySensor gw;
// Initialize temperature message


void setup()
{
	printf_begin();

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(NULL, AUTO, true);
	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	onewire_node_setup(gw);
	// Register all sensors to gw (they will be created as child devices)

	delay(2000);
	setup_dht(gw, HUMIDITY_SENSOR_DIGITAL_PIN, 10);
}

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();
	Serial.print(".");
	onewire_node_loop(gw);

	unsigned long sleep_time = SLEEP_TIME;
	if(loop_dht(gw) == false)
	{
		sleep_time = SLEEP_TIME/10;
	}

	Serial.print("+");
	//Repeater mode needs not sleeping : process messages instead.
	unsigned long start_millis = millis();
	while( ((unsigned long)(millis() - start_millis)) < sleep_time)
	{
		gw.process();
	}
}

