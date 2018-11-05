<?php	// last change: 2018-09-16
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
			if (!empty($sensor['unit']))
				$value_txt = $sensor['value'].' '.$sensor['unit'];
			else
				$value_txt = $sensor['value'];

			echo "<div class='sensor-box'>";
			echo "<div class='sensor-icon'><img src='res/img/sensors/",$sensor_image,"' width='32' height='32' alt='",$sensor['type'],"' title='",$sensor_image,"'></div>";
			echo "<div class='sensor-name'>",$sensor['name'],"</div>";
			echo "<div class='sensor-value ",$value_type,"'>",$value_txt,"</div>";
			echo "<div class='sensor-change'>",dtstr($sensor['change']),"</div>";
			echo "<div class='sensor-options'><a href='index.php?view=sensor-edit&id=",$sensor['id'],"'><img src='res/img/ui/edit.png' width='24' height='24' alt='edit' title='edit'></a></div>";
			echo "<div class='sensor-options'><a href='index.php?view=sensors&id=",$sensor['id'],"&do=delete'><img src='res/img/ui/delete.png' width='24' height='24' alt='delete' title='delete'></a></div>";
// TODO			echo "<div class='sensor-chart' id='js-",$sensor['id'],"'></div>";
			echo "</div>"; // .sensor-box
		}

		echo "</div>"; // .sensor-container
return;
/*
		$tmpArr = array();
		foreach ($this->ohoco->status as $sensor_id => $sensor_array) {
			if (array_key_exists($sensor_id, $this->ohoco->config['names']))
				$sensor_name = $this->ohoco->config['names'][$sensor_id];
			else
				$sensor_name = $sensor_id;
			if (array_key_exists($sensor_id, $this->ohoco->config['types']))
				$sensor_type = $this->ohoco->config['types'][$sensor_id];
			else
				$sensor_type = 'unknown';

			$key = $sensor_name;

			$tmpArr[$key]['id'] = $sensor_id;
			$tmpArr[$key]['name'] = $sensor_name;
			$tmpArr[$key]['type'] = $sensor_type;
			$tmpArr[$key]['status'] = $sensor_array['status'];
			$tmpArr[$key]['change'] = $sensor_array['change'];
		}

		ksort($tmpArr);

		foreach ($tmpArr as $key => $sensor) {
			echo "<div class='sensor-box'>";

			$enable_chart = false;

			switch ($sensor['type']) {
				case 'unknown':
					$sensor_image = $sensor['type'].'.png';
					$enable_chart = is_numeric($sensor['status']);
					break;
				case 'time':
				case 'date':
					$sensor_image = $sensor['type'].'.png';
					$enable_chart = false;
					break;
				case 'temperature':
				case 'lightintensity':
				case 'voltage':
				case 'currency':
					$sensor_image = $sensor['type'].'.png';
					$enable_chart = true;
					break;
				case 'humidity':
					$raw_val = str_replace(' %', '', $sensor['status']);
					if ($raw_val < 40)
						$sensor_image = 'humidity-low.png';
					elseif ($raw_val > 60)
						$sensor_image = 'humidity-high.png';
					else
						$sensor_image = 'humidity-ok.png';
					$enable_chart = true;
					break;
// 				case 'daynight':	$sensor_image = 'daynight-'.$sensor['status'].'.png'; break;
// 				case 'weather':		$sensor_image = 'weather-'.$sensor['status'].'.png'; break;
// 				case 'gate':		$sensor_image = 'gate-'.$sensor['status'].'.png'; break;
// 				case 'light':		$sensor_image = 'light-'.$sensor['status'].'.png'; break;
				default:
					$sensor_image = ''.$sensor['type'].'-'.$sensor['status'].'.png';
					$enable_chart = true;
					break; //$sensor_image = 'unknown.png'; break;
			}

			echo "<div class='sensor-icon'><img src='res/img/sensors/",$sensor_image,"' width='32' height='32' alt='",$sensor['type'],"' title='",$sensor_image,"'></div>";
			echo "<div class='sensor-name'>",$sensor['name'],"</div>";
			switch ($sensor['status']) {
				case 'on':
				case 'open':
				case 'home':
					if ($enable_chart)
						echo "<div class='sensor-value is-on hand' onclick='return ohocochart.toggle(\"",$sensor['id'],"\", \"",$sensor['name'],"\");'>",$sensor['status'],"</div>";
					else
						echo "<div class='sensor-value is-on'>",$sensor['status'],"</div>";
					break;
				case 'off':
				case 'closed':
				case 'away':
					if ($enable_chart)
						echo "<div class='sensor-value is-off hand' onclick='return ohocochart.toggle(\"",$sensor['id'],"\", \"",$sensor['name'],"\");'>",$sensor['status'],"</div>";
					else
						echo "<div class='sensor-value is-off'>",$sensor['status'],"</div>";
					break;
				default:
					if ($enable_chart)
						echo "<div class='sensor-value is-val hand' onclick='return ohocochart.toggle(\"",$sensor['id'],"\", \"",$sensor['name'],"\");'>",$sensor['status'],"</div>";
					else
						echo "<div class='sensor-value is-val'>",$sensor['status'],"</div>";
			}
			echo "<div class='sensor-change'>",dtstr($sensor['change']),"</div>";
			echo "<div class='sensor-options'><a href='sensors.php?id=",$sensor['id'],"&do=rename'><img src='res/img/ui/edit.png' width='24' height='24' alt='edit' title='edit'></a></div>";
			echo "<div class='sensor-options'><a href='sensors.php?id=",$sensor['id'],"&do=delete'><img src='res/img/ui/delete.png' width='24' height='24' alt='delete' title='delete'></a></div>";
			echo "<div class='sensor-chart' id='js-",$sensor['id'],"'></div>";

			echo "</div>";
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

// 		echo "<div class='debug'>";
// 		debugarr($tmpArr);
// 		debugarr($this->ohoco->config);
// 		debugarr($this->ohoco->status);
// 		echo "</div>";
*/
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