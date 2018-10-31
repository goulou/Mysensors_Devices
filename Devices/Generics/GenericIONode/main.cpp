
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

#include <default_mapping.hpp>

#include <eeprom_reset.hpp>
#include <battery_monitored_node.hpp>
#include <si7021_node.hpp>

#define BATTERY_SENSE_PIN  A0

#include <digital_input.hpp>
#include <digital_output.hpp>


/****************************************
 *      DIGITAL INPUTS AND OUTPUTS      *
 ***************************************/
unsigned long SLEEP_TIME = 60000*5; // Sleep time between reads (in milliseconds)

void before()
{

	Serial.begin(BAUD_RATE);
	setup_serial();
	Serial.println();
	Serial.println("launched");
	wdt_reset();

	eeprom_reset_check(EEPROM_RESET_PIN);
	setup_si7021(5, false, true);
	setup_digital_output();
	setup_digital_input();

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
	present_digital_inputs();
	present_digital_output();
}


uint8_t last_tripped_state;
unsigned long last_sensor_updates;

void loop()
{
	wdt_reset();
#ifdef INPUT_INTERRUPT
	battery_monitored_node_loop();
	loop_si7021();

	wdt_reset();
	Serial.println("Sl");
	wdt_disable();
	while(sleep(
			digitalPinToInterrupt(get_digital_input_pin_for_id(0)),
			digitalRead(get_digital_input_pin_for_id(0))==LOW?RISING:FALLING,
			digitalPinToInterrupt(get_digital_input_pin_for_id(1)),
			digitalRead(get_digital_input_pin_for_id(1))==LOW?RISING:FALLING,
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
#elif defined(MY_REPEATER_FEATURE)
	// Process incoming messages (like config from server)
	loop_digital_output();
	loop_digital_inputs();

	if(millis() - last_sensor_updates > SLEEP_TIME)
	{
		wdt_reset();
		loop_si7021();
		last_sensor_updates = millis();
	}

	loop_serial();
#else
	no_send_count = (no_send_count+1) % 6;
	bool force_send = (no_send_count==0);

	loop_si7021(force_send);
	battery_monitored_node_loop();

	wdt_reset();
	wdt_disable();
	sleep(SLEEP_TIME);
	wdt_enable(WDTO_8S);
	wdt_reset();
#endif
}


void receive(const MyMessage &message)
{
	wdt_reset();
	// We only expect one type of message from controller. But we better check anyway.
	if (incoming_message_digital_output(message))
	{
		wdt_reset();
		Serial.println(F("Incomming msg was for digital outputs"));
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
	wdt_reset();
}
