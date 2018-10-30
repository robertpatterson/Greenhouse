/*
5V+ -> 4.7K resister -> sensor wire 2
Sensor wires 1 and 3 to grownd
ALSO Arduino pin 2 to sensor wire 2


 */

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("_18B20_usingDallasTempLib"); //Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
}


void loop(void)
{ 
  float celsius;
  float fahrenheit;
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  celsius = sensors.getTempCByIndex(0);
  fahrenheit = celsius * 1.8 + 32.0;
//  Serial.print(fahrenheit);
  Serial.print("Temperature for Device 1 is: ");
  Serial.print(fahrenheit);
  Serial.println();
  //Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print("Temperature for Device 2 is: ");
  celsius = sensors.getTempCByIndex(1); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print(fahrenheit);
  Serial.println();
  delay(3000);
}

