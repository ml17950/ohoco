<?php	// last change: 2018-11-05
class clsSensorInterface extends clsUserInterface {
	var $ohoco;

	function __construct(&$ohoco) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$this->ohoco = $ohoco;
	}

	function __destruct() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
	}

	function display() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		echo "<div class='js-sensors sensor-container'>";

		$sensor_array = array();

		foreach ($this->ohoco->config['sensor'] as $sensor_id => $sensor) {
			if (!empty($sensor['name']))
				$key = strtolower($sensor['name']);
			else {
				$key = strtolower($sensor_id);
				$sensor['name'] = $sensor_id;
			}
			$sensor['id'] = $sensor_id;
			$sensor_array[$key] = $sensor;
		}

		ksort($sensor_array);
// debugarr($sensor_array);

		foreach ($sensor_array as $key => $sensor) {
			$sensor_image = $this->get_icon_by_type($sensor['type'], $sensor['value']);
			$value_type = $this->get_value_type($sensor['value']);
			$enable_chart = $this->is_chart_enabled($sensor['type'], $sensor['value']);
			if (!empty($sensor['unit']))
				$value_txt = $sensor['value'].' '.$sensor['unit'];
			else
				$value_txt = $sensor['value'];

			echo "<div class='sensor-box'>";
			echo "<div class='sensor-icon'><img src='res/img/sensors/",$sensor_image,"' width='32' height='32' alt='",$sensor['type'],"' title='",$sensor_image,"'></div>";
			echo "<div class='sensor-name'>",$sensor['name'],"</div>";
			if ($enable_chart)
				echo "<div class='sensor-value ",$value_type," hand' onclick='return ohocochart.toggle(\"",$sensor['id'],"\", \"",$sensor['name'],"\");'>",$value_txt,"</div>";
			else
				echo "<div class='sensor-value ",$value_type,"'>",$value_txt,"</div>";
			echo "<div class='sensor-change'>",dtstr($sensor['change']),"</div>";
			echo "<div class='sensor-options'><a href='index.php?view=sensor-edit&id=",$sensor['id'],"'><img src='res/img/ui/edit.png' width='24' height='24' alt='edit' title='edit'></a></div>";
			echo "<div class='sensor-options'><a href='index.php?view=sensors&id=",$sensor['id'],"&do=delete'><img src='res/img/ui/delete.png' width='24' height='24' alt='delete' title='delete'></a></div>";
			echo "<div class='sensor-chart' id='js-",$sensor['id'],"'></div>";
			echo "</div>"; // .sensor-box
		}

		echo "</div>"; // .sensor-container

		echo "<script language='javascript' type='text/javascript' src='res/js/flot/jquery.flot.js'></script>";
		echo "<script language='javascript' type='text/javascript' src='res/js/flot/jquery.flot.time.js'></script>";
		echo "<script language='javascript' type='text/javascript' src='res/js/flot/jquery.flot.navigate.js'></script>";
// 		echo "<script language='javascript' type='text/javascript' src='res/js/flot/jquery.flot.selection.js'></script>";
// 		echo "<script language='javascript' type='text/javascript' src='res/js/flot/jquery.flot.touch.js'></script>";
		echo "<script>";
		echo "$(\"<div id='tooltip'></div>\").css({\n";
		echo "	position: 'absolute',\n";
		echo "	display: 'none',\n";
		echo "	border: '1px solid #fdd',\n";
		echo "	padding: '2px',\n";
		echo "	'background-color': '#fee',\n";
		echo "	opacity: 0.80\n";
		echo "}).appendTo('body');\n";
		echo "</script>";
	}

	function edit($sensor_id) {
		echo "<div class='form-container'>";

		echo "<form method='POST' action='index.php?view=sensor-edit' accept-charset='utf-8'>";
		echo "<input type='hidden' name='do' value='save-sensor'>";
		echo "<input type='hidden' name='id' value='",$sensor_id,"'>";

		echo "<label for='currid'>Sensor-ID</label>";
		echo "<span id='currid'>",$sensor_id,"</span><br>";

		echo "<label for='newname'>Name</label>";
		echo "<input type='text' name='newname' value='",$this->ohoco->config['sensor'][$sensor_id]['name'],"'><br>";

		echo "<label for='newvalue'>Wert</label>";
		echo "<input type='text' name='newvalue' value='",$this->ohoco->config['sensor'][$sensor_id]['value'],"'><br>";

		echo "<label for='oldunit'>Einheit</label>";
		echo "<input type='text' name='oldunit' value='",$this->ohoco->config['sensor'][$sensor_id]['unit'],"' disabled><br>";

		echo "<label for='oldchange'>Wert von</label>";
		echo "<input type='text' name='oldchange' value='",dtstr($this->ohoco->config['sensor'][$sensor_id]['change']),"' disabled><br>";

		echo "<input type='submit' value='Speichern'>";
		echo "</form>";

		echo "<em>",TXTAPILINKS,"<br><br>";
		if (array_key_exists($sensor_id, $this->ohoco->config['switch'])) {
			echo BASEURL.'/rpc/switch/on/?id='.$sensor_id;
			echo "<br><br>";
			echo BASEURL.'/rpc/switch/off/?id='.$sensor_id;
			echo "<br><br>";
			echo BASEURL.'/rpc/switch/toggle/?id='.$sensor_id;
		}
		else
			echo BASEURL.'/rpc/sensor/value/?id='.$sensor_id.'&payload=%val%';
		echo "</em>";

		echo "</div>";
	}
}
?>