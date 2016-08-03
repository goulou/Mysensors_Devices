// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <NewPing.h>

#include <eeprom_reset.hpp>
#include <1w_node.hpp>
#include <dht_node.hpp>
#include <battery_monitored_node.hpp>
#include <si7021_node.hpp>
#include <digital_input.hpp>
#include <serial.hpp>

MySensor gw;
unsigned long SLEEP_TIME = 60000; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0

#define IO_5V_SWITCH	A2
#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     A1  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

#define NUMBER_OF_INPUT_SENSORS 3 // Total number of attached motion sensors
const uint8_t input_pins[NUMBER_OF_INPUT_SENSORS] PROGMEM = {8, 3, 6};
const uint8_t input_ids [NUMBER_OF_INPUT_SENSORS] PROGMEM = {64, 65, 66};
const mysensor_sensor input_types [NUMBER_OF_INPUT_SENSORS] PROGMEM = {S_BINARY, S_BINARY, S_BINARY};
#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

MyMessage msg(0, V_DISTANCE);
unsigned int get_distance_cm();
unsigned int get_distance_cm_long_median();

void setup()
{
	Serial.begin(BAUD_RATE);
	setup_serial();
	Serial.println();
	Serial.println("launched");
	wdt_reset();

	eeprom_reset_check(4);

	wdt_reset();

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(NULL, 0x6);

	// Send the sketch version information to the gateway and Controller
	Serial.println("sendSketchInfo");
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");
	wdt_reset();

	battery_monitored_node_setup(gw, BATTERY_SENSE_PIN);

	setup_digital_input(gw, input_pins, input_ids, NUMBER_OF_INPUT_SENSORS, false, true, input_types);
	present_digital_inputs(gw);
	wdt_reset();

}

void loop()
{
	// Process incoming messages (like config from server)
//	gw.process();
//	loop_si7021(gw);
//	battery_monitored_node_loop(gw);
//	if(count > 120)
	{
//		loop_si7021(gw);
		Serial.print("2");
		battery_monitored_node_loop(gw);
//		count = 0;
	}
//	count ++;
	wdt_reset();
	Serial.println("Sl");
	wdt_disable();
	while(gw.sleep(0, CHANGE, 1, CHANGE, SLEEP_TIME) == true)
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
		wdt_disable();
	}
	Serial.println("Up");
	wdt_reset();
}

