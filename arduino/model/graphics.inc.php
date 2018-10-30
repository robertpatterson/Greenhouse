<?php

/* greenhouse.inc.php R Patterson 2016

2018-7-19
Query for temps across months
SELECT time, isT FROM `temperature` WHERE isT < 50
and NOT isT = '0.0'
and time > '2017-09'
and time < '2018-06'
and hour(time) > '04'
and hour(time) < '06'
and minute(time) < '05'

//++++++++++++++++++++++++++++++FOR TESTING ++++++++++++++++++++++++++++++++++++++++++++++++

// include classes
//require_once('../common/class/DatabaseConnection.inc.php');

*/

//--------------------------------------END TEST CODE ------------------------------------

class GraphicsClass{
	
	
	public static function getGraphData($day_1,$day_2){		
		 
		if($day_1 == 0){
			$timePeriod = " time > CURRENT_DATE ";
			}else{
				$timePeriod = " time > CURRENT_DATE - INTERVAL " . $day_1 . " DAY";
				}
		$link = DatabaseConnection::Data();
		$sql = "				
			SELECT
				osT,
				isT,
				cT,
				pT,
				rT,
				case colstate
       				when 3 then 5 
       				when 2 then 3 
       				when 1 then 1 
       				ELSE 0
    				end as col,
   				case panelstate        				
					WHEN 3 then 6
					WHEN 2 then 8
        			ELSE 11
    				end as panel,
				case ventstate        				
					when 0 then 12
        			ELSE 16
    				end as vent,
				EXTRACT(YEAR from time) AS year,
				EXTRACT(MONTH from time) AS month,
				EXTRACT(DAY from time) AS day,
				EXTRACT(HOUR from time) AS hour,
				EXTRACT(MINUTE from time) AS minute,
				quarter,
				time				
			FROM `minutes15temps`
			WHERE
				" . $timePeriod . "
				ORDER BY auto ASC 
				LIMIT 500";
//		echo $sql;  
		$resp = mysqli_query($link,$sql);
		$ar = array();		
		while($row = mysqli_fetch_object($resp)){
			array_push($ar, array(
				'osT'		=>$row->osT,
				'isT'		=>$row->isT,
				'cT'		=>$row->cT,
				'pT'		=>$row->pT,
				'rT'		=>$row->rT,
				'colstate'	=>$row->col,
				'panelstate'=>$row->panel,
				'ventstate'	=>$row->vent,
				'quarter' 	=>$row->quarter,
				'year' 		=>$row->year,	
				'month' 	=>$row->month,
				'day' 		=>$row->day,
				'hour' 		=>$row->hour,
				'minute' 	=>$row->minute,
				'time'      =>$row->time
				));
		}
		return $ar;		 
	}

	
	public static function storeMeanTemp(){
		$link = DatabaseConnection::Data();
		$sql = "
			SELECT
				 *,
				 EXTRACT(MINUTE from time) AS minute,
				 EXTRACT(HOUR from time) AS hour
			FROM 
				`temperature`
			ORDER BY id DESC LIMIT 3";
//		echo $sql;
		$resp = mysqli_query($link,$sql);		// 15 minute intervals
		$id = 0;
		$osT = 0;
		$isT = 0;
		$cT = 0;
		$pT = 0;
		$rT = 0;
		$time = 0;
		$colstate = 0;
		$panelstate = 0;
		$ventstate = 0;
		$grabTime = 0;
		$osTDiv = 3;
		$isTDiv = 3;
		$cTDiv = 3;
		$pTDiv = 3;
		$rTDiv = 3;
		while($row = mysqli_fetch_object($resp)){
			$result = GraphicsClass::checkRange($row->osT,$osTDiv,-10,160);
			$osT += $result["temp"]; 
			$osTDiv = $result["div"];					
			$result = GraphicsClass::checkRange($row->isT,$isTDiv,10,150);
			$isT += $result["temp"]; 
			$isTDiv = $result["div"];
			$result = GraphicsClass::checkRange($row->cT,$cTDiv,-1,300);
			$cT += $result["temp"]; 
			$cTDiv = $result["div"];			
			$result = GraphicsClass::checkRange($row->pT,$pTDiv,10,150);
			$pT += $result["temp"]; 
			$pTDiv = $result["div"];
			$result = GraphicsClass::checkRange($row->rT,$rTDiv,10,200);
			$rT += $result["temp"]; 
			$rTDiv = $result["div"];
			$colstate += $row->colstate;		//	Gets sum
			$panelstate += $row->panelstate;
			$ventstate += $row->ventstate;
			$grabTime++;
			$hour = $row->hour;
			if($grabTime == 2){		// The middle value
				$time = $row->time;
				$id = $row->id;
				$min = $row->minute;
				switch (true) {		// decides which quarter hour it covers 
 					case $min >53:
        				$q = $hour + 1;
        				break;
   				 	case $min >38:
        				$q = $hour + .75;  // 4
        				break;
    				case $min >23:
        				$q = $hour + .50;   //3;
        				break;
    				case $min >8:
        				$q = $hour + .25;		//2;
       					break;
     				default:
        				$q = $q = $hour; 	//1;
       					break;
				}					
			}
		}
		$ostM = round($osT/$osTDiv,1);		
		$istM = round($isT/$isTDiv,1);
		$ctM = round($cT/$cTDiv,1);
		$ptM = round($pT/$pTDiv,1);
		$rtM = round($rT/$rTDiv,1);		
		$sql = "
			INSERT INTO
				minutes15temps                
			SET
				ost = $ostM,
				id   = $id,
				isT  = $istM,
				cT   = $ctM,
				pT   = $ptM,
				rT   = $rtM,
				colstate = $colstate,
				panelstate = $panelstate,
				ventstate = $ventstate,
				time = '$time',
				quarter = $q";				
//		echo $sql;
		mysqli_query($link,$sql);
	}	



	public static function checkRange($t,$divider,$low,$high){
		if($t > $high || $t< $low || $t == 0.0) {
			$t = 0;
			$divider--;
		}	
		if($divider == 0){
			$t = 30;
			$divider = 1;
		}
		$ar = ["temp"=>$t,"div"=>$divider];
		return $ar;		
	}

}
