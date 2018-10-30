// from: http://tronixstuff.com/2013/12/06/arduino-tutorials-chapter-16-ethernet/

#include <SPI.h>
#include <Ethernet.h>

// for DHT22 sensor
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {   0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10,1,1,77);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(125);
DHT dht(DHTPIN, DHTTYPE);

void setup() 
{
  dht.begin();
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() 
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) 
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  client.println("Refresh: 30");  // refresh the page automatically every 30 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          // get data from DHT22 sensor
          float h = dht.readHumidity();
          float t = dht.readTemperature();
          Serial.println(t);
          Serial.println(h);

          // from here we can enter our own HTML code to create the web page
          client.print("<head><title>Office Weather</title></head><body><h1>Office Temperature</h1><p>Temperature - ");
          client.print(t);
          client.print(" degrees Celsius</p>");
          client.print("<p>Humidity - ");
          client.print(h);
          client.print(" percent</p>");
          client.print("<p><em>Page refreshes every 30 seconds.</em></p></body></html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}