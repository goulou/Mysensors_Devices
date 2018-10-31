
#ifndef BAUD_RATE
#define BAUD_RATE 9600
#endif
#define MY_BAUD_RATE BAUD_RATE

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

#include <default_mapping.hpp>

#include <eeprom_reset.hpp>
#include <battery_monitored_node.hpp>
#include <si7021_node.hpp>

#define BATTERY_SENSE_PIN  A0

#include <digital_input.hpp>
#include <digital_output.hpp>

#ifdef ENABLE_BMP_SENSOR
#include <bmp_node.hpp>
#endif

/**
 * My Sensors
 */
// Sleep time between reads (in milliseconds)
#ifdef ENABLE_BMP_SENSOR
	#ifdef SLEEP_TIME
		#undef SLEEP_TIME
	#endif
	#define SLEEP_TIME ((unsigned long)(60000*4))
#else
	#ifndef SLEEP_TIME
		#define SLEEP_TIME ((unsigned long)(60000*5))
	#endif
#endif

void before()
{

	Serial.begin(BAUD_RATE);
	setup_serial();
	Serial.println();
	Serial.println("launched");
	wdt_reset();

	eeprom_reset_check(EEPROM_RESET_PIN);
	setup_si7021(5, false, true);

#ifdef ENABLE_BMP_SENSOR
	setup_bmp();
#endif
//	setup_digital_output(output_pins, output_ids, NUMBER_OF_RELAYS, RELAY_ON, RELAY_OFF, false);
//	setup_digital_input(input_pins, input_ids, NUMBER_OF_DIGITAL_SENSORS, false, false, input_types);
	wdt_reset();

	wdt_enable(WDTO_8S);
}


void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

	// Register binary input sensor to sensor_node (they will be created as child devices)
	// You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
	// If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
	battery_monitored_node_setup(BATTERY_SENSE_PIN);
	present_si7021();
#ifdef ENABLE_BMP_SENSOR
	present_bmp();
#endif
}

uint8_t no_send_count = 0;

void loop()
{
	wdt_reset();
	no_send_count = (no_send_count+1) % 6;
	bool force_send = (no_send_count==0);

#ifdef ENABLE_BMP_SENSOR
	loop_bmp(force_send, SLEEP_TIME/60000);
#endif
	loop_si7021(force_send);
	battery_monitored_node_loop();

	wdt_reset();
	wdt_disable();
	sleep(SLEEP_TIME);
	wdt_enable(WDTO_8S);
	wdt_reset();

}

