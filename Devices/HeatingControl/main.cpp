// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <EmonLib.h>

#include <printf.hpp>

#define NUMBER_OF_RELAYS 16 // Total number of attached relays
const uint8_t relay_pins[NUMBER_OF_RELAYS] PROGMEM = {22, 24, 26, 28, 23, 25, 27, 29, 34, 35, 36, 37, 38, 39, 40, 41};
const uint8_t relay_ids [NUMBER_OF_RELAYS] PROGMEM = {16, 17, 18, 19, 20, 21, 22, 23, 32, 33, 34, 35, 36, 37, 38, 39};


#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay

MyTransportNRF24 transport(/*_cepin=*/14, /*_cspin=*/15, RF24_PA_MAX);
MyHwDriver hw;

MySensor gw(transport, hw);
boolean receivedConfig = false;
// Initialize temperature message

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const char Str_BuiltAt[] PROGMEM = "built at:";
const char Str_Time[] PROGMEM = __TIME__;


void incomingMessage(const MyMessage &message);
void present_relays();

uint8_t get_relay_pin_for_id(uint8_t sensor_id)
{
	for(int i=0; i<NUMBER_OF_RELAYS; i++)
	{
		if(pgm_read_byte(&relay_ids[i]) == sensor_id)
		{
			return pgm_read_byte(&relay_pins[i]);
		}
	}
	return 255;
}


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
	Serial.println("Initializing Radio");
	gw.begin(incomingMessage, 0x13, true, AUTO);
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
		int sensor = pgm_read_byte(&relay_ids[i]);
		int pin = get_relay_pin_for_id(sensor);
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

	gw.present(8, S_POWER);
}

void present_relays()
{
	for (int i=0; i < NUMBER_OF_RELAYS; i++)
	{
		int sensor = pgm_read_byte(&relay_ids[i]);
		int pin = get_relay_pin_for_id(sensor);
		Serial.print("Presenting relay ");
		Serial.println(sensor);
		// Register all sensors to gw (they will be created as child devices)
		gw.present(sensor, V_LIGHT);
		bool value = gw.loadState(sensor);
		MyMessage msg(sensor, V_TRIPPED);
		gw.send(msg.set(value == HIGH ? 1 : 0));
	}
}

void send_current_measurments()
{
	EnergyMonitor emon1;
	emon1.current(A0, 60);
	double Irms = emon1.calcIrms(1480);
	MyMessage msg(8, V_WATT);
	msg.set(Irms*230, 4);
	gw.send(msg);
}

void loop()
{
	unsigned long next_update = millis() + 24L*60L*60L*1000L;
	while((long)next_update - (long)millis() > 0)
	{
		unsigned long next_reading = millis() + 30*1000;
		while(millis() < next_reading)
		{
			// Alway process incoming messages whenever possible
			gw.process();
		}
		send_current_measurments();
	}
//	present_relays();
}

void incomingMessage(const MyMessage &message)
{
	// We only expect one type of message from controller. But we better check anyway.
	if (message.type == V_LIGHT)
	{
		int pin = get_relay_pin_for_id(message.sensor);
		// Change relay state
		digitalWrite(pin, message.getBool() ? RELAY_ON : RELAY_OFF);
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
