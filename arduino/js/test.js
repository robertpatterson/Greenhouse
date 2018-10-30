
      google.charts.load('current', {'packages':['line']});
      google.charts.setOnLoadCallback(drawChart);

    function drawChart() {
  		jsonData = $.ajax({		// jsondata is a json string
  			url: "controller/graphics.inc.php",
  			dataType: "json",
  			async: false
  		}).responseText;
  		var json_parsed = $.parseJSON(jsonData);
  		data = new google.visualization.DataTable(json_parsed);    	    
      var options = {
        chart: {
          title: 'Greenhouse Conditions',
          subtitle: ''
        },
        width: 900,
        height: 500
      };

      var chart = new google.charts.Line(document.getElementById("chart_div"));  //'));

      chart.draw(data, google.charts.Line.convertOptions(options));
   }
