// Example sketch showing how to send in OneWire temperature readings
// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <eeprom_reset.hpp>
#include <LiquidCrystal.h>

#include <dht_node.hpp>
#include <1w_node.hpp>
#include <printf.hpp>
#include <relay_node.hpp>

MyTransportNRF24 transport(/*_cepin=*/14, /*_cspin=*/15, RF24_PA_MAX);
MyHwDriver hw;

MySensor gw(transport, hw);
boolean receivedConfig = false;
// Initialize temperature message
// Initialize temperature message

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const char Str_BuiltAt[] PROGMEM = "built at:";
const char Str_Time[] PROGMEM = __TIME__;



unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)



#define HUMIDITY_SENSOR_DIGITAL_PIN 49




#define NUMBER_OF_RELAYS 16 // Total number of attached relays
const uint8_t relay_pins[NUMBER_OF_RELAYS] PROGMEM = {22, 24, 26, 28, 23, 25, 27, 29, 30, 32, 34, 36, 31, 33, 35, 37};
const uint8_t relay_ids [NUMBER_OF_RELAYS] PROGMEM = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
#define RELAY_ON LOW  // GPIO value to write to turn on attached relay
#define RELAY_OFF HIGH // GPIO value to write to turn off attached relay

void incomingMessage(const MyMessage &message);

void setup()
{
	Serial.begin(BAUD_RATE);
	Serial.println("launched");

	printf_begin();

	//IO PIN 5
	eeprom_reset_check(3);


	setup_relay(gw, relay_pins, relay_ids, NUMBER_OF_RELAYS, RELAY_ON, RELAY_OFF, false);
	setup_dht(gw, HUMIDITY_SENSOR_DIGITAL_PIN, 5, false);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, 69, true);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	onewire_node_setup(gw);

	present_dht(gw);

	DEBUG_PRINT_ln("Registering relays");
	present_relays(gw);
}

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();
	onewire_node_loop(gw);
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
