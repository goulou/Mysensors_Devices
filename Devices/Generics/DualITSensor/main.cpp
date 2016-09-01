
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


#include <eeprom_reset.hpp>
#include <battery_monitored_node.hpp>

#include <si7021_node.hpp>

#include <default_mapping.hpp>
#include <digital_input.hpp>

#include <serial.hpp>

unsigned long SLEEP_TIME = 60000; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0

#define NUMBER_OF_DIGITAL_SENSORS 4 // Total number of attached motion sensors
const uint8_t input_pins[NUMBER_OF_DIGITAL_SENSORS] PROGMEM = {INPUT_PINS_DEFAULT};
const uint8_t input_ids [NUMBER_OF_DIGITAL_SENSORS] PROGMEM = {64, 65, 66, 67};
const mysensor_sensor input_types [NUMBER_OF_DIGITAL_SENSORS] PROGMEM = {S_BINARY, S_BINARY, S_BINARY, S_BINARY};


void setup()
{
	Serial.begin(BAUD_RATE);
	setup_serial();
	Serial.println();
	Serial.println("launched");
	wdt_reset();

	eeprom_reset_check(EEPROM_RESET_PIN);

	setup_si7021(5, false, true);
	wdt_reset();

	setup_digital_input(input_pins, input_ids, NUMBER_OF_DIGITAL_SENSORS, false, true, input_types, true);
	wdt_reset();

}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
	present_si7021();
	battery_monitored_node_setup(BATTERY_SENSE_PIN);
	present_digital_inputs();
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
//		loop_si7021(gw);
		Serial.print("2");
		battery_monitored_node_loop();
//		count = 0;
	}
//	count ++;
	loop_si7021();
	wdt_reset();
	Serial.println("Sl");
	wdt_disable();
	while(sleep(
			digitalPinToInterrupt(input_pins[0]),
			digitalRead(input_pins[0])==LOW?RISING:FALLING,
			digitalPinToInterrupt(input_pins[1]),
			digitalRead(input_pins[1])==LOW?RISING:FALLING,
					SLEEP_TIME) == true)
	{
		wdt_reset();
		wdt_enable(WDTO_8S);
		wdt_reset();
//		Serial.begin(BAUD_RATE);
		Serial.println("IT");
		while(loop_digital_inputs())
		{
			Serial.print("r");
		}
		Serial.println("Sl");
		wdt_reset();
		wdt_disable();
	}
	loop_digital_inputs();
	Serial.println("Up");
	wdt_reset();
}

