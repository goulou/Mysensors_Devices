// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

#include <printf.hpp>
#include <dht_node.hpp>
#include <1w_node.hpp>


#define HUMIDITY_SENSOR_DIGITAL_PIN 4


/**
 * My Sensors
 */
MySensor gw;
boolean receivedConfig = false;
void incomingMessage(const MyMessage &message);



#define RELAY_1_ID   16  // MySensors node ID of relay 1
#define RELAY_1_PIN  5  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define NUMBER_OF_RELAYS 1 // Total number of attached relays
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay



void setup()
{
	printf_begin();



	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, AUTO, true);
	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	setup_dht(gw, HUMIDITY_SENSOR_DIGITAL_PIN, 0);
	onewire_node_setup(gw);

	DEBUG_PRINT_ln("Registering relay");
	// Fetch relay status
	for (int i=0; i < NUMBER_OF_RELAYS; i++)
	{
		int sensor = RELAY_1_ID + i;
		int pin = RELAY_1_PIN + i;
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
		digitalWrite((message.sensor-RELAY_1_ID) + (RELAY_1_PIN), message.getBool() ? RELAY_ON : RELAY_OFF);
		// Store state in eeprom
		gw.saveState(message.sensor, message.getBool());
		// Write some debug info
		Serial.print("Incoming change for sensor:");
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
