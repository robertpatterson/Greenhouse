<?php

/*
 Robert patterson 11/26/2016
 



 ------------------LOCATION CHANGES ---------------------------------



    Only changes for different servers are here and at top of /js/greenhouse.js
   
    
    
*/   

// ------- LOCAL AND REMOTE SERVERS MAY DIFFER ------------------------------------

define("PW","dev");  // --- UI password

//----------------------------- EXCLUDE FOM REMOTE SERVER ------------------------------------

 ini_set('display_errors', 'On');

 error_reporting(E_ALL | E_STRICT);


//---------------- FOR LOCAL MACHINE --------------
// define("UNSERNAME","root");
// define("DBPASSWORD","SAMRAP");

//---------- FOR REMOTE SERVER ------------------------------
define("UNSERNAME","greenhouseman");
define("DBPASSWORD","s5a9m6r1aP");

//------------------- SUBJECT TO CHANGE AT WHIM OF VERIZON --------------------

define("URLANDTASK", "http://96.237.233.147/?task=");  // as of 9/22/18

//----------------------------------------------------------------------------------------

ini_set("date.timezone", "America/New_York");
$curr_timestamp = date("Y-m-d H:i:s");


require_once('common/class/DatabaseConnection.inc.php');

require_once('model/graphics.inc.php');

require_once('model/greenhouse.inc.php');  

require_once('controller/greenhouse.inc.php');

 require_once('controller/logic.inc.php');

require_once('controller/graphics.inc.php'); 

require_once('controller/main.inc.php');


// GLOBALS

$logicDescription;


 
?>