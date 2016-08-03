#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

#include <eeprom_reset.hpp>
#include <dht_node.hpp>
#include <si7021_node.hpp>
#include <serial.hpp>
#include <digital_input.hpp>
#include "../../Common/digital_output.hpp"

unsigned long SLEEP_TIME = 60000; // Sleep time between reads (in milliseconds)

#define HUMIDITY_SENSOR_DIGITAL_PIN A1

MySensor gw;


/**
 * My Sensors
 */
#define NUMBER_OF_RELAYS 4 // Total number of attached relays
const uint8_t relay_pins[NUMBER_OF_RELAYS] PROGMEM = {7, 5, A3, A2};
const uint8_t relay_ids [NUMBER_OF_RELAYS] PROGMEM = {16, 17, 18, 19};

#define NUMBER_OF_DIGITAL_SENSORS 4 // Total number of attached motion sensors
const uint8_t input_pins[NUMBER_OF_DIGITAL_SENSORS] PROGMEM = {3, 6, 8, A1};
const uint8_t input_ids [NUMBER_OF_DIGITAL_SENSORS] PROGMEM = {64, 65, 66, 67};
const mysensor_sensor input_types [NUMBER_OF_DIGITAL_SENSORS] PROGMEM = {S_DOOR, S_MOTION, S_MOTION, S_MOTION};
#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

void incomingMessage(const MyMessage &message);

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup()
{
	Serial.begin(BAUD_RATE);
	setup_serial(false);
	wdt_disable();
	//IO PIN 5
	eeprom_reset_check(4);
	wdt_reset();

	setup_relay(gw, relay_pins, relay_ids, NUMBER_OF_RELAYS, RELAY_ON, RELAY_OFF, false);
	setup_digital_input(gw, input_pins, input_ids, NUMBER_OF_DIGITAL_SENSORS, false, true, input_types);
	setup_si7021(gw, 3, false, false);
//	setup_onewire(gw, false);

	wdt_reset();
	wdt_disable();
	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, 0x8, false);
	wdt_reset();

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");
	wdt_reset();

	present_si7021(gw);
//	present_relays(gw);
//	present_onewire(gw);
//	present_digital_inputs(gw);

	wdt_reset();
	wdt_enable(WDTO_8S);
//	loop_si7021(gw);
//	loop_onewire(gw);

}

uint8_t last_tripped_state;
unsigned long last_sensor_updates;

void loop()
{
	wdt_reset();
	// Process incoming messages (like config from server)
	gw.process();
	loop_relays(gw);
	loop_digital_inputs(gw);

	if(millis() - last_sensor_updates > SLEEP_TIME)
	{
		wdt_reset();
		loop_si7021(gw);
//		loop_onewire(gw);
		last_sensor_updates = millis();
	}

	loop_serial();
}


void incomingMessage(const MyMessage &message)
{
	wdt_reset();
	// We only expect one type of message from controller. But we better check anyway.
	if (incoming_message_relay(gw, message))
	{
		wdt_reset();
		Serial.println(F("Incomming msg was for relay"));
	}
	else
	{
		Serial.print(F("unknown message type : "));
		Serial.print(message.type);
		Serial.print(F(" -- "));
		Serial.print(message.sensor);
		Serial.print(F(", New status: "));
		Serial.println(message.getBool());
	}
}
