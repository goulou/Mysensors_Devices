// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>

#include <eeprom_reset.hpp>
#include <1w_node.hpp>
#include <dht_node.hpp>
#include <battery_monitored_node.hpp>
#include <si7021_node.hpp>
#include <serial.hpp>

MySensor gw;
unsigned long SLEEP_TIME = 120000; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0

#define NUMBER_OF_RELAYS 4 // Total number of attached relays
const uint8_t relay_pins[NUMBER_OF_RELAYS] PROGMEM = {A1, 8, 6, 3};
const uint8_t relay_ids [NUMBER_OF_RELAYS] PROGMEM = {16, 17, 18, 19};
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay
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
	int i=0;
	Serial.println("launched");
	setup_serial();

//	eeprom_reset_check(3);

	Serial.println("begin");
	setup_si7021(gw, 10, false);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, 2);
//	Serial.println("setPALevel");
//	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	Serial.println("sendSketchInfo");
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	present_si7021(gw);
	battery_monitored_node_setup(gw, BATTERY_SENSE_PIN);

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

	unsigned long sleep_time = SLEEP_TIME;
	if(loop_si7021(gw) == false)
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
		Serial.print(", pin: ");
		Serial.print(pin);
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
