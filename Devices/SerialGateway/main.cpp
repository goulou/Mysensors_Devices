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


#define NO_PORTB_PINCHANGES

#include <MySigningNone.h>
#include <MyTransportRFM69.h>
#include <MyTransportNRF24.h>
#include <MyHwATMega328.h>
#include <MySigningAtsha204Soft.h>
#include <MySigningAtsha204.h>

#include <SPI.h>
#include <MyParserSerial.h>
#include <MySensor.h>
#include <stdarg.h>
#include <PinChangeInt.h>
#include "GatewayUtil.h"


#include <stdarg.h>
#include <serial.hpp>


#define INCLUSION_MODE_TIME 1 // Number of minutes inclusion mode is enabled
#define INCLUSION_MODE_PIN 3 // Digital pin used for inclusion mode button
// NRFRF24L01 radio driver (set low transmit power by default)
MyTransportNRF24 transport(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_MAX);
//MyTransportRFM69 transport;

// Message signing driver (signer needed if MY_SIGNING_FEATURE is turned on in MyConfig.h)
//MySigningNone signer;
//MySigningAtsha204Soft signer;
//MySigningAtsha204 signer;

// Hardware profile
MyHwATMega328 hw;

// Construct MySensors library (signer needed if MY_SIGNING_FEATURE is turned on in MyConfig.h)
// To use LEDs blinking, uncomment WITH_LEDS_BLINKING in MyConfig.h
#ifdef WITH_LEDS_BLINKING
MySensor gw(transport, hw /*, signer*/, RADIO_RX_LED_PIN, RADIO_TX_LED_PIN, RADIO_ERROR_LED_PIN);
#else
MySensor gw(transport, hw /*, signer*/);
#endif



char inputString[MAX_RECEIVE_LENGTH] = "";    // A string to hold incoming commands from serial/ethernet interface
int inputPos = 0;
boolean commandComplete = false;  // whether the string is complete

void parseAndSend(char *commandBuffer);

void output(const char *fmt, ... ) {
   va_list args;
   va_start (args, fmt );
   vsnprintf_P(serialBuffer, MAX_SEND_LENGTH, fmt, args);
   va_end (args);
   Serial.print(serialBuffer);
}


void setup()
{
	setupGateway(INCLUSION_MODE_PIN, INCLUSION_MODE_TIME, output);
  gw.begin(incomingMessage, 0, true, 0);

  // Send startup log message on serial
  serial(PSTR("0;0;%d;0;%d;Gateway startup complete.\n"),  C_INTERNAL, I_GATEWAY_READY);
}

void loop()
{
  gw.process();

  if (commandComplete) {
    // A command wass issued from serial interface
    // We will now try to send it to the actuator
    parseAndSend(gw, inputString);
    commandComplete = false;
    inputPos = 0;
  }
}


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inputPos<MAX_RECEIVE_LENGTH-1 && !commandComplete) {
      if (inChar == '\n') {
        inputString[inputPos] = 0;
        commandComplete = true;
      } else {
        // add it to the inputString:
        inputString[inputPos] = inChar;
        inputPos++;
      }
    } else {
       // Incoming message too long. Throw away
        inputPos = 0;
    }
  }
}



