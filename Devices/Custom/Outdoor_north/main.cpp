
#ifndef BAUD_RATE
#define BAUD_RATE 9600
#endif
#define MY_BAUD_RATE BAUD_RATE

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <SPI.h>
#include <MySensors.h>

#define SKETCH_NAME xstr(PROGRAM_NAME)
#define SKETCH_MAJOR_VER "2"
#define SKETCH_MINOR_VER "0"

#define DEBUG
#include <serial.hpp>

#include <eeprom_reset.hpp>
#include <battery_monitored_node.hpp>
#include <si7021_node.hpp>
unsigned long SLEEP_TIME = 60000*5; // Sleep time between reads (in milliseconds)

#define BATTERY_SENSE_PIN  A0

#include <Adafruit_BMP085.h>
#define BARO_CHILD_ID				130
Adafruit_BMP085 bmp = Adafruit_BMP085();      // Digital Pressure Sensor
float lastPressure = -1;
int lastForecast = -1;
const char *weather[] =
{ "stable", "sunny", "cloudy", "unstable", "thunderstorm", "unknown" };
int minutes;
float pressureSamples[45];
int minuteCount = 0;
bool firstRound = true;
float pressureAvg[7];
float dP_dt;
int compute_forecast(float pressure);
MyMessage pressureMsg(BARO_CHILD_ID, V_PRESSURE);
MyMessage forecastMsg(BARO_CHILD_ID, V_FORECAST);

boolean bmp_present = false;
void setup()
{

	Serial.begin(BAUD_RATE);
	setup_serial();
	Serial.println();
	Serial.println("launched");
	wdt_reset();

	eeprom_reset_check(4);
	setup_si7021(5, false, true);
	wdt_reset();

	uint8_t i = 0;
	/****************************
	         init BMP180
	 ****************************/
	while (!bmp.begin(BMP085_ULTRALOWPOWER) && i<5)
	{
		DEBUG_PRINT_ln("Could not find a valid BMP085 sensor, check wiring!");
		delay(500);
		i++;
		DEBUG_PRINT_ln("retrying");

	}

	if(i<5)
	{
		bmp_present = true;
	}

	if(bmp_present)
	{
		float pressure = bmp.readSealevelPressure(81) / 100; // 205 meters above sealevel
		int forecast = compute_forecast(pressure);

		DEBUG_PRINT("Pressure = ");
		DEBUG_PRINT(pressure);
		DEBUG_PRINT_ln(" Pa");
		DEBUG_PRINT_ln(weather[forecast]);
	}
	/****************************
	       end init BMP180
	 ****************************/

}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
	battery_monitored_node_setup(BATTERY_SENSE_PIN);
	present_si7021();

	/****************************
		   present BMP180
	 ****************************/
	if(bmp_present)
	{
		present(BARO_CHILD_ID, S_BARO);
	}
}



int lastRainValue = -1;

void loop()
{
	if(bmp_present)
	{

		/**************************************************/
		/***********    Compute forecast    ***************/
		/**************************************************/

		float pressure = bmp.readSealevelPressure(81) / 100; // 205 meters above sealevel
		int forecast = compute_forecast(pressure);

		DEBUG_PRINT("Pressure = ");
		DEBUG_PRINT(pressure);
		DEBUG_PRINT_ln(" Pa");
		DEBUG_PRINT_ln(weather[forecast]);

		if (abs(pressure - lastPressure) > 0.5)
		{
			send(pressureMsg.set(pressure, 0));
			lastPressure = pressure;
		}

		if (forecast != lastForecast)
		{
			send(forecastMsg.set(weather[forecast]));
			lastForecast = forecast;
		}

		/*
		 DP/Dt explanation

		 0 = "Stable Weather Pattern"
		 1 = "Slowly rising Good Weather", "Clear/Sunny "
		 2 = "Slowly falling L-Pressure ", "Cloudy/Rain "
		 3 = "Quickly rising H-Press",     "Not Stable"
		 4 = "Quickly falling L-Press",    "Thunderstorm"
		 5 = "Unknown (More Time needed)
		 */
	}

	// Process incoming messages (like config from server)
	loop_si7021();
	battery_monitored_node_loop();

	wdt_disable();
	sleep(SLEEP_TIME);
	wdt_enable(WDTO_8S);
	wdt_reset();
}

float get_pressure_sample(int idx)
{
	return pressureSamples[idx/4];
}

int compute_forecast(float pressure)
{
	// Algorithm found here
	// http://www.freescale.com/files/sensors/doc/app_note/AN3914.pdf
	if (minuteCount == 180)
		minuteCount = 5;

	pressureSamples[minuteCount/4] = pressure;
	minuteCount++;

	if (minuteCount == 5)
	{
		// Avg pressure in first 5 min, value averaged from 0 to 5 min.
		pressureAvg[0] = ((get_pressure_sample(0) + get_pressure_sample(1) + get_pressure_sample(2) + get_pressure_sample(3)
				+ get_pressure_sample(4)) / 5);
	}
	else if (minuteCount == 35)
	{
		// Avg pressure in 30 min, value averaged from 0 to 5 min.
		pressureAvg[1] = ((get_pressure_sample(30) + get_pressure_sample(31) + get_pressure_sample(32) + get_pressure_sample(33)
				+ get_pressure_sample(34)) / 5);
		float change = (pressureAvg[1] - pressureAvg[0]);
		if (firstRound) // first time initial 3 hour
			dP_dt = ((65.0 / 1023.0) * 2 * change); // note this is for t = 0.5hour
		else
			dP_dt = (((65.0 / 1023.0) * change) / 1.5); // divide by 1.5 as this is the difference in time from 0 value.
	}
	else if (minuteCount == 60)
	{
		// Avg pressure at end of the hour, value averaged from 0 to 5 min.
		pressureAvg[2] = ((get_pressure_sample(55) + get_pressure_sample(56) + get_pressure_sample(57) + get_pressure_sample(58)
				+ get_pressure_sample(59)) / 5);
		float change = (pressureAvg[2] - pressureAvg[0]);
		if (firstRound) //first time initial 3 hour
			dP_dt = ((65.0 / 1023.0) * change); //note this is for t = 1 hour
		else
			dP_dt = (((65.0 / 1023.0) * change) / 2); //divide by 2 as this is the difference in time from 0 value
	}
	else if (minuteCount == 95)
	{
		// Avg pressure at end of the hour, value averaged from 0 to 5 min.
		pressureAvg[3] = ((get_pressure_sample(90) + get_pressure_sample(91) + get_pressure_sample(92) + get_pressure_sample(93)
				+ get_pressure_sample(94)) / 5);
		float change = (pressureAvg[3] - pressureAvg[0]);
		if (firstRound) // first time initial 3 hour
			dP_dt = (((65.0 / 1023.0) * change) / 1.5); // note this is for t = 1.5 hour
		else
			dP_dt = (((65.0 / 1023.0) * change) / 2.5); // divide by 2.5 as this is the difference in time from 0 value
	}
	else if (minuteCount == 120)
	{
		// Avg pressure at end of the hour, value averaged from 0 to 5 min.
		pressureAvg[4] = ((get_pressure_sample(115) + get_pressure_sample(116) + get_pressure_sample(117) + get_pressure_sample(118)
				+ get_pressure_sample(119)) / 5);
		float change = (pressureAvg[4] - pressureAvg[0]);
		if (firstRound) // first time initial 3 hour
			dP_dt = (((65.0 / 1023.0) * change) / 2); // note this is for t = 2 hour
		else
			dP_dt = (((65.0 / 1023.0) * change) / 3); // divide by 3 as this is the difference in time from 0 value
	}
	else if (minuteCount == 155)
	{
		// Avg pressure at end of the hour, value averaged from 0 to 5 min.
		pressureAvg[5] = ((get_pressure_sample(150) + get_pressure_sample(151) + get_pressure_sample(152) + get_pressure_sample(153)
				+ get_pressure_sample(154)) / 5);
		float change = (pressureAvg[5] - pressureAvg[0]);
		if (firstRound) // first time initial 3 hour
			dP_dt = (((65.0 / 1023.0) * change) / 2.5); // note this is for t = 2.5 hour
		else
			dP_dt = (((65.0 / 1023.0) * change) / 3.5); // divide by 3.5 as this is the difference in time from 0 value
	}
	else if (minuteCount == 180)
	{
		// Avg pressure at end of the hour, value averaged from 0 to 5 min.
		pressureAvg[6] = ((get_pressure_sample(175) + get_pressure_sample(176) + get_pressure_sample(177) + get_pressure_sample(178)
				+ get_pressure_sample(179)) / 5);
		float change = (pressureAvg[6] - pressureAvg[0]);
		if (firstRound) // first time initial 3 hour
			dP_dt = (((65.0 / 1023.0) * change) / 3); // note this is for t = 3 hour
		else
			dP_dt = (((65.0 / 1023.0) * change) / 4); // divide by 4 as this is the difference in time from 0 value
		pressureAvg[0] = pressureAvg[5]; // Equating the pressure at 0 to the pressure at 2 hour after 3 hours have past.
		firstRound = false; // flag to let you know that this is on the past 3 hour mark. Initialized to 0 outside main loop.
	}

	if (minuteCount < 35 && firstRound) //if time is less than 35 min on the first 3 hour interval.
		return 5; // Unknown, more time needed
	else if (dP_dt < (-0.25))
		return 4; // Quickly falling LP, Thunderstorm, not stable
	else if (dP_dt > 0.25)
		return 3; // Quickly rising HP, not stable weather
	else if ((dP_dt > (-0.25)) && (dP_dt < (-0.05)))
		return 2; // Slowly falling Low Pressure System, stable rainy weather
	else if ((dP_dt > 0.05) && (dP_dt < 0.25))
		return 1; // Slowly rising HP stable good weather
	else if ((dP_dt > (-0.05)) && (dP_dt < 0.05))
		return 0; // Stable weather
	else
		return 5; // Unknown
}

