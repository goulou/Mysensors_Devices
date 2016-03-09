#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

#include <eeprom_reset.hpp>
#include <dht_node.hpp>
#include <1w_node.hpp>
#include <relay_node.hpp>

#include <si7021_node.hpp>
#include <serial.hpp>

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

#define HUMIDITY_SENSOR_DIGITAL_PIN A1

MySensor gw;


/**
 * My Sensors
 */
#define NUMBER_OF_RELAYS 1 // Total number of attached relays
const uint8_t relay_pins[NUMBER_OF_RELAYS] PROGMEM = {6};
const uint8_t relay_ids [NUMBER_OF_RELAYS] PROGMEM = {16};
#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

void incomingMessage(const MyMessage &message);

void setup()
{
	Serial.begin(BAUD_RATE);
	setup_serial();
	Serial.println("launched");

	//IO PIN 5
	eeprom_reset_check(A3);


	setup_relay(gw, relay_pins, relay_ids, NUMBER_OF_RELAYS, RELAY_ON, RELAY_OFF, false);
	analogWrite(A1, 0);
	pinMode(A1, OUTPUT);
	setup_dht(gw, 8, 5, false);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, 0x1B, true);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");


	present_dht(gw);

	DEBUG_PRINT_ln("Registering relays");
	present_relays(gw);

}

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();
	loop_relays(gw);
	loop_dht(gw);

	//Repeater mode needs not sleeping : process messages instead.
	unsigned long start_millis = millis();
	while( ((unsigned long)(millis() - start_millis)) < SLEEP_TIME)
	{
		gw.process();
	}
}


void incomingMessage(const MyMessage &message)
{
	// We only expect one type of message from controller. But we better check anyway.
	if (incoming_message_relay(gw, message))
	{
		Serial.println("Incomming msg was for relay");
	}
	else
	{
		Serial.print("unknown message type : ");
		Serial.print(message.type);
		Serial.print(" -- ");
		Serial.print(message.sensor);
		Serial.print(", New status: ");
		Serial.println(message.getBool());
	}
}
