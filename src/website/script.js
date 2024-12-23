<script type='text/javascript' src='https://ajax.googleapis.com/ajax/libs/jquery/1.4.4/jquery.min.js'></script> 
<script type='text/javascript' src='https://www.google.com/jsapi'></script>
<script type='text/javascript'>

	var chart; 
	var charts;
	var data;

      	google.load('visualization', '1', {packages:['gauge']});
      	google.setOnLoadCallback(initChart);

	function displayData(point) {
	
		data.setValue(0, 0, 'Light Level');
		data.setValue(0, 1, point);
		chart.draw(data, options);
	
	}

	function loadData() {
	
		// variable for the data point
		var p;
	
		$.getJSON('https://api.thingspeak.com/channels/9/feed/last.json?callback=?', function(data) {
		
		// get the data point
		p = data.field1;
	
		if (p)
		{
			p = Math.round((p / 1023) * 100);
			displayData(p);
		}
	
		});
	
	}

	function initChart() {

		data = new google.visualization.DataTable();
		data.addColumn('string', 'Label');
		data.addColumn('number', 'Value');
		data.addRows(1);
	        
	        chart = new google.visualization.Gauge(document.getElementById('chart_div'));
	        options = {width: 120, height: 120, redFrom: 90, redTo: 100,
	            yellowFrom:75, yellowTo: 90, minorTicks: 5};
	
		loadData();

		setInterval('loadData()', 15000);
	        
	}

</script>