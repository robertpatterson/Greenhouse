

<?php

/*
 Working addresses
 http://robertpattersonus.com/arduino/php/greenhouse.php?task=sendPage
 http://96.237.233.147:8080/?task=setPins_A=247_B=225
 
FOR TESTING AUTOMATIC CONTROLS LOGIC
vent 1= closed
panelstate 0 = open
http://robertpattersonus.com/arduino/php/greenhouse.php?task=arduinoCalling&callType=conditionsReport&temps=901|501|743|516|987^0^0^9^0

http://robertpattersonus.com/arduino/php/greenhouse.php?task=arduinoCalling&callType=conditionsReport&temps=901|501|743|516|987^0^0^9^0 

*/


if(isset($_GET['task'])){
	$task = $_GET['task'];
	switch ($task){
		case "arduinoCalling":
			arduinoCalling();
			break;
		case "userCalling":
			userCalling();
			break;
		case "makeGraphs":
			$day_1 = $_GET['day_1'];
			$day_2 = $_GET['day_2'];
			GraphicsLogicClass::makeChartData($day_1,$day_2);
			break;  
		case "password":
			password();
			break;
		default:
			echo sendPage();
		}
	}
	else{
		echo sendPage();
} 


//---------------------------------------USER CALLING ---------------------------------------------
// Arduino response:  327|306|531|597|495^0^1^0

function userCalling(){   // from the web page. Pass along the job requested. Get back current temps
	global $settings;
	global $states;		// Array already gotten from database
	$jobNames = array(
	        "Do nothing    ",
			"Status request",
			"Open vent     ",
			"Close vent    ",
			"Colector on   ",
			"Collector off ",
			"All panels opn",
			"All pnels clse",
			"Empty air seals",
			"              ",
			"              ",
			"Close West    ",
			"Close Center  ",
			"Close East    ",
			"msg end mrkr  ",
			"Open center   ",
			"Open East     ",
			"Auto off      ",
			"Auto on       ",
			"12V on        ",
			"12V off       ",
			"Stop all      ",
			"Fill air seals",
			"              ",
			"Open top      ",
			"              ",
			"              ",
			"Reset         ",
			"Open West     ",
			"Unused M      ",
			"Close top     ");
	

	$job = $_GET['job'];  // it is one char
	updatestates($job);	// local function.  If power off or on will send job# and requested 12V power state
	$request_url = URLANDTASK . $job . ">" . $states['devicespower'];
	$response = get_url($request_url);
	//ORDER: temps, 1panelsState, 2ventState, 3collectorPumpState, 4logEventCode, 5autoControls, 6jobLog
	$respArray = explode("^",$response);	
	$eventLogCode = $respArray[1]; 				// only reset at this time  4
	$eventLogTxt = "No";
	if($eventLogCode == 1){
		$eventLogTxt = "Yes";
		GreenhouseClass::eventLog($eventLogTxt);
	}
	$now = date('l h:i a');
	$autoTxt = "On";
	if($states['autocontrol'] == 0){
		$autoTxt = "Off";
	}	
	$volts = $states['devicespower'];
	$voltsText = "On";
	if($volts == 0){
		$voltsText = "Off"; 
	}  
	$panelMovement = "xx";
	if($job != '1'){
		$j = ord($job) - 48;
		$jobAsName = $jobNames[$j];
		GreenhouseClass::jobLogPut("User",$jobAsName);
	}
	echo "Conditions on $now";
	echo "^";
	$tempsArray = explode("|",$respArray[0]);
	$index = 0;
	foreach($tempsArray as $value){
		$num = number_format(($value/10),1);		
		if($index == 3){
			echo "$num<br>(x&#772;" . GreenhouseClass::averageTemp("cT",2) . ")^";
		}elseif($index == 2){
			echo "$num<br>(x&#772;" . GreenhouseClass::averageTemp("pT",2) . ")^";
		}else{
			echo "$num^";
		}
		$index++;			
	}  // GOING TO WEB PAGE	
	echo $states['panelsClosed']  . "^" . $states['ventOpen'] . "^" . $states['collectorPumpOn'] .
	"^" . $autoTxt . "^" . $settings['ventTempOpen'] .
	"^" . $settings['ventTempClose'] . "^" . $settings['collectorTempDeltaOn' ] . "^" .
	$settings['returnTempDeltaOff'] .
	"^" . $settings['panelCanOpenTime'] . "^" . $settings['panelMustCloseTime'] . "^" .
	$settings['panelTempDeltaOpen'] . "^" . $settings['panelTempDeltaClose'] .
	"^" . $voltsText . "^" . $settings['maxPanelCloseTemp'] . "^" .  
	$panelMovement . "^" . $eventLogTxt . "^" . GreenhouseClass::jobLogGet();
	
}


function stopJobs(){
	$request_url = URLANDTASK . "stopJobs\n";
	$response = get_url($request_url);  //,$params);
	echo "<br>Stop Jobs sent.<br>Response = $response";	
}



//-----------------------------------ARDUINO CALLING --------------------------------------

//----------------------------------------------------------------------------------------

function arduinoCalling(){		// He calls periodically to get orders
	global $states;
	global $logicDescription;
	if(isset($_GET['callType'])){
		//$callType = $_GET['callType'];
		$temps = $_GET['temps'];
	}
	$temperatures = processTemperature($temps);
	$entryCount = GreenhouseClass::temperaturePut(
			$temperatures["osT"],
			$temperatures["isT"],
			$temperatures["cT"],
			$temperatures["pT"],
			$temperatures["rT"],
			$states['collectorPumpOn'],  // These may have been changed by the control logic module
			$states['panelsClosed'],
			$states['ventOpen']);
	if($entryCount == 3){
		GraphicsClass::storeMeanTemp();
		GreenhouseClass::restarttempcount(); 
	}	
	$resp = "?task=";
	if(badTemps($temperatures) || $states['reset']){
		$logicDescription = "Sent reset" . $logicDescription;
		$resp .= 'K';		 // Will it reboot if we repeatedly use mean pT????????????????????
		$states['reset'] = 0;
		}elseif($states['autocontrol']){  // auto control 1=on = send jobs. Send info on each type of device		
			$logicDescription = "";
			$resp .= manageVent($temperatures);		
			$resp .= "^" . manageCollector($temperatures);
			$resp .= "^" . managePanels($temperatures);
		}else{
			$logicDescription .= " AUTO OFF";
		}  
	echo $resp . ">" . $states['devicespower'];  // Produces "?task=2^4>X" (X=devicepower) or if no jobs to do "?task=>X"
	if($logicDescription != ""){
		GreenhouseClass::jobLogPut("Logic",$logicDescription);   // Save the 3 work functions texts.
	}
	GreenhouseClass::updatestates(
			$states['collectorPumpOn'],
			$states['panelsClosed'],
			$states['ventOpen'],
			$states['devicespower'],
			$states['autocontrol'],
			$states['reset']);

}

function processTemperature($temps){
	$temperatures = [];
	$tempsArray = [];
	$tempsArray = explode("|",$temps);  // Temps are degrees to one decimal place but no period
	$labels = array("osT","rT","pT","cT","isT","UnKn");
	for($i=0;$i<count($tempsArray);$i++){
		$fTemp = (float) $tempsArray[$i];
			$fTemp = $fTemp/10;		
		$temperatures[$labels[$i]] = $fTemp;
	}
	return $temperatures;	
}

function password(){
	if(isset($_GET['pw'])){
		$pw = $_GET['pw'];
		if($pw == PW){
			echo "result = ok";
		}else{
			echo "0";
		}
	}	
}

//----------------------------------- -----------------------------------
function get_url($request_url) {     
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $request_url);
	curl_setopt($ch, CURLOPT_VERBOSE,1);
	curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 1000);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);  //forces bytes from other server to be the return of curl_exec()
	$response = curl_exec($ch);  // true or false unless CURLOPT_RETURNTRANSFER causes it to be the bytes from other server
	curl_close($ch);
	return $response;
}


function updateStates($job){
	global $states;
	switch ($job){
		case 'C':
			$states['devicespower'] = 0;  // off
			break;
		case 'D':
			$states['devicespower'] = 1;	// 0n
			break;
		case 'A':
			$states['autocontrol'] = 0;
			break;
		case 'B':
			$states['autocontrol'] = 1;
			break;
		case 'K':
			$states['reset'] = 1;
			break;
		case "7":
			$states['panelsClosed'] = 1;
			break;
		case "6":
			$states['panelsClosed'] = 0;
			break;
		case "<":
			$states['panelsClosed'] = 1;
			break;
		case "?":
			$states['panelsClosed'] = 0;
			break;
		case "4":
			$states['collectorPumpOn'] = 1;
			break;
		case "5":
			$states['collectorPumpOn'] = 0;
			break;
		case "2":
			$states['ventOpen'] = 1;
			break;
		case "3":
			$states['ventOpen'] = 0;
			break;
		case "4":
			$states['collectorPumpOn'] = 1;
			break;
		case "5":
			$states['collectorPumpOn'] = 0;
			break;			
		default:
			break;
	}
	GreenhouseClass::updatestates(
			$states['collectorPumpOn'],
			$states['panelsClosed'],
			$states['ventOpen'],
			$states['devicespower'],
			$states['autocontrol'],
			$states['reset']);
	GreenhouseClass::currentState();		// It may has been changed
	
}

	


/* / All state info from Arduino is reliable except if panels = 0 it may because 
// job was completed earlier.
function setNewStates($job,$vent, $collector, $panels){
	global $states;
	switch($job){		// updates to what user is requesting
		case 'C':
			$states['devicespower'] = 0;
			break;
		case 'D':
			$states['devicespower'] = 1;
			break;
		case 'A':
			$states['autocontrol'] = 0;
			break;
		case 'B':
			$states['autocontrol'] = 1;
			break;
	}
	if($panels == "9"){
		$panels = $states['panelsClosed'];  // Arduino does not know actual state if = 9.
	}
	$states['ventOpen'] = $vent;
	$states['collectorPumpOn'] = $collector;
	GreenhouseClass::updatestates($collector,
			$panels, 
			$vent ,
			$states['devicespower'],
			$states['autocontrol']);
}
*/


		
?>