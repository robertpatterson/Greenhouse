<?php 
/*
require_once('../model/graphics.inc.php');
require_once('../model/greenhouse.inc.php');

GraphicsLogicClass::makeChartData(1,0);
*/


//--------------------------------------END TEST CODE ------------------------------------

class GraphicsLogicClass{

	public static function makeChartData($day_1,$day_2){
		$ar = GraphicsClass::getGraphData($day_1,$day_2);
//		$count mysqli_num_rows($resp);
//		$maxCount = getMaxCount($result);
//		$pointRangesAr = calcPointSizes($maxCount);  // calc the size of the point for the 3 different point sizes used
		echo '{"cols":[
		{"id":"col1","label":"Time","type":"datetime"},		
		{"id":"col2","label":"Outside Temperature","type":"number"},
		{"id":"col1","label":"Inside Temperature","type":"number"},
		{"id":"col3","label":"Collector Temperature","type":"number"},
		{"id":"col4","label":"Storage Temperature","type":"number"},
		{"id":"col4","label":"Return Temperature","type":"number"},
		{"id":"col4","label":"Collector On/Off","type":"number"},
		{"id":"col4","label":"Insulation Open/Closed","type":"number"},		
		{"id":"col4","label":"Vent Open/Closed","type":"number"}
		
		],"rows":[';
		$sep = "";
		foreach($ar as $elem){
			$mon = $elem["month"] - 1;
	/*		if($elem["cT"] > 50){
				$ct = '{},';
			}else{
				$ct = '{"v":' . $elem["cT"] .  '},';
			}	*/
			echo $sep . '{"c":[
			{"v":"Date(' . $elem["year"] . ',' . $mon . ',' . $elem["day"] . ',' . $elem["hour"] . ',' . $elem["minute"] . ')"},
			{"v":' . $elem["osT"] . '},
			{"v":' . $elem["isT"].  '},
			{"v":' . $elem["cT"] .  '},
			{"v":' . $elem["pT"]  . '},
			{"v":' . $elem['rT']  . '},
			{"v":' . $elem["colstate"]   . '},
			{"v":' . $elem["panelstate"] . '},
			{"v":' . $elem["ventstate"]  . '}

			]}';			
		$sep = ",";
	}
		echo "]}";		
	}		

}


?>


