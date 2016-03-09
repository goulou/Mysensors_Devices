// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>
#include <NewPing.h>

#include <eeprom_reset.hpp>
#include <1w_node.hpp>
#include <dht_node.hpp>
#include <battery_monitored_node.hpp>
#include <si7021_node.hpp>
#include <serial.hpp>

MySensor gw;
unsigned long SLEEP_TIME = 5000; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0

#define PRESENCE_DETECTOR_PIN  PD5
boolean presence_detector_value_previous = false;
boolean established_detector_value= false;

#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     A1  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


#define UPDATE_INCREMENT (31000/100)
void incomingMessage(const MyMessage &message);

void waitForResponse(unsigned long baudrate, boolean print_if_found) {
    delay(1000);
    boolean found = false;
    while (Serial.available()) {
    	Serial.read();
    	delay(50);
//      Serial.write(Serial.read());
      found = true;
    }
//    Serial.write("\n");
    if(found && print_if_found)
    {
    	Serial.print("found baudrate : ");
    	Serial.println(baudrate);
    	delay(1100);
    }
}

void recover_serial_speed(unsigned long baudrate)
{
	Serial.flush();
	delay(500);
	Serial.begin(9600);
	Serial.flush();
	delay(500);
	Serial.print("Starting config:");
	Serial.println(baudrate);
	Serial.flush();
	delay(500);
	Serial.begin(baudrate);
	Serial.flush();
	delay(1500);

	// Should respond with OK
	Serial.print("AT");
	Serial.flush();
	waitForResponse(baudrate, true);

	// Should respond with its version
	Serial.flush();
	delay(1000);
	Serial.print("AT+VERSION");
	waitForResponse(baudrate, true);

	Serial.println("Done!");
}

void recover_serial()
{
	unsigned long rates[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 1382400};
	for(int i=0; i<12; i++)
	{
		recover_serial_speed(rates[i]);
		delay(2000);
	}
}

void reconfig_serial(unsigned long intial_baudrate)
{
	Serial.flush();
	delay(500);
	Serial.begin(9600);
	Serial.flush();
	delay(500);
	Serial.print("Starting config:");
	Serial.println(intial_baudrate);
	Serial.flush();
	delay(500);
	Serial.begin(intial_baudrate);
	Serial.flush();
	delay(1500);

	  // Should respond with OK
	  Serial.print("AT");
	  waitForResponse(intial_baudrate, true);

	  // Should respond with its version
	  Serial.print("AT+VERSION");
	  waitForResponse(intial_baudrate, false);

	  // Set pin to 0000
	  Serial.print("AT+PIN0000");
	  waitForResponse(intial_baudrate, false);

	  // Set the name to ROBOT_NAME
	  Serial.print("AT+NAME");
	  Serial.print("MCU_2");
	  waitForResponse(intial_baudrate, false);

	  // Set baudrate to 9600
	  Serial.print("AT+BAUD4");
	  waitForResponse(intial_baudrate, false);

	  Serial.println("Done!");
}


void setup()
{
	reconfig_serial(9600);
	recover_serial();
	while(true){}
	return;
	Serial.begin(BAUD_RATE);
	int i=0;
	Serial.println("launched");
	setup_serial();

	pinMode(ECHO_PIN, INPUT);
	pinMode(TRIGGER_PIN, OUTPUT);

//	eeprom_reset_check(3);

	Serial.println("begin");

	setup_dht(gw, 7, 5, false);

	// Startup and initialize MySensors library. Set callback for incoming messages.
	gw.begin(incomingMessage, 254, false, GATEWAY_ADDRESS);
//	Serial.println("setPALevel");
//	gw.setPALevel(RF24_PA_MAX);

	// Send the sketch version information to the gateway and Controller
	Serial.println("sendSketchInfo");
	gw.sendSketchInfo(xstr(PROGRAM_NAME), "1.0");

	gw.present(64, S_COVER);
	gw.request(64, V_DIMMER);

	present_dht(gw);
//	Serial.println("setup1w");
//	onewire_node_setup(gw);
	setup_si7021(gw, 10);
	battery_monitored_node_setup(gw, BATTERY_SENSE_PIN);
}

void update_status();

void loop()
{
	// Process incoming messages (like config from server)
	gw.process();

//	boolean present = digitalRead(PRESENCE_DETECTOR_PIN);
//	if(present != established_detector_value)
//	{
//		if(present == presence_detector_value_previous)
//		{
//			established_detector_value = present;
////			if(present)
////			{
////				DEBUG_PRINT_ln("PRESENT");
////			}
////			else
////			{
////				DEBUG_PRINT_ln("NOT PRESENT");
////			}
//		}
//	}
//	DEBUG_PRINT_ln(present);
//	presence_detector_value_previous = present;

//	onewire_node_loop(g w);
//	loop_si7021(gw);
//	loop_dht(gw);
//	battery_monitored_node_loop(gw);
	long end = millis() + SLEEP_TIME;
	while(millis()<end)
	{
		gw.process();
		update_status();
	}
	Serial.print(".");
	 // establish variables for duration of the ping,
	  // and the distance result in inches and centimeters:
//  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
//  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
//  Serial.print("Ping: ");
//  Serial.print(uS); // Convert ping time to distance in cm and print result (0 = outside set distance range)
//  Serial.print("us, =");
//  Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
//  Serial.println("cm");
}

static int direction = 0;
static int set_point = -1;
static long next_update = 0;
static long next_send_update = 0;
static int position = 0;
MyMessage dimmerMsg(64, V_DIMMER);

void update_status()
{
	if(millis() > next_update)
	{
		if(direction < 0 && position>0)
		{
			position += direction;
		}
		else if(direction > 0 && position<100)
		{
			position += direction;
		}

		if(set_point != -1 && set_point == position)
		{
			set_point = -1;
			direction = 0;
		}

		if(dimmerMsg.getInt() != position && (millis() > next_send_update || direction==0))
		{
			gw.send(dimmerMsg.set(position));
			next_send_update = millis() + 1000;
		}

		next_update = millis() + UPDATE_INCREMENT*abs(direction);
	}
}

void incomingMessage(const MyMessage &message)
{
	switch (message.type)
	{
		case V_VAR1:
		{
			Serial.print("Incoming change for V_VAR1:");
			Serial.print(message.sensor);
			Serial.print(", New status: ");
			Serial.println(message.getInt());
			break;
		}
		case V_DIMMER:
		{
			Serial.print("Incoming change for V_DIMMER:");
			Serial.print(message.sensor);
			Serial.print(", New status: ");
			Serial.println(message.getInt());
			set_point = message.getInt();
			if(set_point > position)
			{
				direction = 1;
			}
			else if(set_point < position)
			{
				direction = -1;
			}
			break;
		}
		case V_UP:
		{
			Serial.print("Incoming change for V_UP:");
			Serial.print(message.sensor);
			Serial.print(", New status: ");
			Serial.println(message.getInt());
			direction = -1;
			break;
		}
		case V_DOWN:
		{
			Serial.print("Incoming change for V_DOWN:");
			Serial.print(message.sensor);
			Serial.print(", New status: ");
			Serial.println(message.getInt());
			direction = 1;
			break;
		}
		case V_STOP:
		{
			Serial.print("Incoming change for V_STOP:");
			Serial.print(message.sensor);
			Serial.print(", New status: ");
			Serial.println(message.getInt());
			direction = 0;
			set_point = -1;
			break;
		}
		default:
		{
			Serial.print("unknown message type : ");
			Serial.print(message.type);
			Serial.print(" -- ");
			Serial.print(message.sensor);
			Serial.print(", New status: ");
			Serial.println(message.getBool());
		}
	}
}
