// Example sketch showing how to send in OneWire temperature readings
// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <EEPROM.h>
#include <eeprom_reset.hpp>
#include <LiquidCrystal.h>

#include <dht_node.hpp>
#include <1w_node.hpp>
#include <printf.hpp>

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
const uint8_t relay_pins[NUMBER_OF_RELAYS] PROGMEM = {22, 23, 24, 25, 26, 27, 28, 29, 34, 35, 36, 37, 38, 39, 40, 41};
const uint8_t relay_ids [NUMBER_OF_RELAYS] PROGMEM = {16, 17, 18, 19, 20, 21, 22, 23, 32, 33, 34, 35, 36, 37, 38, 39};
#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

void incomingMessage(const MyMessage &message);

uint8_t get_relay_pin_for_id(uint8_t sensor_id)
{
	for(int i=0; i<NUMBER_OF_RELAYS; i++)
	{
		if(pgm_read_byte(&relay_ids[i]) == sensor_id)
		{
			return pgm_read_byte(&relay_pins[i]);
		}
	}
	return 255;
}


void setup()
{
	Serial.begin(BAUD_RATE);
	Serial.println("launched");

	printf_begin();

	//IO PIN 3
	eeprom_reset_check(5);

	for (int i=0; i < NUMBER_OF_RELAYS; i++)
	{
		int sensor_id = pgm_read_byte(&relay_ids[i]);
		int pin = get_relay_pin_for_id(sensor_id);
		Serial.print("Having relay ");
		Serial.print(sensor_id);
		Serial.print(" at pin ");
		Serial.println(pin);
	}

	setup_dht(gw, HUMIDITY_SENSOR_DIGITAL_PIN, 5, false);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, 69, true);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	onewire_node_setup(gw);

	present_dht(gw);

	DEBUG_PRINT_ln("Registering relays");
	// Fetch relay status
	for (int i=0; i < NUMBER_OF_RELAYS; i++)
	{
		int sensor = pgm_read_byte(&relay_ids[i]);
		int pin = get_relay_pin_for_id(sensor);
		Serial.print("Presenting relay ");
		Serial.print(sensor);
		Serial.print(" at pin ");
		Serial.println(pin);
		// Register all sensors to gw (they will be created as child devices)
		gw.present(sensor, V_LIGHT);
		// Then set relay pins in output mode
		pinMode(pin, OUTPUT);
		// Set relay to last known state (using eeprom storage)
		digitalWrite(pin, gw.loadState(sensor) ? RELAY_ON : RELAY_OFF);
		// Change to V_LIGHT if you use S_LIGHT in presentation below
		MyMessage msg(sensor, V_TRIPPED);
		bool value = gw.loadState(sensor);
		gw.send(msg.set(value == HIGH ? 1 : 0));
	}


}

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();
	onewire_node_loop(gw);

	unsigned long sleep_time = SLEEP_TIME;
	if(loop_dht(gw) == false)
	{
		sleep_time = SLEEP_TIME/10;
	}

	//Repeater mode needs not sleeping : process messages instead.
	unsigned long start_millis = millis();
	while( ((unsigned long)(millis() - start_millis)) < sleep_time)
	{
		gw.process();
	}
}


void incomingMessage(const MyMessage &message)
{
	// We only expect one type of message from controller. But we better check anyway.
	if (message.type == V_LIGHT)
	{
		// Change relay state
		int pin = get_relay_pin_for_id(message.sensor);
		digitalWrite(pin, message.getBool() ? RELAY_ON : RELAY_OFF);
		// Store state in eeprom
		gw.saveState(message.sensor, message.getBool());
		// Write some debug info
		Serial.print("Incoming change for relay:");
		Serial.print(message.sensor);
		Serial.print(", New status: ");
		Serial.println(message.getBool());
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
