// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

#include <eeprom_reset.hpp>
#include <dht_node.hpp>
#include <1w_node.hpp>

#include <si7021_node.hpp>
#include <serial.hpp>

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

#define HUMIDITY_SENSOR_DIGITAL_PIN 7

MySensor gw;
// Initialize temperature message


void setup()
{
	Serial.begin(BAUD_RATE);
	setup_serial();

	Serial.println("launched");

	eeprom_reset_check(3);

	Serial.println("begin");
	setup_si7021(gw, 5, false);
	setup_dht(gw, HUMIDITY_SENSOR_DIGITAL_PIN, 5, false);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(NULL, 0xA2, true, GATEWAY_ADDRESS);
//	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	setup_onewire(gw);
	present_si7021(gw);
	present_dht(gw);
	// Register all sensors to gw (they will be created as child devices)

	delay(2000);
}

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();
	wdt_reset();
	Serial.print(".");
	loop_onewire(gw);
	loop_si7021(gw);

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
		loop_serial();
	}
}

