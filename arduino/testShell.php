
<?php
/*
 Robert patterson 11/30/2017
 
 Directly call arduino:     http://74.104.185.163/
 Access the verizon hub(?) : http://192.168.1.1/ user: admin pw ?? none or admin?
 
 
 */


echo "Start test shell";

//header("Access-Control-Allow-Origin: *");





echo " 111<br> ";
require_once('common/class/DatabaseConnection.inc.php');
require_once('model/graphics.inc.php');

require_once('model/greenhouse.inc.php'); 

echo " ---------<br> ";

require_once('controller/greenhouse.inc.php');
require_once('controller/logic.inc.php');
require_once('controller/graphics.inc.php');
require_once('controller/logic.inc.php');

define("URLANDTASK", "http://74.104.185.163/?task=");




GreenhouseClass::currentState();  // puts current data in array

echo " 211 <br>";

require_once('controller/main.inc.php');


echo " 222<br> ";


ini_set("date.timezone", "America/New_York");
$curr_timestamp = date("Y-m-d H:i:s");

echo " 333 <br>";

// GLOBALS

$logicDescription;


echo " 444 <br>";


$temperatures = processTemperature("315|499|531|530|570");
$a = managePanels($temperatures);

echo "<br> manage panels returned: $a<br>";




//currentState();

//-------------------------------------- TESTING ------------------------------------------------------

echo "In test shell at 555 <br>";



GraphicsClass::storeMeanTemp();


//GraphicsClass::getGraphData(1);   //storeMeanTemp();



//GraphicsLogicClass::makeChartData(1);

//echo "done?";

//GreenhouseClass::temperaturePut(100,100,50,50,60,1,1,0);


//---------------------------------------------------------------------------



 /*

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
			$type = $_GET['type'];
			//			makeGraphs($type);
			GraphicsLogicClass::storeMeanTemp(14472, 3);  // Updates: proceeses 15min mean temps
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

*/






?>


