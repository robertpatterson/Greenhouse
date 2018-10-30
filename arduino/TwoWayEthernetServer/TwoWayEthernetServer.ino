//zoomkat 7-03-12, combined client and server
//simple button GET with iframe code
//for use with IDE 1.0
//open serial monitor and send an g to test client and
//see what the arduino client/server receives
//web page buttons make pin 5 high/low
//use the ' in html instead of " to prevent having to escape the "
//address will look like http://192.168.1.102:84 when submited
//for use with W5100 based ethernet shields
//note that the below bug fix may be required
// http://code.google.com/p/arduino/issues/detail?id=605

#include <SPI.h>
#include <Ethernet.h>
#include "Wire.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //assign arduino mac address
byte ip[] = {192,168,1,102}; // ip in lan assigned to arduino
byte gateway[] = {192,168,1,1}; // internet access via router
byte subnet[] = {255, 255, 255,0}; //subnet mask
EthernetServer server(8080); //server port arduino server will use
EthernetClient client;
char serverName[] = "robertpattersonus.com"; ///dev/YS_Charts/php/controller/canvas.php'; // (DNS) zoomkat's test web page server
//byte serverName[] = {192,168,1,102}; // 208, 104, 2, 86 }; // (IP) zoomkat web page server IP address

String readString; //used by server to capture GET request 


void setup(){

 Wire.begin(); // wake up I2C bus
// set I/O pins to outputs
 Wire.beginTransmission(0x20);
 Wire.write(0x00); // IODIRA register
 Wire.write(0x00); // set all of port A to outputs
 Wire.endTransmission();

   
/*    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
  
  pinMode(5, OUTPUT); //pin selected to control
  pinMode(6, OUTPUT); //pin selected to control
  pinMode(7, OUTPUT); //pin selected to control
  pinMode(8, OUTPUT); //pin selected to control
*/
  //pinMode(5, OUTPUT); //pin 5 selected to control
  Ethernet.begin(mac,ip,gateway,gateway,subnet); 
  server.begin();
  Serial.begin(9600); 
  Serial.println("server/client 1.0 test 7/03/12"); // keep track of what is loaded
  Serial.println("Send an g in serial monitor to test client"); // what to do to test client


    //    binaryCount();
     //   delay(1000);
}

void binaryCount()
{
  Serial.println("in binaryCount");
 for (byte a=0; a<250; a++)
 {
     Wire.beginTransmission(0x20);
     Wire.write(0x12); // GPIOA
     Wire.write(a); // port A  (7 =Pin 2
     Wire.endTransmission();
     Serial.println("sent output to ");
     Serial.println(a);
      delay(500);
 }
}



void loop(){
  // check for serial input
  if (Serial.available() > 0) {
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

//////////////////////////
void sendGET() //client function to send and receive GET data from external server.
{
  if (client.connect(serverName, 80)) {
    Serial.println("connected");
   // client.println("GET /~shb/arduino.txt HTTP/1.0");
  //  client.println("GET index.html HTTP/1.0");
  client.println("GET /arduino/php/greenhouse.php?task=update&temps=450550670800 HTTP/1.1");
  client.println("Host: www.robertpattersonus.com");
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

