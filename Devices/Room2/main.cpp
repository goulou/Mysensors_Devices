// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 3 // Pin where dallase sensor is connected
#define MAX_ATTACHED_DS18B20 16
unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

MySensor gw;
float lastTemperature[MAX_ATTACHED_DS18B20];
int numDallasSensors = 0;
boolean receivedConfig = false;
// Initialize temperature message
MyMessage msg(0, V_TEMP);

#include "printf.h"
void setup()
{
	Serial.begin(115200);
	Serial.println("launched");
	printf_begin();
	// Startup OneWire
	sensors.begin();

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin();
	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo("Room2", "1.0");

	Serial.println("ready");

	// Fetch the number of attached temperature sensors
	numDallasSensors = sensors.getDeviceCount();

	// Present all sensors to controller
	int i = 0;
	for (i = 0; i < numDallasSensors && i < MAX_ATTACHED_DS18B20; i++)
	{
		gw.present(i, S_TEMP);
	}

	Serial.print("registered ");
	Serial.print(i);
	Serial.println(" onewire temperature device(s)");
}

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

	gw.sleep(SLEEP_TIME);
}

