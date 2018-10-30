<?php
// rdpatterson | 2015-6-6 



class DatabaseConnection{	
	
	public static function Data(){
		global $jsonObj;
		$ver = 'greenhouse';
		//$link = $link = mysqli_connect('localhost', 'greenhouseman', 's5a9m6r1aP', $ver);
		$link = mysqli_connect('localhost', UNSERNAME, DBPASSWORD, $ver);
		if (!$link) {			
			die('Connects Error (' . mysqli_connect_errno() . ') '
			    . mysqli_connect_error()) . DBPASSWORD;
		}
		return $link;
	}	
}
	
	

?>
