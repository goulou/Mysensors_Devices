// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <DHT.h>

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
#define HUMIDITY_SENSOR_DIGITAL_PIN 3

//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);

DHT dht;
float lastTemp;
float lastHum;
boolean metric = true;
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

MySensor gw;
boolean receivedConfig = false;
// Initialize temperature message
MyMessage msg(0, V_TEMP);

#ifdef DEBUG
#define DEBUGln Serial.println
#else
#define DEBUGln(...)
#endif

#include "printf.h"
void setup()
{
	Serial.begin(115200);
	DEBUGln("launched");
	printf_begin();
	// Startup OneWire
//	sensors.begin();
	dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin();
	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	gw.sendSketchInfo("BathRoom1", "1.0");

	DEBUGln("ready");

//	// Fetch the number of attached temperature sensors
//	numSensors = sensors.getDeviceCount();
//
//	// Present all sensors to controller
//	int i = 0;
//	for (i=0; i<numSensors && i<MAX_ATTACHED_DS18B20; i++)
//	{
//		gw.present(i, S_TEMP);
//	}
//
//	Serial.print("registered ");
//	Serial.print(i);
//	DEBUGln(" onewire temperature device(s)");

	delay(2000);
	dht.readSensor();
	// Register all sensors to gw (they will be created as child devices)
	if (isnan(dht.getTemperature()) == false)
	{
		msgHum.setSensor(128);
		gw.present(128, S_HUM);
		msgTemp.setSensor(129);
		gw.present(129, S_TEMP);
	}
	else
	{
		while(isnan(dht.getTemperature()))
		{
			DEBUGln("Temperature is NaN");
			DEBUGln(dht.getStatusString());
			digitalWrite(13, HIGH);
			delay(200);
			digitalWrite(13, LOW);
			gw.sleep(1000);
			dht.readSensor();
		}
	}
	delay(1000);
}
int failed_count = 0;

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();
	/*
	 // Fetch temperatures from Dallas sensors
	 sensors.requestTemperatures();

	 // Read temperatures and send them to controller
	 for (int i=0; i<numSensors && i<MAX_ATTACHED_DS18B20; i++) {

	 // Fetch and round temperature to one decimal
	 float temperature = static_cast<float>(static_cast<int>((gw.getConfig().isMetric?sensors.getTempCByIndex(i):sensors.getTempFByIndex(i)) * 10.)) / 10.;

	 // Only send data if temperature has changed and no error
	 if (lastTemperature[i] != temperature && temperature != -127.00) {
	 Serial.print("sending temperature for device ");
	 DEBUGln(i);
	 // Send in the new temperature
	 gw.send(msg.setSensor(i).set(temperature,1));
	 lastTemperature[i]=temperature;
	 }
	 }
	 */
	dht.readSensor();
	float temperature = dht.getTemperature();
	float humidity = dht.getHumidity();

	if(isnan(temperature) || isnan(humidity))
	{
		failed_count++;
		DEBUGln("unable to read");
		DEBUGln(dht.getStatusString());
		if(failed_count >=20)
		{
			while(true)
			{
				digitalWrite(13, LOW);
				gw.sleep(500);
				digitalWrite(13, HIGH);
				gw.sleep(500);
				DEBUGln("unable to read");
			}
		}

		gw.sleep(SLEEP_TIME/5);
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
			DEBUGln("temperature did not change");
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
			DEBUGln("humidity did not change");
		}


		gw.sleep(SLEEP_TIME);
	}


}

