

var url = "http://localhost/arduino/index.php?task=userCalling&job=; 


//this.homeURL = 'http://robertpattersonus.com/arduino/';	
//this.homeURL = 'http://localhost/arduino/';




$.get(url, function(data){
	var dAray = data.split("^");
	$("#time").html(dAray[0]);

	
	autocontrol = dAray[9];
});