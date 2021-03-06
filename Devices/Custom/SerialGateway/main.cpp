  /*
 * Copyright (C) 2013 Henrik Ekblad <henrik.ekblad@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * DESCRIPTION
 * The ArduinoGateway prints data received from sensors on the serial link.
 * The gateway accepts input on seral which will be sent out on radio network.
 *
 * The GW code is designed for Arduino Nano 328p / 16MHz
 *
 * Wire connections (OPTIONAL):
 * - Inclusion button should be connected between digital pin 3 and GND
 * - RX/TX/ERR leds need to be connected between +5V (anode) and digital ping 6/5/4 with resistor 270-330R in a series
 *
 * LEDs (OPTIONAL):
 * - RX (green) - blink fast on radio message recieved. In inclusion mode will blink fast only on presentation recieved
 * - TX (yellow) - blink fast on radio message transmitted. In inclusion mode will blink slowly
 * - ERR (red) - fast blink on error during transmission error or recieve crc error
 */


#ifndef BAUD_RATE
#define BAUD_RATE 9600
#endif
#define MY_BAUD_RATE BAUD_RATE




// Enable debug prints to serial monitor
#define MY_DEBUG


// Enable and select radio type attached
#define MY_RADIO_NRF24

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
#define MY_RF24_PA_LEVEL RF24_PA_LOW

// Enable serial gateway
#define MY_GATEWAY_SERIAL

// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif

// Inverses the behavior of leds
//#define MY_WITH_LEDS_BLINKING_INVERSE

#include <SPI.h>
#include <MySensors.h>
#include <serial.hpp>

#define SKETCH_NAME xstr(PROGRAM_NAME)
#define SKETCH_MAJOR_VER "2"
#define SKETCH_MINOR_VER "0"

//#include <digital_input.hpp>
//#include <digital_output.hpp>
//#include <si7021_node.hpp>

void before()
{
//	setup_digital_output();
//	setup_digital_input();
//	setup_si7021(3, false, false);
}

void setup() {
	  // Setup locally attached sensors
}

void presentation()
{
	 // Present locally attached sensors
  // Send the sketch version information to the gateway and Controller
	sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);
//	present_digital_output();
//	present_digital_inputs();
//	present_si7021();
}


void loop()
{
//	loop_digital_inputs();
//	loop_digital_output();
//	loop_si7021();
	  // Send locally attached sensor data here
}

void receive(const MyMessage &message)
{
	// We only expect one type of message from controller. But we better check anyway.
//	incoming_message_digital_output(message);
}

