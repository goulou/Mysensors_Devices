// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>

#include <eeprom_reset.hpp>
#include <1w_node.hpp>
#include <dht_node.hpp>
#include <battery_monitored_node.hpp>
#include <si7021_node.hpp>
#include <digital_input.hpp>
#include <serial.hpp>
#include "../../Common/digital_output.hpp"

MySensor gw;
unsigned long SLEEP_TIME = 60000; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0

#define NUMBER_OF_DIGITAL_SENSORS 3 // Total number of attached motion sensors
const uint8_t input_pins[NUMBER_OF_DIGITAL_SENSORS] PROGMEM = {3, 8, A1};
const uint8_t input_ids [NUMBER_OF_DIGITAL_SENSORS] PROGMEM = {64, 65, 66};
const mysensor_sensor input_types [NUMBER_OF_DIGITAL_SENSORS] PROGMEM = {S_BINARY, S_BINARY, S_BINARY};

/**
 * My Sensors
 */
#define NUMBER_OF_RELAYS 2 // Total number of attached relays
const uint8_t relay_pins[NUMBER_OF_RELAYS] PROGMEM = {7, A3};
const uint8_t relay_ids [NUMBER_OF_RELAYS] PROGMEM = {16, 17};
#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay


void incomingMessage(const MyMessage &message);

void setup()
{
	Serial.begin(BAUD_RATE);
	setup_serial();
	Serial.println("launched");

	//IO PIN 5
	eeprom_reset_check(4);
	wdt_reset();

	setup_relay(gw, relay_pins, relay_ids, NUMBER_OF_RELAYS, RELAY_ON, RELAY_OFF, false);
	setup_digital_input(gw, input_pins, input_ids, NUMBER_OF_DIGITAL_SENSORS, false, true, input_types);
	setup_si7021(gw, 3, false, false);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, 0x9, false, hw_readConfig(EEPROM_PARENT_NODE_ID_ADDRESS));
	wdt_reset();

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");
	wdt_reset();

	present_si7021(gw);
	present_relays(gw);
	present_digital_inputs(gw);

	wdt_reset();
	loop_si7021(gw);
//	loop_onewire(gw);


}

uint32_t count = 0;
void loop()
{
	// Process incoming messages (like config from server)
//	gw.process();
//	loop_si7021(gw);
//	battery_monitored_node_loop(gw);
//	if(count > 120)
	{
		loop_si7021(gw);
		Serial.print("2");
		battery_monitored_node_loop(gw);
//		count = 0;
	}
//	count ++;
	loop_digital_inputs(gw);
	wdt_reset();
	Serial.println("Sl");
	wdt_disable();
	while(gw.sleep(1, digitalRead(input_pins[0])==LOW?RISING:FALLING, SLEEP_TIME) == true)
	{
		wdt_reset();
		wdt_enable(WDTO_8S);
		wdt_reset();
//		Serial.begin(BAUD_RATE);
		Serial.println("IT");
		while(loop_digital_inputs(gw))
		{
			Serial.print("r");
		}
		Serial.println("Sl");
		wdt_reset();
		loop_si7021(gw);
		wdt_reset();
		wdt_disable();
	}
	Serial.println("Up");
	wdt_reset();
}



void incomingMessage(const MyMessage &message)
{
	wdt_reset();
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
