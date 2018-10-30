//#include <Wire.h>

/*
To upload on Linux to test arduino in terminal do:
 sudo usermod -a -G dialout robert
 sudo chmod a+rw /dev/ttyACM0  <- zero



   char myChar = 'A';
  char myChar = 65;      // both are equivalent

TESTING:
Fake arduino call to server:
temp order: collec, out, is, pool, return
http://robertpattersonus.com/arduino/php/greenhouse.php?task=arduinoCalling&callType=conditionsReport&temps=901|501|743|516|987^0^0^9^0

------------------------------------------------------------------------------------------
          TASKS ON UPLOADING CODE
------------------------------------------------------------------------------------------



  If desired plug in reset wire.
----------------------------------------------------------------------------------------------
*/

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>
//#include <EthernetUdp.h>

//--------------------------- LOCAL AND REMOTE SERVER DIFFER -----------------------------

char serverName[] = "robertpattersonus.com"; // myIP server
//char serverName[] = "localhost"; // myIP server
int port = 80;

//------------------------------------------------------------------------------------



String introText = "Booting GreenhouseControls version 10/30/18";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address
EthernetClient client;
EthernetServer server = EthernetServer(8080);
char readMonitor[1];


// -------------- GLOBAL VARs AND OBJECTS -----------------------------------------------

#define RELAY_ON 0
#define RELAY_OFF 1


// State info
bool devicesPowerOn = true;
bool doEthernetReset = false;
int group_0_PinsSetTo = 0;
int group_1_PinsSetTo = 0;
//int collectorPumpState = 0;
//int panelsState = 99;  // == down/open
//int ventState = 0;   // closed
//bool autoControls = true;
int logEventCode = 1;  //  arduino was reset
bool doReset = false;
//Job Que
typedef struct {
  int jobNumber;
  int bankA;
  int bankB;
  unsigned long start;
  unsigned long end;
} jobType;
jobType jobArray[10];
int jobLogArray[15];

unsigned long tempArray[5] = {99999};
unsigned long timeToCheckTemp = millis();

// Pin # translation.  41 and 36 are right hand digits in binary number
//                     they are pin 1 on relay at bottom of relay in greenhouse
int bank0Pins[] = {41,39,48,46,44,42,40,38};  //Group A relays
int bank1Pins[] = {36,34,32,30,28,26,24,22};  // 10-22-18 it is 22;  26?? 12V power
String jobNames[31] = {     // 0-29 correspond to decimal value of characters sent by server
   "Do nothing    ",
     "Status        ",
     "Open vent     ",
     "Close vent    ",
     "Pump on       ",
     "Pump off      ", // <- first 5
     "Panels open   ",
     "Panels close  ",
     "Empty ar seals",   //8
     "D             ",
     "              ",   //<-10 = ':'
     "Close West    ",
     "Close centr   ",
     "Close East    ",
     "msg end mrkr  ",
     "Open center   ",  // <-15
     "Open East     ",
     "Auto off      ",
     "Auto on       ",
     "12V Off       ",
     "12V On        ",  // <-20 = D
     "Stop all      ",
     "Fill air seals",  // F
     "Unused G      ",
     "Open top      ",
     "unused I      ",  //<-25
     "Unused J      ",
     "Reset         ",
     "Open West     ",
     "Unused M      ",
     "Close top     "};


//--------------------------- ONE WIRE ------------------------------------------------

// Data wire is plugged into pin 2 on the Arduino  -----------------------
#define ONE_WIRE_BUS 2   // To DIGITAL pin 2! Braided wire w white line. 5V is othe braided and orang unbvraided is grnd
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

//------------------  TIMERS -------------------------------------

unsigned long calledHomeAt_Millis = millis() + 300000;        // this delays first call home for 5 min.
unsigned long jobsNextCheckTime = 0;
//int checkInterval = 0;
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


//MOVER UP EthernetServer server = EthernetServer(8080);


//-------------------- SET UP ----------------------------------------------------

void setup() {

//-------( Initialize Pins so relays are inactive at reset)----
  digitalWrite(38, RELAY_OFF);
  digitalWrite(40, RELAY_OFF);
  digitalWrite(42, RELAY_OFF);
  digitalWrite(44, RELAY_OFF);
  digitalWrite(46, RELAY_OFF);

  digitalWrite(48, RELAY_OFF);
  digitalWrite(39, RELAY_OFF);
  digitalWrite(41, RELAY_OFF);
  digitalWrite(36, RELAY_OFF);
  digitalWrite(34, RELAY_OFF);
  digitalWrite(32, RELAY_OFF);
  digitalWrite(30, RELAY_OFF);
  digitalWrite(28, RELAY_OFF);
  digitalWrite(26, RELAY_OFF);
  digitalWrite(24, RELAY_OFF);
  digitalWrite(22, RELAY_OFF);

  delay(1000);

  Serial.begin(9600);
  Serial.println(introText);

  pinMode(38, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(48, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(41, OUTPUT);
  // Group B
  pinMode(36, OUTPUT);
  pinMode(34, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(26, OUTPUT);  // 12V power switch
  pinMode(24, OUTPUT);
  pinMode(22, OUTPUT);  // 10-22-18 actually the 12V power
  // Other pins
  pinMode(7, OUTPUT);   // 7 connected to reset pine
  pinMode(2, INPUT);    // temps

  digitalWrite(7,HIGH);   // reset off
  sensors.begin();
  clearArray();
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Ethernet failed nothing will work."));
     while (true);
  }else{
    Serial.println(F("Ethernet ok"));
  }
  delay(1000);

}

/* ---------------------------- Main Loop ---------------------------------------
   ------------------------------------------------------------------------- */
bool oneTime = true;

void loop() {
//  Serial.println("xx");
    EthernetClient client = server.available();  // Client has connected, so see what he wants.
    if (client) {
      Serial.println();
      Serial.println("----------CALLER HAS CONNECTED----------");
      if(getInputString(client)){   // finds tasks and processes them
        sendResponse(client);
      }
      delay(1);
      client.stop();
      Serial.println("-----Inbound call ended. client.stopped------------");
    }
    // Check jobs array for jobs to do or stop
    if (millis() > jobsNextCheckTime) {
      setJobPins();   // Searches jobsArray and sets pins as needed.
    }
    callServerGetJobs();    // Calls every 5 minutes

    if(millis() > timeToCheckTemp){  // checks temps
      checkTemperatures();
    timeToCheckTemp = millis() + 30000;
    }

    while (Serial.available()) {
      delay(2);  //delay to allow byte to arrive in input buffer
      char cx = Serial.read();
 //     readMonitor[0] += c;
 //   }

      if (cx > 40) {
        int num = cx -48;
        cx = false;
        Serial.print("Monitor input ");
        Serial.println(num);
        putJobToCue(num);
      }
   }
}

//---------------------------------END of LOOP-----------------------------------------------


//------------------------------- HANDLE JOBS -----------------------------------------------

void putJobToCue(int jobN) {
  Serial.println(jobN);
  unsigned long time = millis();
  jobsNextCheckTime = 0;  // Force immediate checking of jobs array.

  switch (jobN){
    //---------------------------- Vent -------------------
    case 2: { // open Vent
        delFromArray(3);
        if(!inArray(jobN)){
          oneJob(2, 45, 0, time, 255,B01111111); //
        }
        break;
      }
    case 3: { // close Vent
        delFromArray(2);
        if(!inArray(jobN)){
          oneJob(3, 45, 0, time, 255, B00111111); //
        }
        break;
      }
    //--------------------------------COLLECTOR --------------------
    case 4: { // Turn collector Pump On.
      //  Renew job each time so it won't expire between arduino calls to server
      Serial.println("Will del #4 (coll. on)");
      delFromArray(4);
      Serial.println("Will start coll on");
      oneJob(4, 3600, 0, time, 127, 255);  // 20 minutes
        break;
      }
    case 5:  // Turn collector Pump Off
      delFromArray(4);
      break;
    //---------------------------- Ensemble Panel controls ----------------------

    case 6: {// opens all panels
      delFromArray(7);
        if(inArray(6)){   // 1 == open
          break;
        }
        // start = time to start next task; now+ duration of task + delay.
        unsigned long start = oneJob(6, 25, 0, time, 239, 225); // air release opens for 25 seconds.
                start = oneJob(6, 210, 15, start, 241, 249); // West panel 60 seconds after air release opened
                start = oneJob(6, 210, 15, start, 245, 253); // middle. end of prev job is start for next
                        oneJob(6, 210, 15, start, 246, 255); // East 254->255 10/14/18
        // -----------------oneJob(6, 210, 15, start, 255, 231);  // panel 4 top

        break;
      }
    case 7: { // Closes all panels
      delFromArray(6);
        if(inArray(7)){   // 0 = panel open
          break;
        }
        // start = time to start next task; now + duration of task + delay.
        // vars: jobN, jobLength, delay bwtn jobs,Start time for job, A pins, B pins,
        unsigned long start = oneJob(7, 250, 15, time, 255, 225); // west
        start =               oneJob(7, 260, 15, start, 255, 230); // east
        start =               oneJob(7, 275, 15, start, 255, 229); // middle End of prev job is start for next
        oneJob(7, 40, 0, start, 223, 255); // Open air fill valve for 40 seconds
        oneJob(7, 45, 0, start, 191, 255); // Simultaneously run air motor for 50 seconds
        // ------------soneJob(6, 210, 15, start, 247, 239);  // panel 4

        break;
      }
    // -------- Below for testing. Do specific jobs on user command ----------------------

    // -----------------Air Seal System ---------------------------

    case 22: { // Fill air seals = 'F'
        oneJob(22, 40, 0, time, 223, 255); //Open air fill valve for 40 seconds
        oneJob(22, 45, 0, time, 191, 255); // Simultaneously run air motor for 50 seconds
        break;
      }
    case 8:  // Empty air seals
      oneJob(8, 25, 0, time, 239, 255); // air release opens for 25 seconds.
      break;
    case 24: // Open top panel = H
      oneJob(12, 275, 15, time, 255, B11100111);
      break;
    case 30:  // Close top panel = N
      oneJob(12, 275, 15, time, B11101111, B11110111);
      break;

    case 10:
      break;
    // --------------------- Panels ----------------------------
    case 11:  //panel close 1 West  == ;
       clearArray();
       oneJob(11, 250, 15, time, 255, 225);  //225
      break;
    case 12:  //close panel 2 Center == <
      clearArray();
      oneJob(12, 275, 15, time, 255, 229);
      break;
    case 13:  //close panel 3 East == =
      clearArray();
      oneJob(13, 260, 15, time, 255,B11100110);  // "="
      break;
    case 25:  //close panel 4 (top) == J
      clearArray();
      oneJob(25, 210, 15, time, 247, 239);
      break;
    case 28:  //open panel 1 West == L
      clearArray();
      oneJob(13, 210, 15, time, B11110001, B11111001);
      break;
    case 15:  //open panel 2 center == ?
      clearArray();
      oneJob(15, 210, 15, time, 245, 253);
      break;
    case 16:  //open panel 3 East == @
      clearArray();
      oneJob(16, 210, 15, time, 246, 255);
      break;
    case 26:  //open panel 4 (top) =
      clearArray();
      oneJob(26, 210, 15, time, 255, 231);
      break;
    // ---------------- Misc controls -----------------------------------
    case 17:  //stop automatic controls == A
      //autoControls = false;
      break;
    case 18:  //start automatic controls == B
      //autoControls = true;
      break;
    case 19:  //Turn off 12v power - to devices. ='C'; done by setControlStatusInfo() later
      break;
    case 20:  //Turn on 12v power - to devices. ='D'; done by setControlStatusInfo() later

      break;
    case 21:  //stop all jobs immediately = E
      clearArray();
      break;
    case 27:  // Do reset  = K.
      doReset = true;
      break;
   default:
      break;
  }
  // Last instruction from server is to turn on or off 12V power
}

// A description of the job: jobNunber,length,start time, pins to turn on,

unsigned long oneJob(int jobN, int length, int delay, unsigned long start, int bankA, int bankB) {
  unsigned long end = start + (length * 1000L);  // Now in milliseconds
  jobType jobCue = {jobN, bankA, bankB, start, end}; // E
  jobArray[getArrayIndex()] = jobCue; // Schedules job
  if(jobN > 1){
    jobLogArray[getJobLogArrayIndex()] = jobN;    // Puts job on the que/array
  }
  return end +  (delay *  1000L);  // Is start for next job

}



// Looks at each item in jobArray[] to see what to turn on or off based on time.
void setJobPins() {
  unsigned long now = millis();
  int onBitsA = B11111111;
  int onBitsB = B11111111;   //Turn off real pin 22 (12V power to relays.
//  delay(5000);
//  Serial.print( " delayed 0.5 seconds");
  for (int i = 0; i < 10; i++) { //Go through the jobArray
    // Include only if its not job 0 (unused line); after start and before end.
    if (jobArray[i].start < now && jobArray[i].end > now && jobArray[i].jobNumber != 0) { //It's within the time period that it should run
    //  Serial.print("Starting new job: ");
    //  Serial.println(jobArray[i].jobNumber);
    //  Serial.print( " at array index: ");
    //  Serial.println(i);
      onBitsA &= jobArray[i].bankA;   // Anding i'th bit in each currently running job in array to get final state for that bit
      onBitsB &= jobArray[i].bankB;
    }else{
    if (jobArray[i].jobNumber != 0 && jobArray[i].end < now) { //It's after the end time so mark unused
      Serial.print("deleting job#: ");
      Serial.println(jobArray[i].jobNumber);
      Serial.print( " at array index: ");
      Serial.println(i);
      jobArray[i].jobNumber = 0;
    }
    }
  }
  if(group_0_PinsSetTo != onBitsA || group_1_PinsSetTo != onBitsB){ // only set pins if changes
  digitalWrite(26,1);     // 1 turns 12V power off while relays are set.
  delay(100);
    setPins(0, onBitsA);
    setPins(1, onBitsB);
    digitalWrite(26, 0);  //Turn on real pin 26 - 12V power to devices run by relays.
    Serial.print("Sent pin settings: ");
    printArrayRow(onBitsA);
    printArrayRow(onBitsB);
    Serial.println();
    showArray(jobsNextCheckTime);
  }
  jobsNextCheckTime = now + 5000;    //  check every 5 seconds
}

//------------------------------ TEMPERATURE --------------------------------------

void checkTemperatures(){
  int temps[5];
    sensors.requestTemperatures(); // Send the command to get temperatures
    for(int i=0;i<5;i++){
      temps[i] = sensors.getTempFByIndex(i) * 10;
      int t = (int)temps[i];
      if(t < 1500 && t > -100){
        tempArray[i] = t;     // Save to new temp if good
        }
    }
}

String getTemperatures(){
  String resp;
  for(int i=0;i<5;i++){
  char buffer[20];
    resp += itoa(tempArray[i],buffer,10);
      if(i < 4){
       resp += '|';
      }
  }
  Serial.print("-------Temp string =");
  Serial.println(resp);
  return resp;
}

//------------------------------ CALL SERVER GET JOBS ---------------------------------------

// Calls on schedule. Sends status data. Uses response to do jobs if autoControls = true.
void callServerGetJobs() { //client function to send/receive GET request data.
  unsigned long currentMillis = millis();
  if (calledHomeAt_Millis < currentMillis) { // Do it every 5 minutes.
    Serial.println();
    Serial.println(F("----------------- CALL SERVER GET JOBS ---------------------"));
    String temps = getTemperatures();  // 10/28/18 No!->if bad temps it sets 'doReset'
    if(doReset){  // Last time server said reboot and getTemperatures() (above) confirmed "doReset".
      digitalWrite(7,LOW);   // reset the arduino
      Serial.println("Can't reset: is 7 disconnected?");
        // NOTE: If making 7 low does not cause reset (wire disconnected?) the code continues thru this function
        // AND then we need to make doReset false;
      doReset = false;
    }
    if(doEthernetReset == true){
      Serial.println("Doing ethernet reset.");
      digitalWrite(7,LOW);   // reset the arduino
      Serial.println("Can't reset: is 7 disconnected?");
      doEthernetReset = false;  // needed if reset wire is unconnected
    }
    calledHomeAt_Millis = currentMillis + 300000;   // 5 minutes
    if (client.connect(serverName, port)) {  //starts client connection, checks for connection
      client.print(F("GET /arduino/index.php?task=arduinoCalling&callType=conditionsReport&temps="));
      client.print(temps);
      client.println(F(" HTTP/1.1"));
      client.println(F("Host: robertpattersonus.com"));
      client.println(F("Connection: close"));  //close 1.1 persistent connection
      client.println(); //end of get request
    }else{
      Serial.println(F("------- Connection failed ----------------"));
      doEthernetReset = true;
      return;
    }
    int count = 0;
    while (client.connected() && !client.available())  delay(1); //waits for data -Forum says this may cause hanging
    char c;
    while (client.connected()) {
      if (client.available()){
        c = client.read();
Serial.print(c);
        if (c == '?'){
          readChars(client,3);
          if('k' == client.read() && '=' == client.read()) { // have gotten "task="
                    // Below reads jobs string which may be empty or up to several jobs
            for(int n=0;n<7;n++){   // 7= max number of chars expectable
              c = client.read();  // should be a number ^ or >
              serialJobPrint(c);
              if(c == '^'){   // Spacer - move next char
                c = client.read();
                serialJobPrint(c);
              }
              Serial.println();
              if(c == '>'){
                setControlStatusInfo(client);
                Serial.println("---End of message reached (> + 12V power on/off)");
                client.stop();
                return;   // - signals end of job list.
              }
              if((int)c <48 || (int)c > 77){        // its not a job number or ^ or > - quit.
                client.stop();
                return;
              }
              putJobToCue((int)c - 48);  // convert ASCII to int. put job in cue
                       }  // completed reading up to 7 chars of job info and control status info
            client.stop();
            return;
                 }  // end if
              }  // end of found ? mark processing
          }   // char was not "?" so keep reading
          count++;
          if(count > 500){
            client.stop();
            Serial.println("---count over 500. ending--- ");
            return;
          }
    }  // end of connected loop
      client.stop();
      Serial.println("-----No usable message found--- ----------");
    }   // It was not time to call yet
}

//----------------------------------------------------------------------------------------------

// ------------------------ USER CALLING - SEE WHAT TO DO ----------------------------

bool getInputString(EthernetClient client) {
  bool foundaTask = true;
  char c;
  while (client.connected()) { // true = client has responded or come calling
     if (client.available()) {
        readChars(client,5);
      if (client.read() == '?'){
        readChars(client,3);   // step to job # after 'task'. The < is = to length of skip
        if('k' == client.read() && '=' == client.read()) { // we're ready for job numbers
          for(int n=0;n<7;n++){   //
            c = client.read();
            serialJobPrint(c);
            foundaTask = true;
            if(c == '>'){   // Device state info follows job info
              setControlStatusInfo(client);
                return foundaTask;   // - signals end of job list.
            }
            putJobToCue((int)c - 48);  // convert ASCII to int. put job in cue
            return foundaTask;
          }
          return foundaTask;
          }  // Found parameters and processed them
          Serial.println("Did not see GET? at start of text");
          return false;
           }   // Found client available
      }    // ends client connected
      Serial.println("Client not connected. No task found. #2 return false");
      return foundaTask;
  }
}


void setControlStatusInfo(EthernetClient client){  // Runs after pins have been set
  if(client.read() == '1'){ // 1 means On to server
    devicesPowerOn = true;
    digitalWrite(26,0);
    Serial.println("12V On");
  }else{
    devicesPowerOn = false;
    digitalWrite(26,1);
    Serial.println("12V Off");
  }
}



//---------------- Set Pins -----------------------------------
// Fancy anding has already been done in setJobPins().
// 'pins' is the settings the pins should have now. setPins() makes changes if required.

void setPins( int group, int pins) { // group 0 = A; B = 1.
  int truePin;
  int setTo;
  int stateNow;
  for (int n = 0; n < 8; ++n) {
    //
    setTo = (pins >> n) & 1;    // n=0..15.  stores nth bit of pins in setTo.  setTo becomes 0 or 1.
    if (group == 0) {
      truePin  = bank0Pins[n];  // translate bit postion to actual Arduino output pin number
    }else {
      truePin = bank1Pins[n];
    }
    stateNow = digitalRead(truePin);  // Read the current pin state
    if (setTo != stateNow) {
      digitalWrite(truePin, setTo);
    }
  }
  if(group == 0){
    group_0_PinsSetTo = pins;
  }else{
    group_1_PinsSetTo = pins;
  }
}



// ---------------------SEND RESPONES ------------------------------------------

void sendResponse(EthernetClient client ) {
  String temps = getTemperatures();
  client.println(temps);
  client.print("^");
//  client.print(getCurrentState());    // = vent^collecotr^panel
//  client.print("^");
  client.println(logEventCode);
  logEventCode = 0;
}


//--------------------------------- Array operations --------------------------------

bool inArray(int jobN){
    for (int i = 0; i < 10; i++) {
      if (jobArray[i].jobNumber == jobN) {
        return true;
      }
     return false;
    }
}

void clearArray() { // Marks all jobs in array "jobNumber 0(no job to do) and calls for immediate check for jobs to do ("end" = 0).
  for (int c = 0; c < 10; c++) {
    jobArray[c].jobNumber = 0;
    jobArray[c].bankA = 0;
    jobArray[c].bankB = 0;
    jobArray[c].start = 0;
    jobArray[c].end = 0;
  }
  digitalWrite(26,1);   // Turn of power to all devices
  Serial.println(F("Array cleared."));
}


int getArrayIndex() { // Returns first zeroed element it finds
  for (int i = 0; i < 7; i++) {
    if (jobArray[i].jobNumber == 0) {
      return i;
    }
  }
  return 0;
}

int getJobLogArrayIndex() { // Returns first zeroed element it finds
  for (int i = 0; i <15; i++) {
    if (jobLogArray[i] == 0) {
      return i;
    }
  }
  return 0;
}
void delFromArray(int n) {  // Does not remove; just sets value = 0
  for (int i = 0; i < 10; i++) {        // Removes all n's in array
    if (jobArray[i].jobNumber == n) {
      jobArray[i].jobNumber = 0;
    }
  }
}

//----------------------PRINTING ONLY Show Array -----------------------------

void showArray(unsigned long jobsNextCheckTime ) {
  unsigned long time = millis();
  Serial.print(F("\n------------ Job Array at "));
  unsigned long t = time/1000;
  Serial.print(t);
  Serial.print("----Next check at  ");
  unsigned long jn = jobsNextCheckTime/1000;
  Serial.print(jn);
  Serial.print("----------------\n");
  Serial.print("Right hand digit is bottom relay in greenhouse\n");
  Serial.println(F("Jb\t\tbankA ------\tbankB -------\tStart \tStop\t"));

  for (int c = 0; c < 10; c++) {
    int n = jobArray[c].jobNumber;
    String jobName = jobNames[n];
    if (n != 0) {   // Skip deleted jobs
      Serial.print(jobName);
      Serial.print(F("\t"));
      n = jobArray[c].bankA;
      printArrayRow(n);
      n = jobArray[c].bankB;
      Serial.print(F(""));
      printArrayRow(n);
      unsigned long t = jobArray[c].start/1000;
      Serial.print(t);
      Serial.print("\t");
      t = jobArray[c].end/1000;
      Serial.print(t);
      Serial.print("\t\t");
      Serial.println();
    }
  }
}


void printArrayRow(int n){
  Serial.print(F("("));
  Serial.print(n);
  Serial.print(F(")"));
  if(n < 128){
    Serial.print("0");
  }
  if(n < 64){
    Serial.print("0");
  }
  Serial.print(n, BIN);
  Serial.print(F("\t"));
}

void readChars(EthernetClient client,int n){
    char c;
    for (int z = 0; z < n; z++) { // step to job # after 'task'. The < is = to length of skip
      c = client.read();
    }
}

void serialJobPrint(char c){
  if(c != '^' && c != '>' && c != '0'){
     Serial.print("---Got job: (");
     Serial.print(c);
     Serial.print(") = ");
     int i = c - '0';
     String txt = jobNames[i];
     Serial.print(txt);
  }
}



/*
// ------------------------------------------------------------

FROM http://weblog.jos.ph/development/arduino-reset-hack/


//digitalPin 7 is connected to the RESET pin on Arduino
//NOTE: you CANNOT program the board while they are connected
//by default digitalPin 13 will blink upon reset, so stick an LED in there

int interval = 5000;
long int time = 0;

void setup(){
  digitalWrite(7, HIGH); //We need to set it HIGH immediately on boot
  pinMode(7,OUTPUT);     //We can declare it an output ONLY AFTER it's HIGH
                         // (( HACKHACKHACKHACK ))
  Serial.begin(9600);    //So you can watch the time printed
}


void loop(){

  time = millis();

  Serial.println(time);

  if(time > interval){
    Serial.println("RESET!");
    digitalWrite(7, LOW); //Pulling the RESET pin LOW triggers the reset.
  }
}

*/
