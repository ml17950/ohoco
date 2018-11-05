<?php	// last change: 2018-10-08
class clsHomeInterface extends clsUserInterface {
	var $ohoco;

	function __construct(&$ohoco) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$this->ohoco = $ohoco;
	}

	function __destruct() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
	}

	function display_widgets() {
		if (is_array($this->ohoco->config['widgets']) && count($this->ohoco->config['widgets']) > 0) {
			echo "<div class='js-widgets widget-container'>";
			foreach ($this->ohoco->config['widgets'] as $num => $id) {
				if (!empty($id)) {
					$sensor = $this->ohoco->config['sensor'][$id];
					$sensor_image = $this->get_icon_by_type($sensor['type'], $sensor['value']);
					if (!empty($sensor['name']))
						$sensor_name = $sensor['name'];
					else
						$sensor_name = $id;

					// select the status-text
					switch ($sensor['type']) {
						case 'time':
						case 'date':
						case 'temperature':
						case 'humidity':
						case 'weather':
						case 'lightintensity':
          case 'voltage' :
							if (!empty($sensor['unit']))
								$widget_txt = $sensor['value'].' '.$sensor['unit'];
							else
								$widget_txt = $sensor['value'];
							break;
						case 'currency':
							$widget_txt = number_format($sensor['value'], 2, ',', '.').' &euro;';
							break;
						default:
							//$widget_txt = TXTSINCE.' '.date('H:i', $sensor['change']);
							$widget_txt = dtstr($sensor['change'], 'd.m. H:i', 'H:i');
					}

					echo "<div class='widget-box'>";
					echo "<div class='widget-title'>",$sensor_name,"</div>";
					echo "<div class='widget-icon'><img src='res/img/sensors/",$sensor_image,"' width='38' height='38' alt='",$id,"'></div>";
					echo "<div class='widget-status'>",$widget_txt,"</div>";
					echo "</div>";
				}
			}
			echo "</div>"; // .widget-container
		}
	}

	function display_switches() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		echo "<div class='js-devices switch-container'>";

		if (count($this->ohoco->config['switch']) > 0) {
			foreach ($this->ohoco->config['switch'] as $switch_id => $device_item) {
				//$device_info = explode(':', $device_item);
				//$switch_protocol = $device_info[0];
				//$switch_device	 = $device_info[1];

				if (!empty($this->ohoco->config['sensor'][$switch_id]['name']))
					$switch_name = $this->ohoco->config['sensor'][$switch_id]['name'];
				else
					$switch_name = $switch_id;
				$switch_type = $this->ohoco->config['sensor'][$switch_id]['type'];
				if (!empty($this->ohoco->config['sensor'][$switch_id]['value']))
					$switch_value = $this->ohoco->config['sensor'][$switch_id]['value'];
				else
					$switch_value = 'off';

				echo "<div class='switch-box hand' id='device-",$switch_id,"' onclick='return ohocoswitch.toggle(\"",$switch_id,"\", \"",$switch_type,"\");'>";
				echo "<div class='switch-icon'><img id='device-",$switch_id,"-icon' data-state='",$switch_value,"' src='res/img/sensors/",$switch_type,"-",$switch_value,".png' width='48' height='48' alt='",$switch_type,"'></div>";
				echo "<div class='switch-name'>",$switch_name,"</div>";
				echo "<div class='switch-time'>",dtstr($this->ohoco->config['sensor'][$switch_id]['change'], 'd.m H:i:s'),"</div>";
				echo "</div>";
			}
		}

		echo "</div>"; // .switch-container

// 		echo "<div class='debug'>";
// 		debugarr($this->ohoco->conf['devices']);
// 		debugarr($this->ohoco->status);
// 		echo "</div>";
	}
}
?>