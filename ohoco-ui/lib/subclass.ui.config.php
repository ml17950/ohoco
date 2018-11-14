<?php	// last change: 2018-11-14
class clsConfigInterface {
	var $ohoco;

	function __construct(&$ohoco) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$this->ohoco = $ohoco;
	}

	function __destruct() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
	}

	function formHeader($action) {
		echo "<form method='POST' action='index.php?view=config' accept-charset='utf-8' class='form-container'>";
		echo "<input type='hidden' name='do' value='",$action,"'>";
	}

	function formFooter() {
		echo "</form>";
	}

	function display_devices() {
		echo "<h2>",CFGDEVICES," - UI Version: ",VERSION,"</h2>";
		echo "<div class='device-container'>";

		if (is_array($this->ohoco->config['device']) && (count($this->ohoco->config['device']) > 0)) {
			ksort($this->ohoco->config['device']);

			foreach ($this->ohoco->config['device'] as $device_id => $device) {
				$last_ping_dt = $device['alive'];
				$last_ping_seconds = time() - $last_ping_dt;
				$register_dt = $device['connect'];

				echo "<div class='device-box'>";
				echo "<div class='device-name'>",$device['name'],"</div>";
				echo "<div class='device-id'>",$device_id,"</div>";
				echo "<div class='device-wifiname'><em>WiFi SSID</em><br>",$device['ssid'],"</div>";
				echo "<div class='device-wifisignal'><em>Signal</em><br>",$device['signal'],"</div>";
				echo "<div class='device-wifiip'><em>IP</em><br>",$device['ip'],"</div>";
				echo "<div class='device-protocol'><em>Protocol</em><br>",$device['protocol'],"</div>";

				if (floatval(substr($device['voltage'], 0, -2)) < 2.4)
					echo "<div class='device-voltage is-red'><em>Voltage</em><br>",$device['voltage'],"</div>";
				else
					echo "<div class='device-voltage'><em>Voltage</em><br>",$device['voltage'],"</div>";
				echo "<div class='device-sketch'><em>Sketch-Ver.</em><br>",$device['sketch'],"</div>";
				echo "<div class='device-ohoco'><em>OHoCo-Ver.</em><br>",$device['ohoco'],"</div>";
				if ((time() - $last_ping_dt) > intval($device['timeout']))
					echo "<div class='device-runtime is-red'><em>Status</em><br>OFFLINE</div>";
				else
					echo "<div class='device-runtime'><em>Runtime</em><br>",$device['uptime'],"</div>";

				echo "<div class='device-connected'><em>Connected</em><br>",dtstr($device['connect'], 'd.m.y H:i'),"</div>";
				if ((time() - $last_ping_dt) > intval($device['timeout']))
					echo "<div class='device-lastping is-red'><em>Last Ping</em><br>",time_diff($last_ping_seconds, false),"</div>";
				else
					echo "<div class='device-lastping'><em>Last Ping</em><br>",time_diff($last_ping_seconds, false),"</div>";

				echo "<div class='device-options'>";
// TODO					echo "<a href='config.php?id=",$device_id,"&cmd=REREG&do=sendcmd' class='config-button green-button'>REREG</a>";
					echo " <a href='index.php?view=config&id=",$device_id,"&cmd=REBOOT&do=sendcmd' class='config-button orange-button'>REBOOT</a>";
					echo " &bull; <a href='index.php?view=config&id=",$device_id,"&do=delete' class='config-button red-button'>DELETE</a>"; //<img src='res/img/ui/delete.png' width='24' height='24' alt='delete' title='delete'></a>";
					if ($device['ohoco'] >= '18.10.04')
						echo " &bull; <a href='index.php?view=config-edit&id=",$device_id,"' class='config-button cyan-button'>CONFIG</a>";

// 				echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=CFG:checkInterval=900000&do=sendcmd'>15 Min</a>";
// 				echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=CFG:checkInterval=300000&do=sendcmd'>5 Min</a>";
// 				echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=CFG:checkInterval=120000&do=sendcmd'>2 Min</a>";
// 				echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=CFG:checkInterval=60000&do=sendcmd'>1 Min</a>";
// 				echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=CFG:checkInterval=15000&do=sendcmd'>15 Sec</a>";
// 				echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=CFG:checkInterval=5000&do=sendcmd'>5 Sec</a>";
// 				
// 				echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=WRITECFG&do=sendcmd'>WRITECFG</a>";
// 				echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=SENDCFG&do=sendcmd'>SENDCFG</a>";
					echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=DEBUG&do=sendcmd' class='config-button grey-button'>DEBUG</a>";
					if (($device['ohoco'] >= '18.10.05') && is_file(ABSPATH.'/ota-files/'.$device_id.'-'.$device['sketch'].'.bin'))
						echo " &bull; <a href='index.php?view=config&id=",$device_id,"&cmd=OTAUPD&do=sendcmd' class='config-button blue-button'>OTAUPD</a>";

				echo "</div>";
				echo "</div>"; // .device-box
			}

			echo "</div>"; // .device-container
		}

// 		echo "<div class='debug'>";
// 		debugarr($this->ohoco->config['device']);
// 		echo "</div>";
	}

	function display_device_configuration($device_id) {
		$config = $this->ohoco->config['device'][$device_id]['config'];

		if (is_array($config) && (count($config) > 0)) {
			if ((time() - $config['upd']) < 60) {
				$cfg_key_replacements = array(	'chk' => 'checkInterval',
												'min' => 'minValue',
												'max' => 'maxValue',
												'itr' => 'inTrigger',
												'otr' => 'outTrigger',
												'din' => 'Device-Name',
												'wid' => 'WiFi-SSID',
												'wpw' => 'WiFi-PASS',
												'cip' => 'Control-IP',
												'cpo' => 'Control-Port',
												'cus' => 'Control-User',
												'cpw' => 'Control-Pass',
												'mqt' => 'useMQTT');
				unset($config['upd']);

				echo "<input type='submit' value='",CFGDEVCFG02,"' onclick='return ohococonfig.writecfg(\"",$device_id,"\");'>";
				echo "<br><br>";

				foreach ($config as $key => $val) {
					$cfgname = str_replace(array_keys($cfg_key_replacements), array_values($cfg_key_replacements), $key);
					echo "<label for='",$key,"'>",$cfgname,"</label>";
					echo "<input type='text' name='",$key,"' id='js-",$key,"' value='",$val,"' onchange='return ohococonfig.updval(\"",$device_id,"\",\"",$key,"\",this.value);'><br>";
				}

				echo "<br><br>",CFGDEVCFG03;
			}
			else {
				// config to old -> send request to device
				$this->ohoco->device_send_control_command($device_id, 'SENDCFG');
				echo "<br><br>",CFGDEVCFG01,"<br><br>";
				echo "<meta http-equiv='refresh' content='1; URL=index.php?view=config-edit&id=",$device_id,"'>";
			}
		}
		else {
			// no config found -> send request to device
			$this->ohoco->device_send_control_command($device_id, 'SENDCFG');
			echo "<br><br>",CFGDEVCFG01,"<br><br>";
			echo "<meta http-equiv='refresh' content='1; URL=index.php?view=config-edit&id=",$device_id,"'>";
		}

// 		$cfg_file = ABSPATH.'/config/'.$device_id.'.cfg';
// 
// 		if (file_exists($cfg_file)) {
// 			$cfg_time = filemtime($cfg_file);
// 
// 			if ((time() - $cfg_time) > 1200) {
// 				echo "<br><br>",CFGDEVCFG01,"<br><br>";
// 				echo "<meta http-equiv='refresh' content='1; URL=index.php?view=config&view=devicecfg&id=",$device_id,"&cmd=SENDCFG&do=sendcmd'>";
// 			}
// 			else {
// 				$cfg_data = file_get_contents($cfg_file);
// 				$cfg_rows = explode('|', trim($cfg_data));
// 				$cfg_key_replacements = array(	'chk' => 'checkInterval',
// 												'min' => 'minValue',
// 												'max' => 'maxValue',
// 												'itr' => 'inTrigger',
// 												'otr' => 'outTrigger',
// 												'wip' => 'WiFi-SSID',
// 												'wpw' => 'WiFi-PASS',
// 												'cip' => 'Controller-IP',
// 												'cpo' => 'Controller-Port');
// 
// 				echo "<div class='form-container'>";
// 
// 				foreach ($cfg_rows as $row) {
// 					$fields = explode(':', $row);
// 					$cfgname = str_replace(array_keys($cfg_key_replacements), array_values($cfg_key_replacements), $fields[0]);
// 					echo "<label for='newname'>",$cfgname,"</label>";
// 					echo "<input type='text' name='newname' id='js-",$cfgname,"' value='",$fields[1],"' onchange='return ohococonfig.updval(\"",$device_id,"\",\"",$cfgname,"\",this.value);'><br>";
// 				}
// 
// 				echo "<input type='submit' value='",CFGDEVCFG02,"' onclick='return ohococonfig.writecfg(\"",$device_id,"\");'>";
// 				echo "<br><br>",CFGDEVCFG03;
// 
// 				echo "</div>";
// 			}
// 		}
// 		else {
// 			echo "<br><br>",CFGDEVCFG01,"<br><br>";
// 			echo "<meta http-equiv='refresh' content='1; URL=index.php?view=config&view=devicecfg&id=",$device_id,"&cmd=SENDCFG&do=sendcmd'>";
// 		}
	}

	function display_system() {
		echo "<div class='config-container'>";
		echo "<h2>",CFGSYSTEM,"</h2>";

		echo "<span>UI Version: ",VERSION,"</span><br><br>";
		
		echo "<input type='checkbox' name='ready' value='yes'> ",CFGSYS01,"<br<";
		
		echo "</div>"; // .widget-container
	}

	function display_widgets() {
		echo "<h2>",CFGWIDGETS,"</h2>";
		echo "<div class='config-container'>";

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

		for ($i=0; $i<=8; $i++) {
			echo "<label for='widget[",$i,"]'>Widget ",($i + 1),"</label>";
			echo "<select name='widget[",$i,"]' size='1'>";
			echo "<option value=''>-</option>";
			foreach ($sensor_array as $key => $sensor) {
				if ($sensor['id'] == $this->ohoco->config['widgets'][$i])
					echo "<option value='",$sensor['id'],"' selected>",$sensor['name'],"</option>";
				else
					echo "<option value='",$sensor['id'],"'>",$sensor['name'],"</option>";
			}
			echo "</select><br>";
		}

		echo "<input type='submit' value='Speichern'>";

		echo "</div>"; // .config-container
	}

	function display_mqtt() {
		echo "<div class='config-container'>";
		echo __CLASS__.'::'.__FUNCTION__.'<br>';
		echo "</div>"; // .config-container
	}

	function display_types() {
		echo "<div class='config-container'>";
		echo __CLASS__.'::'.__FUNCTION__.'<br>';
		echo "</div>"; // .config-container
	}
}
?>