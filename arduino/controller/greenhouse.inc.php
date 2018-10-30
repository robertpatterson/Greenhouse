<?php


//echo "in controller/greenhouse.inc";

function sendPage(){
	return '
 <!DOCTYPE html>
<html>
<head>

    <meta content="text/html; charset=UTF-8" http-equiv="content-type">    
    <meta content="user-scalable=yes, initial-scale=1, maximum-scale=3.0, width=device-width" name="viewport">
        <!--	<meta http-equiv="Refresh" Content="60">  -->
    <link rel="stylesheet" href="css/styleH.css">

    <script src="js/greenhouse.js"></script>

    
 
<!--                           Jquery                    -->

    <script src="https://code.jquery.com/jquery-1.12.4.js"></script>
    <script src="https://code.jquery.com/ui/1.12.1/jquery-ui.js"></script>
    <link rel="stylesheet" href="//code.jquery.com/ui/1.12.1/themes/base/jquery-ui.css">


<script  src="https://www.gstatic.com/charts/loader.js"></script>

<script type="text/javascript">
google.charts.load("current", {packages: ["corechart", "line"]});
google.charts.setOnLoadCallback(drawChart);
</script>

<script src="js/greenhouse.js"></script>

   <title>Greenhouse Controller</title>
		
</head>
      <body>
           <body>
        <h2>Blossom St. Greenhouse</h2>
        <table border="2" cellpadding="2" cellspacing="0" width="324">
          <tbody>
            <tr>
              <td>
                <table border="2" cellpadding="2" cellspacing="0" width="324">
                  <tbody>
                    <tr>
                      <td colspan="5"><span id="time">Getting data...</span></td>
                    </tr>
                    <tr>
                      <td colspan="2"><b>Temperatures</b></td>
                      <td>&nbsp;</td>
                      <td colspan="2"><b>Other Conditions</b></td>
                    </tr>
                    <tr>
                      <td width="133">T Outside</td>
                      <td width="67"><span id="to"></span><br>
                      </td>
                      <td width="4">&nbsp;</td>
                      <td width="99">Panels </td>
                      <td width="47"><span id="panels"></span><br>
                      </td>
                    </tr>
                    <tr>
                      <td>Inside</td>
                      <td><span id="ti"></span><br>
                      </td>
                      <td>&nbsp;</td>
                      <td>Vent </td>
                      <td><span id="vent"></span><br>
                      </td>
                    </tr>
                    <tr>
                      <td>Collector</td>
                      <td><span id="tc"></span><br>
                      </td>
                      <td>&nbsp;</td>
                      <td>Collector pump</td>
                      <td><span id="collectorPump"></span><br>
                      </td>
                    </tr>
                    <tr>
                      <td>Storage</td>
                      <td><span id="tp"></span><br>
                      </td>
                      <td>&nbsp;</td>
                      <td>Auto Control</td>
                      <td><span id="autoControl"></span><br>
                      </td>
                    </tr>
                    <tr>
                      <td>Collector return</td>
                      <td><span id="tr"></span><br>
                      </td>
                      <td>&nbsp;</td>
                      <td>Devices power</td>
                      <td><span id="volts12"></span><br>
                      </td>
                    </tr>
                  </tbody>
                </table>

  Graph choices:<input value="today" checked="checked" name="graphChoice" type="radio">Today<br>
                              <input value="last2" name="graphChoice" type="radio">Yesterday and today<br>
                              <input value="last4" name="graphChoice" type="radio">Last 4 days<br>
                                      <br>
        		<input onclick="sendOrder(\'1\')" value="Update Data" type="button"><br><br>
              </td>
              <td>
                <div id="chart_div" style="width: 900px; height: 500px">Graph
                  here.</div>
              </td>
            </tr>
          </tbody>
        </table>
		
      
<table style="width: 831px; height: 473px;" border="2" cellpadding="2" cellspacing="0">	
<tbody><tr><td><h3>Actions</h3>
	<div id="logInDiv">Log in to make changes: <input id="logIn" type="password">
	<input onclick="logIn()" value="Log In" type="button"></div>		
		

		
<table border="2" cellpadding="2" cellspacing="0" width="300">

<tbody><tr>
		<td><input onclick="sendOrder(\'E\')" value="Stop All, Clear Cue" type="button"><br></td>
		<td><br></td>
		<td><input onclick="sendOrder(\'K\')" value="Reset Arduino" type="button"><br></td>
				
  </tr><tr>
</tr><tr>

		<td><input onclick="sendOrder(\'A\')" value="Automatic Controls OFF" type="button"><br></td>
		<td><br></td>
		<td><input onclick="sendOrder(\'B\')" value="Automatic Controls ON" type="button"><br></td>		
  </tr><tr>		
</tr><tr>
		<td><input onclick="sendOrder(\'C\')" value="Turn off devices power" type="button"><br></td>
		<td><br></td>
		<td><input onclick="sendOrder(\'D\')" value="Turn on devices power" type="button"><br></td>		
  </tr><tr>
  </tr><tr>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>		
<tr>		
		<td><input onclick="sendOrder(\'7\')" value="Close All Panels" type="button"><br></td>
		
		<td><br></td>
		<td><input onclick="sendOrder(\'6\')" value="Open All Panels" type="button"><br></td>	
  </tr><tr>	
    <td>
		<input onclick="sendOrder(\';\')" value="Close West Panel (1)" type="button"><br>		
		</td>
    <td>&nbsp;</td>
    <td>		
		<input onclick="sendOrder(\'L\')" value="Open West Panel (1)" type="button"><br>		
		</td>
  </tr>		
  <tr>
    <td><input onclick="sendOrder(\'<\')" value="Close Middle Panel (2)" type="button"><br>		
		</td>
    <td>&nbsp;</td>
    <td>	
		<input onclick="sendOrder(\'?\')" value="Open Middle Panel (2)" type="button"><br>		
		</td>
  </tr>		
  <tr>
    <td>		
		<input onclick="sendOrder(\'=\')" value="Close East Panel (3)" type="button"><br>		
		</td>
    <td>&nbsp;</td>
    <td>		
		<input onclick="sendOrder(\'@\')" value="Open East Panel (3)" type="button"><br>		
		</td>
  </tr>
	<tr>
      <td> <input onclick="sendOrder(\'N\')" value="Close Top  Panel" type="button"><br>		
     </td>		
	 <td>&nbsp;</td>
     <td> <input onclick="sendOrder(\'H\')" value="Open Top Panel" type="button"><br>		
  </td>			
	</tr>
  </tr><tr>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>	
  <tr>
    <td>	
		<input onclick="sendOrder(\'4\')" value="Turn on Collectors" type="button"><br>		
		</td>
    <td>&nbsp;</td>
    <td>		<input onclick="sendOrder(\'5\')" value="Turn off Collectors" type="button"><br>		
		</td>
  </tr>
  <tr>
    <td>		
		<input onclick="sendOrder(\'2\')" value="Open Vent" type="button">		
		</td>
    <td>&nbsp;</td>
    <td>
		<input onclick="sendOrder(\'3\')" value="Close Vent" type="button">		
		</td>
  </tr>
  <tr>
    <td>
		<input onclick="sendOrder(\'F\')" value="Fill air seals" type="button"><br>		
		</td>
    <td>&nbsp;</td>
    <td>		
		<input onclick="sendOrder(\'8\')" value="Empty air seals" type="button"><br>		
		</td>
  </tr>
	</tbody></table>
v 10-30-2018
</td><td>
			<h3>Recent Events</h3>
			<span id="logic">None yet.</span>
		<h3>Control Logic Parameters</h3>
		 

<table style="width: 410px; height: 299px;" border="2" cellpadding="2" cellspacing="0">
  <tbody><tr>
    <td width="94">Vent T to open<br>Inside temp &gt; This</td>
    <td width="30"><span id="ventOpen"></span><br></td>
    <td width="7">&nbsp;</td>
    <td width="123">Panel can open time</td>
    <td width="43"><span id="panelCanOpenTime"></span><br></td>
  </tr>
  <tr>
    <td>Vent T to close<br>Inside temp &lt; This</td>
    <td><span id="ventClose"></span><br></td>
    <td>&nbsp;</td>
    <td>Panel must close time</td>
    <td><span id="panelMustCloseTime"></span><br></td>
  </tr>
  <tr>
		
		
		
    <td>Coll T delta on<br>Collector &gt; pool + this</td>
    <td><span id="collDeltaOn"></span><br></td>
    <td>&nbsp;</td>
    <td>Panel T delta open<br>Collector &gt; inside + This</td>
    <td><span id="panelTempDeltaOpen"></span><br></td>
  </tr>
  <tr>
    <td>Coll T delta off<br>Return &lt; pool + This</td>
    <td><span id="collDeltaOff"></span><br></td>
    <td>&nbsp;</td>
    <td>Panel T delta close<br>Collector &lt; inside + This</td>
    <td><span id="panelTempDeltaClose"></span><br></td>
  </tr>
  <tr>
    <td>maxPanelCloseTemp</td>
    <td><span id="maxPanelCloseTemp"></span><br></td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>

</tbody></table>

</td>
</tr>
</tbody>
     
</body></html>
';   /// end of string
}
?>
