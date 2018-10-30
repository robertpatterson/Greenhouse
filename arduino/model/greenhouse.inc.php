<?php

// greenhouse.inc.php R Patterson 2016

// include classes
//require_once('../common/class/DatabaseConnection.inc.php');

//echo "in model/greenhouse.inc";


class GreenhouseClass{
	
	public static function temperaturePut($osT,$isT,$cT,$pT,$rT,$col,$panel,$vent){
		global $curr_timestamp;
		$link = DatabaseConnection::Data();
		$sql = "
			INSERT INTO
				temperature
			SET
				osT = $osT,
				isT = $isT,
				cT = $cT,
				pT = $pT,
				rT = $rT,
				colstate = $col,
				panelstate = $panel,
				ventstate = $vent,
				time = '$curr_timestamp'";
		$res = mysqli_query($link, $sql);
		$sql = "
			UPDATE
				currentstate
			SET
				tempentrycount = tempentrycount + 1
			WHERE auto = 1";
		mysqli_query($link, $sql);
		$sql = "
			SELECT
				tempentrycount
			FROM `currentstate`
			WHERE auto = 1";
		$resp = mysqli_query($link,$sql);
		$row = mysqli_fetch_object($resp);
		return $sql;  //$row->tempentrycount;
	}		
		
	public static function restarttempcount(){
		$link = DatabaseConnection::Data();
		$sql = "
			UPDATE
				currentstate
			SET
				tempentrycount = 0
			WHERE auto = 1";
		mysqli_query($link, $sql);
	}
	
	
	
	public static function jobLogPut($source, $logic){
		global $curr_timestamp;
		$link = DatabaseConnection::Data();
		$sql = "
		INSERT INTO
			joblog
		SET
			source = '$source',
			logic = '$logic',
			time = '$curr_timestamp'";
//		echo "SQL= $sql";
		$res = mysqli_query($link, $sql);
	}
		
	public static function jobLogGet(){
		$link = DatabaseConnection::Data();
		$sql = "
			SELECT				
				CONCAT( time, ' ', source, ' ', logic) AS resp
			FROM
				joblog
			ORDER BY time DESC
			LIMIT 4 
		";
		//		echo "SQL= $sql";
		$resp = mysqli_query($link, $sql);
		$respText = "";
		while($row = mysqli_fetch_object($resp)){		
			$respText .= $row->resp . "<br>";
		}
		return $respText;
	}
	
	
	public static function eventlog($event){
		global $curr_timestamp;
		$link = DatabaseConnection::Data();
		$sql = "
			INSERT INTO
				log
			SET
				event = '$event',
				time = '$curr_timestamp'";
		$res = mysqli_query($link, $sql);
		return;
	}
		
	public static function updatestates($col,$panel,$vent,$devicespower,$autocontrol,$reset){
		global $curr_timestamp;
		$link = DatabaseConnection::Data();
		$sql = "
		UPDATE
			currentstate
		SET
			colstate = $col,
			panelstate = $panel,
			ventstate = $vent,
			devicespower = $devicespower,
			autocontrol = $autocontrol,
			reset = '$reset',
			time = '$curr_timestamp'
		WHERE auto = 1";
		mysqli_query($link, $sql);
		//echo $sql;
	}
	
	public static function setOpenTime(){
		global $curr_timestamp;
		$link = DatabaseConnection::Data();
		$sql = "
			UPDATE
				currentstate
			SET
				opentime = NOW()
			WHERE auto = 1";
		mysqli_query($link, $sql);
		//echo $sql;
	}
	
	
	public static function getOpenSufficientTime($minimumTimeOpen){
		global $curr_timestamp;
		$link = DatabaseConnection::Data();
		$sql = "
			SELECT 
				opentime 
			FROM  currentstate
			WHERE NOW() > (opentime + ". $minimumTimeOpen . ")";
		$resp = mysqli_query($link, $sql);
		if(mysqli_num_rows($resp) == 1){
			return true;
		}
		return false;
	}
	
	public static function currentState(){
		global $states;
		$link = DatabaseConnection::Data();
		$sql = "				
			SELECT
				colstate,
				panelstate,
				ventstate,
				devicespower,
				autocontrol,
				reset,
				opentime
			FROM `currentstate`
			
			WHERE auto = 1";
		$resp = mysqli_query($link,$sql);
		$row = mysqli_fetch_object($resp);
		$states['collectorPumpOn'] = $row->colstate;
		$states['panelsClosed'] = $row->panelstate;
		$states['ventOpen'] = $row->ventstate;
		$states['devicespower'] = $row->devicespower;
		$states['autocontrol'] = $row->autocontrol;
		$states['reset']       = $row->reset;
	}

	
	public static function averageTemp($tempName,$numberOfObservations){
		$link = DatabaseConnection::Data();
		$sql = "
			SELECT
				round(avg(". $tempName . "),1) as mean_temp			
			FROM (
				SELECT 
					". $tempName ."
				FROM `temperature`
				WHERE
					". $tempName ." > 0 AND " .$tempName ." < 180
					ORDER BY id DESC 
					LIMIT " . $numberOfObservations . ") AS
			T";
//		echo $sql;
		$resp = mysqli_query($link,$sql);
		$row = mysqli_fetch_object($resp);
		return $row->mean_temp;
	}
	
	

}




/*
 * 	time >= '2016-01-01' AND time <= NOW()
 * 
 * 
 * 
 * 
			SELECT
					SUM(donation_amount) AS total_donation_amount,
EXTRACT(YEAR from time_inserted) AS year,
EXTRACT(MONTH from time_inserted) AS month,
EXTRACT(DAY from time_inserted) AS day,
EXTRACT(HOUR from time_inserted) AS hour,
EXTRACT(MINUTE from time_inserted) AS minute
FROM `DONATION`
WHERE `time_inserted` > '2012-12-07 10:00:00'
AND `donation_type` = 'em1'
GROUP BY year, month, day, hour, minute;
*/
	
?>