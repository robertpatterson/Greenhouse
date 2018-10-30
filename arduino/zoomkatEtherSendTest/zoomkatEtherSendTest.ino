
//zoomkat 11-04-13
//simple client test
//for use with IDE 1.0.1
//with DNS, DHCP, and Host
//open serial monitor and send an e to test client GET
//for use with W5100 based ethernet shields
//remove SD card if inserted
//data from weather server captured in readString 

#include <SPI.h>
#include <Ethernet.h>
String readString;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address

char serverName[] = "robertpattersonus.com"; // myIP server test web page server
EthernetClient client;
long previousMillis = 0;

//////////////////////

void setup(){

  Serial.begin(9600); 
  Serial.println("client readString test 11/04/13"); // so I can keep track of what is loaded
  Serial.println("Send an e in serial monitor to test"); // what to do to test
  Serial.println();
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while(true);
  }
  Serial.println("ether started");
}

void loop(){
  // check for serial input
/*  if (Serial.available() > 0) //if something in serial buffer
  {
    byte inChar; // sets inChar as a byte
    inChar = Serial.read(); //gets byte from buffer
    if(inChar == 'e') // checks to see byte is an e
    {   */
      sendGET(); // call sendGET function below when byte is an e
//    }
//  }  
} 

//////////////////////////

void sendGET() //client function to send/receive GET request data.
{
  client.stop();
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > 30000){  // Do it every .
        previousMillis = currentMillis; 
    


  
  if (client.connect(serverName, 80)) {  //starts client connection, checks for connection
    Serial.println("connected");
    client.println(F("GET /arduino/php/greenhouse.php?task=arduinoCalling&callType=conditionsReport HTTP/1.1"));

 //   client.println("GET /arduino/html/test.html,us HTTP/1.1"); //download text
    client.println("Host: robertpattersonus.com");
    client.println("Connection: close");  //close 1.1 persistent connection  
    client.println(); //end of get request
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    readString += c; //places captured byte in readString
  }

  //Serial.println();
  client.stop(); //stop client
  Serial.println("client disconnected.");
  Serial.println("Data from server captured in readString:");
  Serial.println();
  Serial.print(readString); //prints readString to serial monitor 
  Serial.println();  
  Serial.println();
  Serial.println("End of readString");
  Serial.println("=======e===========");
  Serial.println();
  readString=""; //clear readString variable
    }
}

