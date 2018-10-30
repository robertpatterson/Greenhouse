// Greenhouse.js file 2/12/16 RDP



//-------------------- LOCAL AND REMOTE SERVERS DIFFER --------------------------


var variablesOb = new Object;
//	variablesOb.homeURL = 'http://localhost/greenhouse/';
	variablesOb.homeURL = 'http://robertpattersonus.com/arduino/';	



	variablesOb.loggedIn = false;

var autocontrol = 'On';	
	
window.onload = ready;	
	
function ready(){
	sendOrder('1');
}	

function sendOrder(param){  // param is one char
	if(! variablesOb.loggedIn && param != '1'){
		alert("Please log in.");
		return;
	}
	var url = variablesOb.homeURL + "index.php?task=userCalling&job=" + param; 
	$.get(url, function(data){
		var dAray = data.split("^");
		$("#time").html(dAray[0]);
		$("#to").html(dAray[1]);
		$("#tr").html(dAray[2]);
		$("#tp").html(dAray[3]);
		$("#tc").html(dAray[4]);
		$("#ti").html(dAray[5]);
		$panelTxt = "Open";
		if(dAray[6] == 1){
			$panelTxt = "Closed";
		}			
		$("#panels").html($panelTxt);
		$ventTxt = "Open";
		if(dAray[7] == 0){
			$ventTxt = "Closed";
		}		
		$("#vent").html($ventTxt);
		$colTxt = "On";
		if(dAray[8] == 0){
			$colTxt = "Off";
		}		
		$("#collectorPump").html($colTxt);
		$("#autoControl").html(dAray[9]);
		$("#ventOpen").html(dAray[10] + "&deg;");		
		$("#ventClose").html(dAray[11] + "&deg;");
		$("#collDeltaOn").html(dAray[12] + "&deg;");
		$("#collDeltaOff").html(dAray[13] + "&deg;");
		$("#panelCanOpenTime").html(dAray[14]);
		$("#panelMustCloseTime").html(dAray[15]);
		$("#panelTempDeltaOpen").html(dAray[16] + "&deg;");
		$("#panelTempDeltaClose").html(dAray[17] + "&deg;");
		$("#volts12").html(dAray[18]);
		$("#maxPanelCloseTemp").html(dAray[19] + "&deg;");
//		$("#panelMovement").html(dAray[20]);
//		$("#logEventCode").html(dAray[21]);
		$("#logic").html(dAray[22]);
		
		autocontrol = dAray[9];
    });
}

function logIn(){
	var pw = $("#logIn").val();
	var url = variablesOb.homeURL + "index.php?task=password&pw="  + pw; 
	$.get(url, function(data){
		var n = data.search("ok");
		if(n > 0){
			variablesOb.loggedIn = true;
			$("#logInDiv").html("Logged in");
		}else{
			alert("Log in password incorrect.");
		}				
	});		
}

//------------------------ GRAPHS ------------------------------------


function drawChart() {
	var graphType = $("input:radio[name ='graphChoice']:checked").val();
	var day_1 = 1;
	var day_2 = 0;
	if(graphType == "last2"){
		day_1 = 2;
		day_2 = 0
	}
	if(graphType == "last4"){
		day_1 = 4;
		day_2 = 0
	}
	var options = {
			chart: {
				title: 'Greenhouse Conditions Today',
				subtitle: 'Hover over lines to see more.'
			},
			width: 900,
			height: 500,
	        hAxis: {
	            viewWindow: {
	            },
	            gridlines: {
	              count: -1,
	              units: {
	                days: {format: ['MMM dd']},
	                hours: {format: ['HH:mm', 'ha']},
	              }
	            },
	            minorGridlines: {
	              units: {
	                hours: {format: ['hh:mm:ss a', 'ha']},
	                minutes: {format: ['HH:mm a Z', ':mm']}
	              }
	            }  
	          }		
	    };
	jsonData = $.ajax({		// jsondata is a json string
			// day_1 must 0 (current day) be earlier than day_
			url: variablesOb.homeURL + "index.php?task=makeGraphs&day_1=" + day_1 + "&day_2=" + day_2,
			dataType: "json",
			async: false
	}).responseText;
	var json_parsed = $.parseJSON(jsonData);
	data = new google.visualization.DataTable(json_parsed); 
	view = new google.visualization.DataView(data);
	table = new google.charts.Line(document.getElementById("chart_div"));
	table.draw(view, options);

  
}




