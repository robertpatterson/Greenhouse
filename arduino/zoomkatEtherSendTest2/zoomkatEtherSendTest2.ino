
//zoomkat 11-04-13
//simple client test
//for use with IDE 1.0.1
//with DNS, DHCP, and Host
//open serial monitor and send an e to test client GET
//for use with W5100 based ethernet shields
//remove SD card if inserted
//data from weather server captured in readString 
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address

char serverName[] = "robertpattersonus.com"; // myIP server test web page server
EthernetClient client;


//////////////////////

// -------------- GLOBAL VARs AND OBJECTS -----------------------------------------------

String readString; //used by server to capture GET request
String jobsList;
int tempArray[5];
int ventState = 0;
int collectorPumpState = 0;
int panelsState = 0;  // == down/open
int pinStateCurrent[2] = {255,255};
int pinCodeRunUntimed[2]= {255,255};
//void updatePinStates(int startStop, int group,int code);

//--------------------------- ONE WIRE ------------------------------------------------

// Data wire is plugged into pin 2 on the Arduino  -----------------------
#define ONE_WIRE_BUS 2   // To DIGITAL pin 2! Braided wire w white line. 5V is othe braided and orang unbvraided is grnd
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);



//------------------  TIMERS -------------------------------------

long previousMillis = 0;        // will store last time LED was updated
long jobFinishTime = 0;
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
EthernetServer server = EthernetServer(23);


void setup(){

//  Serial.begin(9600); 
  Wire.begin(); // wake up I2C bus
  // set I/O pins to outputs
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // IODIRA register
  // Wire.write(255);  //set all pins high = relay off
  Wire.write(0x00); // set all of port A to outputs

  Wire.endTransmission();
  Wire.beginTransmission(0x20);
  Wire.write(0x01); // IODIRB register
  //   Wire.write(255);  //set all pins high = relay off
  Wire.write(0x00); // set all of port B to outputs
  Wire.endTransmission();
  sensors.begin();
  Serial.begin(9600);
  Serial.println(F("GreenhouseControls_3.10.16"));

//  Serial.println("client readString test 11/04/13"); // so I can keep track of what is loaded
 // Serial.println("Send an e in serial monitor to test"); // what to do to test
//  Serial.println();
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    // no point in carrying on, so do nothing forevermore:
    while(true);
  }
}
/* ---------------------------- loop ---------------------------------------
 * 
 * -------------------------------------------------------------------------
 */
void loop(){
  /*    EthernetClient client = server.available();  //gets client who has connected to server.
      if (client){
    	  Serial.println("in client");
        getInputString(client); 
      } 
      unsigned long currentMillis = millis(); 
      if(currentMillis > jobFinishTime){
      doOneTimedJob();   // There may be no jobs to do.                
  }  */
    sendGET();  
}   

//------------------------------------------------------------------------------

void getInputString(EthernetClient client){
  char c;
  boolean currentLineIsBlank = true;
  while (client.connected()){ // true = client has come calling
      if(client.available()){       
        c = client.read();
        readString += c;    // readString is a String object
        Serial.println(readString);
        if (c == '\n' && currentLineIsBlank){
          
          Serial.println(F("incoming string: "));
          Serial.println(readString);
          readString.replace("\n", "><");
  //        Serial.println(readString);
  //        Serial.println("----1------");            
          int questM = readString.indexOf('?') + 1;
          String cleanParams = readString.substring(questM);  // cleanParams = "task=...
         String response = doItOrQIt(cleanParams);
         sendResponse(response, client);
          break;              
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
         }else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
         }  
       }
       // out of client.available - runs many times
       sendGET();    
  }       
  delay(1);   // give the web browser time to receive the data
  // close the connection:
  client.stop();
  Serial.println(F("Ethernet client stopped"));
  readString = "";
}


void sendResponse(String response, EthernetClient client ){
    // send a standard http response header
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html"));
    client.println(F("Connection: close"));  // the connection will be closed after completion of the response
 //   client.println("Refresh: 60");  // refresh the page automatically every 30 sec
    client.println();
    client.println(response);
    Serial.print(F("response sent: "));
    Serial.println(response); 
} 


// determines if its an immediate job or one to be qued. Alsways sends a response
// cleanParams starts with 'task =.....'
String doItOrQIt(String cleanParams){
  String response = F("Ardoino says I dont know what to do with: ");
  Serial.println(cleanParams);
  
    // ----- Open/Close all panels --MUST COME BEFORE SETPINS IF STATEMENT -----------------------------
    if(find_text("closePanels-",cleanParams) > -1){
      panelsState = 1;  // Closed
      doItOrQIt(cleanParams.substring(12));
    }
    if(find_text("openPanels-",cleanParams) > -1){
      Serial.println(F("IN open panels params after shortening: "));
      panelsState = 0;  // open
      String tmp = cleanParams.substring(11);
      Serial.println(tmp);
      doItOrQIt(tmp);
    }   
  
    // --- A setPins order came ------------------
  
    if(find_text("setPins",cleanParams) > -1 ){   // returns index of pos
 //     Serial.println("task = setPins");
       jobsList += cleanParams;  // Put it on the list of jobs to do
  //    String tempTx = F("Current jobs list: ");
 //    Serial.println(tempTx + " " + jobsList);
       response = jobsList;
    }
    
    // ------- Non-timed tasks -------------------------
    
    if(find_text("currentStatus",cleanParams) > -1){
      response ="123-123-123-123-666";
      //response = getTemperatures();
    } 
    if(find_text("stopJobs",cleanParams) > -1){
  //    Serial.println("task = stop jobs");
      jobFinishTime = 0;
      jobsList = "";
      pinStateCurrent[0] = 255;  // Document everythging as off
      pinStateCurrent[1] = 255;
      pinCodeRunUntimed[0] = 255;
      pinCodeRunUntimed[1] = 255;
      setPins(0); // Turn everything off
      setPins(1);               
      response = F("All jobs stopped.");
    }

    // -------------Vent --------------------
    if(find_text("openVent",cleanParams) > -1){
      Serial.println(F("opening Vent"));
    pinCodeRunUntimed[1] &= 223;
    setPins(1);
      ventState = 1;
    response = F("Open vent set.");
    }
    if(find_text("closeVent",cleanParams) > -1){
    if(collectorPumpState){
      pinCodeRunUntimed[0] = 127;
      pinCodeRunUntimed[1] = 127;
    }
      Serial.println("closing Vent");
      setPins(0);
      setPins(1);
      ventState = 0;
    response = F("Close vent set.");
    } 
    // ----------------Collector pump ---------------------------
  if(find_text("startCollectorPump",cleanParams) > -1){
      Serial.println("start collector");
    pinCodeRunUntimed[0] &= 127;
    pinCodeRunUntimed[1] &= 127;
    setPins(0);
    setPins(1);
      collectorPumpState = 1;
    response = F("Collector started.");
    }
    if(find_text("stopCollectorPump",cleanParams) > -1){
    if(ventState){
      pinCodeRunUntimed[0] = 223;
      pinCodeRunUntimed[1] = 255;
    }
      Serial.println(F("stop collector"));
    setPins(0);
    setPins(1);
      //  setPinStateOld(1,127);
         // stopping = 1,grp A, 255
      collectorPumpState = 0;
    response = F("Collector stopped.");   
    }
    // Air Valve ----------------------   
    if(find_text("airValveOpen",cleanParams) > -1){   
      Serial.println(F("air Valve Open"));
      pinStateCurrent[0] = 223;
    pinStateCurrent[1] = 127;
    setPins(0);
    setPins(1);
       // stopping = 1,grp A, 255 
    response = F("Air valve opened.");    
    }
    if(find_text("airValveClose",cleanParams) > -1){    
      Serial.println(F("stop air Valve Close"));
      pinStateCurrent[0] = 255;
    pinStateCurrent[1] = 255;
    setPins(0);
    setPins(1);
        //Stopping does not use the pin value here
    response = F("Air valve closed.");    
    }
    // Air Pump ---------------------------------------
    if(find_text("airMotorOn",cleanParams) > -1){  
      Serial.println(F("stop collector"));
      pinStateCurrent[0] = 191;
    pinStateCurrent[1] = 127;
    setPins(0);
    setPins(1);
      //controller.Obj.setPin(0,0,191);  //  
      //controller.Obj.setPin(0,1,127);  // 
    response = F("Air motor on.");    
    }
    if(find_text("airMotorOff",cleanParams) > -1){ 
      Serial.println(F("stop collector"));
      pinStateCurrent[0] = 255;
    pinStateCurrent[1] = 255;
    setPins(0);
    setPins(1);
        //controller.Obj.setPin(1,0, 191);
      //controller.Obj.setPin(1,1, 127);
    response = F("Air motor off.");   
    }
    // No changes Required -----------------------------
    if(find_text("NothingRequired",cleanParams) > -1){
      Serial.println(F("NothingRequired"));
    response = F("Nothing was required.");    
    }
    return response;
}   
    
    

// -A000-B000-000Task=setpins-A000-B000-000...        = pinCodes
//01234567890123456789012345678901234567
// task=setPins-A000-B000-000Task=setpins-A000-B000-000 = jobsList


// Time has expired on job (or there are none in jobsList)
void doOneTimedJob(){  // Clean up after finished job. Then start a job from jobsList - if any.
//   Serial.println("Start of doOneJob");
//  int p0;
//  int p1;
  if(jobFinishTime != 0){   // A job was being done and has finished - so clean up first
    Serial.print("in doj jobFinishTime = ");
    Serial.println(jobFinishTime);
      jobFinishTime = 0;   // Because we got here when job was done.
    // Set pins back to previous state
//   Serial.println("in doj pineCodes p0 = "  + p0 );
//    Serial.println("in doj pineCodes p1 = "  + p1 );
    updatePinStates(1, 0, 000);  // sets pinStateCurrent[] and ..Running[]. Third param noty needed for stopping
    updatePinStates(1, 1, 000);  // sets pinStateCurrent[]
    setPins(0); //1= stopping,0= group,000-> turn off pins set on with pinCodeRunUntimed[]
    setPins(1);
  }
  // Now do any jobs waiting in jobsList
  //   jobsList starts with 'task=setPins-A000...   
  String pinCodes = jobsList.substring(12);  // next pinCodes starts with -A....
  if(pinCodes.length() > 13){   // a job to do
    if(find_text("task=setPins-",jobsList) != 0){  // Be sure it is not trash.
      return;
    }
    int p0 = pinCodes.substring(2,5).toInt();  // '-A127.....'
    int p1 = pinCodes.substring(7,10).toInt();
    updatePinStates(0, 0, p0);  // sets pinStateCurrent[]
    updatePinStates(0, 1, p1);  // sets pinStateCurrent[]
    setPins(0);  //group A
    setPins(1);  //group B
    pinCodeRunUntimed[0] = p0;    // code of this timed task 
    pinCodeRunUntimed[1] = p1;
//    Serial.println("in doj pineCodes p1 = "  + p1 );
    // This job is started. Now set its duration
    unsigned long currentMillis = millis();
    jobFinishTime = currentMillis + (pinCodes.substring(11,14).toInt() * 1000);
    // Job is started and recorded in pinCodeRunUntimed
    // Remove this job from jobs list and leave next one
    if(find_text("task=",pinCodes) == 14 && pinCodes.length() > 32){ //There is another job waiting
      Serial.println(F("in doOneTimedJob. Found another task."));
      jobsList = pinCodes.substring(14);   //Will start with 'task=setpins-A....'
    }
    else{   // There are no other jobs waiting. Remove this one.
      jobsList = "";
    }
  } 
} 
/*
void stopUntimedJob(int group, int startPinCode){
    if(ventState){
      pinCodeRunUntimed[group] = 
    }
    int negCode = ~startPinCode;// bitwise NOT. Removes a task from set of ON pins
  //  Serial.println(pinStateCurrent[group]);
    pinStateCurrent[group] ^= negCode;
    setPins(group);
//    Serial.println(pinStateCurrent[group]);
  }
  
  
}  */
    
String getTemperatures(){
      /*
      One wire multi sensor -----------------------
      5V+ -> 4.7K resister -> sensor wire 2
      Sensor wires 1 and 3 to grownd
      ALSO Arduino pin 2 to sensor wire 2
      */
      sensors.requestTemperatures(); // Send the command to get temperatures
      for(int i=0;i<5;i++){
          tempArray[i] = sensors.getTempFByIndex(i) * 10;
      }
      String resp;
      char buffer[20];
    for(int i=0;i<5;i++){
      resp += itoa(tempArray[i],buffer,10);
      if(i < 4){
        resp += '-';
      }
    }  
    return resp;
}
  



void sendGET() //client function to send/receive GET request data.
{
//  client.stop();
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > 15000){  // Do it every .
        previousMillis = currentMillis; 
        String temps = getTemperatures();
	  if (client.connect(serverName, 80)) {  //starts client connection, checks for connection
		Serial.println(F("in GET connected"));
		client.print(F("GET /arduino/php/greenhouse.php?task=arduinoCalling&callType=conditionsReport&temps="));
		client.print("1232-1234-1236-4448-5550");
		client.print("&ventState=");
		client.print(ventState);
		client.print("&collectorPumpState=");
		client.print(collectorPumpState);
		client.print("&panelsState=");
		client.print(panelsState);
		client.println(F(" HTTP/1.1"));
	
	 //   client.println("GET /arduino/html/test.html,us HTTP/1.1"); //download text
		client.println(F("Host: robertpattersonus.com"));
		client.println(F("Connection: close"));  //close 1.1 persistent connection  
		client.println(); //end of get request
	  } 
	  else {
		Serial.println(F("connection failed")); //error message if no client connect
		Serial.println();
	  }
 //   }
	
	  while(client.connected() && !client.available()) delay(1); //waits for data
	  while (client.connected() || client.available()) { //connected or data available
		char c = client.read(); //gets byte from ethernet buffer
		readString += c; //places captured byte in readString
	  }
	
	  //Serial.println();
	  client.stop(); //stop client
//	  Serial.println("client disconnected.");
//	  Serial.println("Data from server captured in readString:");
	  Serial.println(); 
//	  if(readString.length()> 2){
	  Serial.print(readString); //prints readString to serial monitor 
	  Serial.println(); 
//	  }
//	  /*
	  Serial.println();
//	  Serial.println(F("End of readString"));
	  Serial.println("=======e===========");
	  Serial.println();  
	  readString=""; //clear readString variable
    }
}



// ----------------- setPins  ---------------------------------

void setPins( int group) { // group 0 = A; B = 1.
  int pins = pinStateCurrent[group] & pinCodeRunUntimed[group];  //BITwise and
  Wire.beginTransmission(0x20);
  if (group == 0) {
    Wire.write(0x12); // GPIOA   Bank selection
  }
  else {
    Wire.write(0x13); // GPIOB
  }
  Wire.write(pins);
  Wire.endTransmission();
  Serial.print(group);
  Serial.print(F(" pins set: ("));
  Serial.print(pins);
  Serial.print(F(") "));
  Serial.println(pins,BIN);
  
}

// ---------------------- Find pos of a substring ----------------------------

// finds first instance of a substring        
int find_text(String needle, String haystack) {
  int foundpos = -1;
  for (int i = 0; (i < haystack.length() - needle.length()); i++) {
    if (haystack.substring(i,needle.length()+i) == needle) {
      foundpos = i;
    }
  }
  return foundpos;
}


///------------------- Manage Pin States ------------------

// -------- Called only by timed jobs ---------------
// Takes pinCode being started and combines it with the current vent and collector states.
// This allows starting and stopping things without changing the vent or collector states.

void updatePinStates(int startStop, int group,int code){
/*  Serial.print(F("----in ups ---startStop = "));
  Serial.print(startStop);
  Serial.print(F(" group = "));
  Serial.print(group); 
  Serial.print(F(" code = "));
  Serial.println(code);   */
  if(startStop == 0){  // 0 = start a job
      pinStateCurrent[group] = code;  
  }else{    //  1 = stop a timed job.
//    Serial.println("stopping something");
    pinStateCurrent[group] = 255;
  } 
}

