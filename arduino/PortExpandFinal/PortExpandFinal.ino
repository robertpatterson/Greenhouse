


/*
 Example 41.1 - Microchip MCP23017 with Arduino
 http://tronixstuff.com/tutorials > chapter 41
 John Boxall | CC by-sa-nc
*/
// pins 15~17 to GND, I2C bus address is 0x20
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Ethernet server
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //assign arduino mac address
byte ip[] = {192, 168, 1, 102 }; // ip in lan assigned to arduino
byte gateway[] = {192, 168, 1, 1 }; // internet access via router
byte subnet[] = {255, 255, 255, 0 }; //subnet mask
EthernetServer server(8080); //server port arduino server will use
EthernetClient client;
char serverName[] = "robertpattersonus.com"; ///dev/YS_Charts/php/controller/canvas.php'; // (DNS) zoomkat's test web page server
//byte serverName[] = {127,0,0,1}; // 208, 104, 2, 86 }; // (IP) zoomkat web page server IP address

String readString; //used by server to capture GET request 




void setup()
{
   Wire.begin(); // wake up I2C bus
  // set I/O pins to outputs
   Wire.beginTransmission(0x20);
   Wire.write(0x00); // IODIRA register
   Wire.write(0x00); // set all of port A to outputs
   Wire.endTransmission();
   Wire.beginTransmission(0x20);
   Wire.write(0x01); // IODIRB register
   Wire.write(0x00); // set all of port B to outputs
   Wire.endTransmission();
   
    // start serial port
    Serial.begin(9600);
    Serial.println("_18B20_usingDallasTempLib"); //Dallas Temperature IC Control Library Demo");
  
    // Start up the library
    sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
   
   // Ethernet setup
   Wire.begin(); // wake up I2C bus
// set I/O pins to outputs
   Wire.beginTransmission(0x20);
   Wire.write(0x00); // IODIRA register
   Wire.write(0x00); // set all of port A to outputs
   Wire.endTransmission();
    //pinMode(5, OUTPUT); //pin 5 selected to control
   Ethernet.begin(mac,ip,gateway,gateway,subnet); 
   server.begin();
   Serial.begin(9600); 
   Serial.println("server/client 1.0 test 7/03/12"); // keep track of what is loaded
   Serial.println("Send an g in serial monitor to test client"); // what to do to test client

}



void loop(){
  binaryCount(2,1);
  binaryCount(2,128);
  getTemperatures();



  // check for serial input
  if (Serial.available() > 0) 
  {
    byte inChar;
    inChar = Serial.read();
    if(inChar == 'g')
    {
      sendGET(); // call client sendGET function
    //    binaryCount();
     //   delay(500);
    }
  }  

  EthernetClient client = server.available();
  
  // Ethernet -------------
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string 
          readString += c; 
          //Serial.print(c);
        } 

        //if HTTP request has ended
        if (c == '\n') {

          ///////////////
          Serial.print(readString); //print to serial monitor for debuging 

            //now output HTML data header
          if(readString.indexOf('?') >=0) { //don't send new page
            client.println(F("HTTP/1.1 204 Zoomkat"));
            client.println();
            client.println();  
          }
          else {   
            client.println(F("HTTP/1.1 200 OK")); //send new page on browser request
            client.println(F("Content-Type: text/html"));
            client.println();

            client.println(F("<HTML>"));
            client.println(F("<HEAD>"));
            client.println(F("<TITLE>Arduino GET test page</TITLE>"));
            client.println(F("</HEAD>"));
            client.println(F("<BODY>"));

            client.println(F("<H1>Zoomkat's simple Arduino 1.0 button</H1>"));

            // DIY buttons
            client.println(F("Pin5"));
            client.println(F("<a href=/?on2 target=inlineframe>ON</a>")); 
            client.println(F("<a href=/?off3 target=inlineframe>OFF</a><br><br>")); 

            client.println(F("Pin6"));
            client.println(F("<a href=/?on4 target=inlineframe>ON</a>")); 
            client.println(F("<a href=/?off5 target=inlineframe>OFF</a><br><br>")); 

            client.println(F("Pin7"));
            client.println(F("<a href=/?on6 target=inlineframe>ON</a>")); 
            client.println(F("<a href=/?off7 target=inlineframe>OFF</a><br><br>")); 

            client.println(F("Pin8"));
            client.println(F("<a href=/?on8 target=inlineframe>ON</a>")); 
            client.println(F("<a href=/?off9 target=inlineframe>OFF</a><br><br>")); 

            client.println(F("Pins"));
            client.println(F("&nbsp;<a href=/?off2468 target=inlineframe>ALL ON</a>")); 
            client.println(F("&nbsp;<a href=/?off3579 target=inlineframe>ALL OFF</a>")); 

            client.println(F("<IFRAME name=inlineframe style='display:none'>"));          
            client.println(F("</IFRAME>"));

            client.println(F("</BODY>"));
            client.println(F("</HTML>"));
          }

          delay(1);
          //stopping client
          client.stop();

          ///////////////////// control arduino pin
          if(readString.indexOf('2') >0)//checks for 2
          {
            digitalWrite(5, HIGH);    // set pin 5 high
            Serial.println("Led 5 On");
            Serial.println();
          }
          if(readString.indexOf('3') >0)//checks for 3
          {
            digitalWrite(5, LOW);    // set pin 5 low
            Serial.println("Led 5 Off");
            Serial.println();
          }
          if(readString.indexOf('4') >0)//checks for 4
          {
            digitalWrite(6, HIGH);    // set pin 6 high
            Serial.println("Led 6 On");
            Serial.println();
          }
          if(readString.indexOf('5') >0)//checks for 5
          {
            digitalWrite(6, LOW);    // set pin 6 low
            Serial.println("Led 6 Off");
            Serial.println();
          }
          if(readString.indexOf('6') >0)//checks for 6
          {
            digitalWrite(7, HIGH);    // set pin 7 high
            Serial.println("Led 7 On");
            Serial.println();
          }
          if(readString.indexOf('7') >0)//checks for 7
          {
            digitalWrite(7, LOW);    // set pin 7 low
            Serial.println("Led 7 Off");
            Serial.println();
          }     
          if(readString.indexOf('8') >0)//checks for 8
          {
            digitalWrite(8, HIGH);    // set pin 8 high
            Serial.println("Led 8 On");
            Serial.println();
          }
          if(readString.indexOf('9') >0)//checks for 9
          {
            digitalWrite(8, LOW);    // set pin 8 low
            Serial.println("Led 8 Off");
            Serial.println();
          }         

          //clearing string for next read
          readString="";

        }
      }
    }
  }  
  
  
}

void getTemperatures(){ 
  /*
 One wire multi sensor -----------------------
5V+ -> 4.7K resister -> sensor wire 2
Sensor wires 1 and 3 to grownd
ALSO Arduino pin 2 to sensor wire 2
 */  
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

/* Port expander manager
PinCodes A side (on right of cip)
Pin     1    2    3    4    5    6    7    8
Code# 128   64   32   16    8    4    2    1

PinCodes B side (on left of cip)
Pin     1    2    3    4    5    6    7    8
Code#   1    2    4    8   16   32   64  128

*/
void binaryCount(int group, int pinCode){  // group 1 = A; B = 2
  Wire.beginTransmission(0x20);
  if(group == 1){
     Wire.write(0x12); // GPIOA    
  }
  else{      
     Wire.write(0x13); // GPIOB
  }
   Wire.write(pinCode);
   Wire.endTransmission();
}

//// Ethernet -----------//////////////////////
void sendGET(){ //client function to send and receive GET data from external server.
  if (client.connect(serverName, 80)) {
    Serial.println("connected");
   // client.println("GET /~shb/arduino.txt HTTP/1.0");
   // client.println("GET /index.html");
    client.println("GET index.html HTTP/1.0");
    client.println();
  } 
  else {
    Serial.println("connection failed");
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read();
    Serial.print(c);
  }

  Serial.println();
  Serial.println("g");
  Serial.println("==================");
  Serial.println();
  client.stop();

}

