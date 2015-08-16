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
unsigned long SLEEP_TIME = 60000; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0

#define IO_5V_SWITCH	A2
#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     A1  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


MyMessage msg(0, V_DISTANCE);
unsigned int get_distance_cm();
unsigned int get_distance_cm_long_median();

void setup()
{
	Serial.begin(BAUD_RATE);
	Serial.println("launched");
	printf_begin();

//	eeprom_reset_check(3);
	pinMode(ECHO_PIN, INPUT);
	pinMode(TRIGGER_PIN, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(IO_5V_SWITCH, OUTPUT);
//	analogWrite(IO_5V_SWITCH, 255);


	Serial.println("begin");
	setup_si7021(gw, 10, false);
	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(NULL, 70);
	Serial.print(get_distance_cm_long_median()); // Convert ping time to distance in cm and print result (0 = outside set distance range)
	Serial.println("cm");
//	Serial.println("setPALevel");
//	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	Serial.println("sendSketchInfo");
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	Serial.println("setup1w");
	// Startup OneWire
	present_si7021(gw);

	gw.present(0, S_DISTANCE);

	battery_monitored_node_setup(gw, BATTERY_SENSE_PIN);
}

unsigned int get_distance_cm()
{
	Serial.println("turn on");
	digitalWrite(IO_5V_SWITCH, HIGH);
	//Let the IO be on
	gw.sleep(1000);
	unsigned int uS = sonar.ping_median(20); // Send ping, get ping time in microseconds (uS).
	Serial.println("turn off");
	digitalWrite(IO_5V_SWITCH, LOW);
	Serial.print("Ping: ");
	Serial.print(uS); // Convert ping time to distance in cm and print result (0 = outside set distance range)
	Serial.print("us, =");
	Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
	Serial.println("cm");
	return uS/US_ROUNDTRIP_CM;
}

unsigned int get_distance_cm_long_median()
{
	int it = 10;
	unsigned int cm[10], last;
	uint8_t j, i = 0;
	cm[0] = NO_ECHO;
	while (i < it) {
		unsigned int last = get_distance_cm(); // Send ping, get ping time in microseconds (uS).
		if (last == NO_ECHO) {   // Ping out of range.
			it--;                // Skip, don't include as part of median.
		} else {                       // Ping in range, include as part of median.
			if (i > 0) {               // Don't start sort till second ping.
				for (j = i; j > 0 && cm[j - 1] < last; j--) // Insertion sort loop.
					cm[j] = cm[j - 1]; // Shift ping array to correct position for sort insertion.
			} else j = 0;              // First ping is starting point for sort.
			cm[j] = last;              // Add last ping to array in sorted position.
			i++;                       // Move to next ping.
		}
		if (i < it) gw.sleep(2000); // Millisecond delay between pings.
	}

	return (cm[it >> 1]); // Return the ping distance median.
}

unsigned long counter = 10;

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();

	loop_si7021(gw);
	battery_monitored_node_loop(gw);

	//Dont't use 'millis()' to known how long we slept : this counter does NOT get incremented while in gw.sleep()!
	if(counter >= 10)
	{
		int d = get_distance_cm_long_median();
		gw.send(msg.set(d));
		counter = 0;
	}
	counter++;


	gw.sleep(SLEEP_TIME);
}

