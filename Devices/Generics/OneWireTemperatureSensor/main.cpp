
#ifndef BAUD_RATE
#define BAUD_RATE 9600
#endif
#define MY_BAUD_RATE BAUD_RATE

#define MY_REPEATER_FEATURE
// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <SPI.h>
#include <MySensors.h>

#define SKETCH_NAME xstr(PROGRAM_NAME)
#define SKETCH_MAJOR_VER "2"
#define SKETCH_MINOR_VER "0"

#define DEBUG
#include <serial.hpp>

#include <eeprom_reset.hpp>
#include <battery_monitored_node.hpp>
#include <1w_node.hpp>
unsigned long SLEEP_TIME = 60000*5; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0



void setup()
{
	Serial.begin(BAUD_RATE);
	setup_serial();
	Serial.println();
	Serial.println("launched");
	wdt_reset();

	eeprom_reset_check(4);
	setup_onewire();
	wdt_reset();
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
	battery_monitored_node_setup(BATTERY_SENSE_PIN);
	present_onewire();
}

void loop()
{
	// Process incoming messages (like config from server)
	loop_onewire();
	battery_monitored_node_loop();

	wdt_disable();
	sleep(SLEEP_TIME);
	wdt_enable(WDTO_8S);
	wdt_reset();
}

