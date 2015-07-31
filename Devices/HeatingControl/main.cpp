// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#include <printf.hpp>

#define RELAY_1_ID   16  // MySensors node ID of relay 1
#define RELAY_1_PIN  22  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define NUMBER_OF_RELAYS 16 // Total number of attached relays
#define RELAY_ON 0  // GPIO value to write to turn on attached relay
#define RELAY_OFF 1 // GPIO value to write to turn off attached relay

MySensor gw(/*_cepin=*/14, /*_cspin=*/15);
boolean receivedConfig = false;
// Initialize temperature message

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const char Str_BuiltAt[] PROGMEM = "built at:";
const char Str_Time[] PROGMEM = __TIME__;


void incomingMessage(const MyMessage &message);

void setup()
{
	Serial.begin(115200);
	Serial.println("Launched");

	lcd.begin(16, 2);
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("built at : ");
	lcd.setCursor(0, 1);
	lcd.print(__TIME__);
	delay(1000);


//	for (int i = 0; i < 512; i++)
//	{
//		EEPROM.write(i, 0xff);
//	}
//	Serial.println("cleared");
//	lcd.clear();
//	lcd.setCursor(0, 0);
//	lcd.print("ROM cleared");
//	delay(1000);
	// Initialize library and add callback for incoming messages

	gw.begin(incomingMessage, AUTO, true, AUTO, RF24_PA_MAX);
	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	Serial.println("Ready");
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("ready");
	lcd.setCursor(0, 1);
	lcd.print("node id:");
	lcd.print(gw.getNodeId());

	// Fetch relay status
	for (int i=0; i < NUMBER_OF_RELAYS; i++)
	{
		int sensor = RELAY_1_ID + i;
		int pin = RELAY_1_PIN + i;
		Serial.print("Presenting relay ");
		Serial.println(sensor);
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
	// Alway process incoming messages whenever possible
	gw.process();
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
		Serial.print("Incoming change for Relay:");
		Serial.print(message.sensor);
		Serial.print(", New status: ");
		Serial.println(message.getBool());
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Relay changed");
		lcd.setCursor(0, 1);
		lcd.print(message.sensor);
		lcd.print(" ");
		lcd.print(message.getBool());
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
