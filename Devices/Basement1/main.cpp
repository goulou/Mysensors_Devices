// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)



/*
 * DHT (Temperature + Humidity) sensor
 */
#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
#define HUMIDITY_SENSOR_DIGITAL_PIN 4

DHT dht;
float lastTemp;
float lastHum;
boolean metric = true;
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

/**
 * My Sensors
 */
MySensor gw;
boolean receivedConfig = false;
void incomingMessage(const MyMessage &message);

/**
 * One Wire
 */
#define ONE_WIRE_BUS 3 // Pin where dallase sensor is connected
#define MAX_ATTACHED_DS18B20 16

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float lastTemperature[MAX_ATTACHED_DS18B20];
int numDallasSensors = 0;
// Initialize temperature message
MyMessage msg(0, V_TEMP);


#define RELAY_1_ID   16  // MySensors node ID of relay 1
#define RELAY_1_PIN  5  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define NUMBER_OF_RELAYS 1 // Total number of attached relays
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay


#ifdef DEBUG
#define DEBUG_PRINT_ln 	Serial.println
#define DEBUG_PRINT 	Serial.print
#else
#define DEBUG_PRINT_ln(...)
#endif

#include "printf.h"
void setup()
{
	printf_begin();

	// Startup OneWire
	sensors.begin();
	dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, AUTO, true);
	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo("Basement1", "1.0");

	// Fetch the number of attached temperature sensors
	numDallasSensors = sensors.getDeviceCount();

	// Present all sensors to controller
	int i = 0;
	for (i = 0; i < numDallasSensors && i < MAX_ATTACHED_DS18B20; i++)
	{
		gw.present(i, S_TEMP);
	}

	DEBUG_PRINT("registered ");
	DEBUG_PRINT(i);
	DEBUG_PRINT_ln(" onewire temperature device(s)");

	// Register all sensors to gw (they will be created as child devices)

	while (isnan(dht.getTemperature()))
	{
		DEBUG_PRINT_ln("Temperature is NaN");
		DEBUG_PRINT_ln(dht.getStatusString());
		digitalWrite(13, HIGH);
		delay(200);
		digitalWrite(13, LOW);
		gw.sleep(1000);
		dht.readSensor();
	}

	DEBUG_PRINT_ln("Registering relay");
	// Fetch relay status
	for (int i=0; i < NUMBER_OF_RELAYS; i++)
	{
		int sensor = RELAY_1_ID + i;
		int pin = RELAY_1_PIN + i;
		Serial.print("Presenting relay ");
		Serial.print(sensor);
		Serial.print(" at pin ");
		Serial.println(pin);
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


	DEBUG_PRINT_ln("Registering DHT");
	msgHum.setSensor(128);
	gw.present(128, S_HUM);
	msgTemp.setSensor(129);
	gw.present(129, S_TEMP);
}

unsigned int failed_count = 0;

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();

	// Fetch temperatures from Dallas sensors
	sensors.requestTemperatures();

	// Read temperatures and send them to controller
	for (int i = 0; i < numDallasSensors && i < MAX_ATTACHED_DS18B20; i++)
	{

		// Fetch and round temperature to one decimal
		float temperature = static_cast<float>(static_cast<int>((
				gw.getConfig().isMetric ? sensors.getTempCByIndex(i) : sensors.getTempFByIndex(i)) * 10.)) / 10.;

		// Only send data if temperature has changed and no error
		if (lastTemperature[i] != temperature && temperature != -127.00)
		{
			Serial.print("sending temperature for device ");
			Serial.println(i);
			// Send in the new temperature
			gw.send(msg.setSensor(i).set(temperature, 1));
			lastTemperature[i] = temperature;
		}
	}

	dht.readSensor();
	float temperature = dht.getTemperature();
	float humidity = dht.getHumidity();

	unsigned long sleep_time;

	if(isnan(temperature) || isnan(humidity))
	{
		failed_count++;
		DEBUG_PRINT_ln("unable to read");
		DEBUG_PRINT_ln(dht.getStatusString());
		if(failed_count >=20)
		{
			while(true)
			{
				digitalWrite(13, LOW);
				gw.sleep(500);
				digitalWrite(13, HIGH);
				gw.sleep(500);
				DEBUG_PRINT_ln("unable to read");
			}
		}

		sleep_time = SLEEP_TIME/10;
	}
	else
	{
		failed_count = 0;

		if (temperature != lastTemp)
		{
			lastTemp = temperature;
			if (!metric)
			{
				temperature = dht.toFahrenheit(temperature);
			}
			gw.send(msgTemp.set(temperature, 1));
	//		Serial.print("T: ");
	//		DEBUGln(temperature);
		}
		else
		{
			DEBUG_PRINT_ln("temperature did not change");
		}

		if (humidity != lastHum)
		{
			lastHum = humidity;
			gw.send(msgHum.set(humidity, 1));
	//		Serial.print("H: ");
	//		DEBUGln(humidity);
		}
		else
		{
			DEBUG_PRINT_ln("humidity did not change");
		}


		sleep_time = SLEEP_TIME;
	}

	//Receiver+Repeater mode needs not sleeping : process messages instead.
	unsigned long start_millis = millis();
	while( ((unsigned long)(millis() - start_millis)) < sleep_time)
	{
		gw.process();
	}
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
		Serial.print("Incoming change for sensor:");
		Serial.print(message.sensor);
		Serial.print(", New status: ");
		Serial.println(message.getBool());
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
