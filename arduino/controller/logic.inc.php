

<?php

/*
 v 3/14/18
 Greenhouse controls

 r patterson 3-1-16
 */


$settings = [
	'ventTempOpen' => 71,
    'ventTempClose' => 66,
    "collectorTempDeltaOn" => 10,		//pool vs collector
    "returnTempDeltaOff" => 5,   // differemce between pool and return pipe
    "panelCanOpenTime" => '070000',		// 24 h clock 
    "panelMustCloseTime" => '173000',
    "panelTempDeltaOpen" => 10,
    "panelTempDeltaClose" => 2,
	"maxPanelCloseTemp" => 55,
	"minOpenTime" => 16.00		// (xx.xx) minutes.seconds
];

sunTimes();

$states = [
	'ventOpen' => 0,
	'collectorPumpOn' => 0,
	'panelsClosed' => 0,
	'devicespower' => 0,
	'autocontrol' => 0
];

GreenhouseClass::currentState(); 
	
		
/*
   LOGIC may call for a change or it may not call for any change - in which case it depends on the
   currentState info from the database. If database data is wrong a later call hopefully will
    send a signal for change.
   
    ---------------------- VENT -----------------------------------------------------   */

// ventState 0 = closed; 1= open
function manageVent($temperatures){
	global $settings;
	global $states;
	global $logicDescription;
	$ist= $temperatures["isT"];
	$ost= $temperatures["osT"];
	$response .= "";
	$initVentState = "3";   // closed
	if($states['ventOpen'] == 1){  // closed
		$initVentState = "2";
	}
	//-------------------------OPEN   VENT ------------------------
	
	if(!$states['ventOpen'] && $ist > $settings["ventTempOpen"]){
		$states['ventOpen'] = 1;
		$logicDescription .= "Open vent. $ist &gt; " .$settings["ventTempOpen"] . "<br>";
		return "2";		
	}
	//------------------ CLOSE  VENT -------------------------
	// currenely open
	if($states['ventOpen'] && $ist < $settings["ventTempClose"]){
		$states['ventOpen'] = 0;
		$logicDescription .= "Close vent. $ist &lt; " . $settings["ventTempClose"] . "<br>";
		return "3";
	}
	
	if($states['ventOpen'] && $ist < $settings["ventTempOpen"] && $ost > 55){
		$states['ventOpen'] = 0;
		$logicDescription .= "Close vent mild weather<br>";
		return "3";
	}
	
	
	return "0";   // Does nothing
	}

/* /------------- ------------------------COLLECTORS -------------------------

Arduino runs motor for some minutes after 'on' command so the command needs to be sent repeatedly

*/
	
function manageCollector($temperatures){
	global $settings;
	global $states;
	global $logicDescription;
	$pt= $temperatures["pT"];  // if current temp bad it may use average value
	$ct= GreenhouseClass::averageTemp("cT",2);
	$rt= $temperatures["rT"];
	$response .= "";   // maybe do nothing
	$initCollState = "5";   // off
	if($states['collectorPumpOn'] == 1){  
		$initCollState = "4";
	}
		
	// ------------------ Always send some signal--------------------------
	
	//Send PUMP ON signal
	if($ct > ($pt + $settings['collectorTempDeltaOn'])){
		if($states['collectorPumpOn'] != 1){
			$logicDescription .= "Col. on.  $ct &gt; $pt + " .  $settings['collectorTempDeltaOn'] . "<br>";
		}
		$states['collectorPumpOn'] = 1;
		return "4";
	}
		//- SEND PUMP OFF SIGNAL -----------------------
		// Pump is on
	if($rt < ($pt + $settings['returnTempDeltaOff'])){
		if($states['collectorPumpOn'] != 0){
				$logicDescription .= "Col. off. $rt &lt; $pt + " .  $settings['returnTempDeltaOff'] . "<br>";
		}
		$states['collectorPumpOn'] = 0;
		return "5";
	}
	return  $initCollState;   // No change was called for. But send bc arduino will turn off q 60 minutes 
}

//--------------------------------- PANELS	--------------------------------------------

function managePanels($temperatures){
	global $settings;
	global $states;
	global $logicDescription;
	$ist= $temperatures["isT"];
	$ct= GreenhouseClass::averageTemp("cT",3);
	$ost= $temperatures["osT"];
	$now = date('His');  // format is '083000,= 8:30am
	$initPanelState = "6"; // closed
	if($states['panelsClosed'] == 1){
		$initPanelState = "7";
	}
	
	/* /--------------------- OPEN IT ------------------------  
		Requirements:
		It's open
		Its after panel can open time
		collector is hotter than outside + openFactor -- There is radiant heating
		collector is hotter than inside -- It not so cold that open panels can cool greenhouse
	
	*/
	if($states['panelsClosed'] && $now > $settings["panelCanOpenTime"] && 
				$now < $settings["panelMustCloseTime"] && 
				$ct > ($ist + $settings["panelTempDeltaOpen"])){  		
		$logicDescription .= "Open panels: $ct &gt; ($ist + " . $settings["panelTempDeltaOpen"] . ")";		
		$states['panelsClosed'] = 0;
		GreenhouseClass::setOpenTime();
		return "6";
	}   
	//---------------- CLOSE IT  --------------------------------
	
	// Never close if outside temp is > maxPanelCloseTemp
	// AND either its after closing time or
	// The collector is cooler than the outside by X degrees == little radiation coming in

	$istM= GreenhouseClass::averageTemp("isT",6);
	$ctM= GreenhouseClass::averageTemp("cT",5);
	
	if(!$states['panelsClosed'] 
			&& $ost < $settings["maxPanelCloseTemp"] 
			&& GreenhouseClass::getOpenSufficientTime($settings["minOpenTime"]) 
			&& $ctM < ($istM + $settings["panelTempDeltaClose"])
	   ){			
		$logicDescription .= "Close panels: $ctM < $istM + " . $settings["panelTempDeltaClose"];
		$states['panelsClosed'] = 1;
		return "7";
	} 
	return "0";   // Does nothing
}


/* ----------------------------- HELPER FUNCTIONS ----------------------------------------*/


function badTemps($temperatures){		// current temp sent in by arduino
	global $logicDescription;
	$ist= intval($temperatures["isT"]);
	if($ist > 150 || $ist < 10){
		$logicDescription .= " Inside $ist ";
		return true;
	}
	$ct= intval($temperatures["ct"]);  // collector Temperature
	if($ct > 300 || $ct < 0){
		$logicDescription .= " Collector $cT ";
		return true;  // Reset arduino bc temps bad
	}
	$pt= intval($temperatures["pT"]);  // pool Temperature
	if($pt > 110 || $pt < 33){
		if(GreenhouseClass::averageTemp("pT",2) > 0){ // use from database if possible
			$logicDescription .= " Pool $pt. Using average";
			return true;
		}
	}
	$rt= intval($temperatures["rT"]);  // return Temperature
	if($rt > 200 || $rt < 10){
		$logicDescription .= " Return $rt ";
		return true;
	}
	$ost= intval($temperatures["osT"]);
	if($ost > 160 || $ost < -10){
		$logicDescription .= " Ouitside $ost ";
		return true;
	}
	return false;
}

function sunTimes(){
	global $settings;
	$output = file_get_contents('http://api.sunrise-sunset.org/json?lat=42.40&lng=-72.22');
	$ar = json_decode($output, true);
	$params = $ar['results'];
	$sunrise = $params['sunrise'];
	$riseArray = explode(":",$sunrise);
	$hr = intval ($riseArray[0]) - 5;		// GMT is +5 hours over Boston
	$settings["panelCanOpenTime"] =  "0" . $hr . $riseArray[1] . "00";
	$sunset = $params['sunset'];
	$setArray = explode(":",$sunset);
	$hr = intval ($setArray[0]) + 6;		// -6 +12 for 24h clock
	$settings["panelMustCloseTime"] = "0" . $hr . $setArray[1] . "00";
}

?>
