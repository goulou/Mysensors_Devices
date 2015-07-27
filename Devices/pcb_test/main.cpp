// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <NewPing.h>

#include <eeprom_reset.hpp>
#include <1w_node.hpp>
#include <dht_node.hpp>
#include <battery_monitored_node.hpp>
#include <printf.hpp>

#include <si7021_node.hpp>

MySensor gw;
unsigned long SLEEP_TIME = 100; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0

#define PRESENCE_DETECTOR_PIN  PD5
boolean presence_detector_value_previous = false;
boolean established_detector_value= false;

#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     A1  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup()
{
	Serial.begin(BAUD_RATE);
	int i=0;
	Serial.println("launched");
	printf_begin();

	pinMode(ECHO_PIN, INPUT);
	pinMode(TRIGGER_PIN, OUTPUT);

//	eeprom_reset_check(3);

	Serial.println("begin");

	return;
	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin();
//	Serial.println("setPALevel");
//	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	Serial.println("sendSketchInfo");
	gw.sendSketchInfo("pcb_test", "1.0");

	Serial.println("setup1w");

//	onewire_node_setup(gw);
	setup_si7021(gw, 10);
	battery_monitored_node_setup(gw, BATTERY_SENSE_PIN);
}

void loop()
{
	// Process incoming messages (like config from server)
//	gw.process();

	boolean present = digitalRead(PRESENCE_DETECTOR_PIN);
	if(present != established_detector_value)
	{
		if(present == presence_detector_value_previous)
		{
			established_detector_value = present;
//			if(present)
//			{
//				DEBUG_PRINT_ln("PRESENT");
//			}
//			else
//			{
//				DEBUG_PRINT_ln("NOT PRESENT");
//			}
		}
	}
//	DEBUG_PRINT_ln(present);
	presence_detector_value_previous = present;

//	onewire_node_loop(gw);
//	loop_si7021(gw);
//	battery_monitored_node_loop(gw);

//	gw.sleep(SLEEP_TIME);
	 // establish variables for duration of the ping,
	  // and the distance result in inches and centimeters:
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping: ");
  Serial.print(uS); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.print("us, =");
  Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");
}

