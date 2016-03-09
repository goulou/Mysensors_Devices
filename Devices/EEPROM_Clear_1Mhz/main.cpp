// Example sketch showing how to send in OneWire temperature readings
#include <Arduino.h>
#include <EEPROM.h>

#include <serial.hpp>

#define LED_PIN 13

void setup()
{
	Serial.begin(9600);
	setup_serial();

	Serial.println("Clearing EEPROM");
	for (int i=0;i<512;i++)
	{
		EEPROM.write(i, 0xff);
	}
	pinMode(LED_PIN, OUTPUT);
}


void loop()
{
	while(true)
	{
		Serial.println("EEPROM cleared : waiting in endless loop");
		digitalWrite(LED_PIN, HIGH);
		delay(1000);
		digitalWrite(LED_PIN, LOW);
		delay(1000);
	}
}



