// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

#define ONE_WIRE_BUS 3 // Pin where dallase sensor is connected
#define MAX_ATTACHED_DS18B20 16
unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
#define HUMIDITY_SENSOR_DIGITAL_PIN 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DHT dht;
float lastTemp;
float lastHum;
boolean metric = true;
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);


MySensor gw;
float lastTemperature[MAX_ATTACHED_DS18B20];
int numSensors=0;
boolean receivedConfig = false;
// Initialize temperature message
MyMessage msg(0,V_TEMP);

#include "printf.h"
void setup()
{
printf_begin();
  // Startup OneWire
  sensors.begin();
  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN);

  // Startup and initialize MySensors library. Set callback for incoming messages.
  gw.begin();
  gw.setPALevel(RF24_PA_MAX);

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Bathroom1", "1.0");

  // Fetch the number of attached temperature sensors
  numSensors = sensors.getDeviceCount();

  // Present all sensors to controller
  int i = 0;
  for (i=0; i<numSensors && i<MAX_ATTACHED_DS18B20; i++) {
     gw.present(i, S_TEMP);
  }

  Serial.print("registered ");
  Serial.print(i);
  Serial.println(" onewire temperature device(s)");

  // Register all sensors to gw (they will be created as child devices)
  msgHum.setSensor(i);
  gw.present(i++, S_HUM);
  msgTemp.setSensor(i);
  gw.present(i++, S_TEMP);
}


void loop()
{
  // Process incoming messages (like config from server)
  gw.process();

  // Fetch temperatures from Dallas sensors
  sensors.requestTemperatures();

  // Read temperatures and send them to controller
  for (int i=0; i<numSensors && i<MAX_ATTACHED_DS18B20; i++) {

    // Fetch and round temperature to one decimal
    float temperature = static_cast<float>(static_cast<int>((gw.getConfig().isMetric?sensors.getTempCByIndex(i):sensors.getTempFByIndex(i)) * 10.)) / 10.;

    // Only send data if temperature has changed and no error
    if (lastTemperature[i] != temperature && temperature != -127.00) {
    	  Serial.print("sending temperature for device ");
    	  Serial.println(i);
      // Send in the new temperature
      gw.send(msg.setSensor(i).set(temperature,1));
      lastTemperature[i]=temperature;
    }
  }

  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
        Serial.println("Failed reading temperature from DHT");
    } else if (temperature != lastTemp) {
      lastTemp = temperature;
      if (!metric) {
        temperature = dht.toFahrenheit(temperature);
      }
      gw.send(msgTemp.set(temperature, 1));
      Serial.print("T: ");
      Serial.println(temperature);
    }
  float humidity = dht.getHumidity();
    if (isnan(humidity)) {
        Serial.println("Failed reading humidity from DHT");
    } else if (humidity != lastHum) {
        lastHum = humidity;
        gw.send(msgHum.set(humidity, 1));
        Serial.print("H: ");
        Serial.println(humidity);
    }

  gw.sleep(SLEEP_TIME);
}



