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
#include <digital_input.hpp>

unsigned long SLEEP_TIME = 60000; // Sleep time between reads (in milliseconds)

#define HUMIDITY_SENSOR_DIGITAL_PIN A1

MySensor gw;


/**
 * My Sensors
 */
#define NUMBER_OF_RELAYS 3 // Total number of attached relays
const uint8_t relay_pins[NUMBER_OF_RELAYS] PROGMEM = {7, A2, A1};
const uint8_t relay_ids [NUMBER_OF_RELAYS] PROGMEM = {16, 17, 18};

#define NUMBER_OF_MOTION_SENSORS 3 // Total number of attached motion sensors
const uint8_t motion_pins[NUMBER_OF_MOTION_SENSORS] PROGMEM = {8, 3, 6};
const uint8_t motion_ids [NUMBER_OF_MOTION_SENSORS] PROGMEM = {64, 65, 66};
#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

void incomingMessage(const MyMessage &message);

void setup()
{
	Serial.begin(BAUD_RATE);
	setup_serial();
	Serial.println("launched");


	while(true)
	{
		Serial.println("UP A1");
		digitalWrite(HUMIDITY_SENSOR_DIGITAL_PIN, HIGH);
		delay(1000);
		Serial.println("DOWN A1");
		digitalWrite(HUMIDITY_SENSOR_DIGITAL_PIN, LOW);
		delay(1000);
		Serial.println("UP 8");
		digitalWrite(8, HIGH);
		delay(1000);
		Serial.println("DOWN 8");
		digitalWrite(8, LOW);
		delay(1000);
	}
	return;


	//IO PIN 5
	eeprom_reset_check(A3);


	setup_relay(gw, relay_pins, relay_ids, NUMBER_OF_RELAYS, RELAY_ON, RELAY_OFF, false);
	setup_digital_input(gw, motion_pins, motion_ids, NUMBER_OF_MOTION_SENSORS, false);
	analogWrite(A1, 0);
	pinMode(A1, OUTPUT);
	setup_si7021(gw, 3, false, false);
	setup_onewire(gw, false);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, 0xFE, true);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");


	present_si7021(gw);
	present_relays(gw);
	present_onewire(gw);
	present_digital_inputs(gw);

	loop_si7021(gw);
	loop_onewire(gw);

}

uint8_t last_tripped_state;
unsigned long last_sensor_updates;

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();
	loop_relays(gw);
	loop_digital_inputs(gw);

	if(millis() - last_sensor_updates > SLEEP_TIME)
	{
		loop_si7021(gw);
		loop_onewire(gw);
		last_sensor_updates = millis();
	}

	loop_serial();
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
