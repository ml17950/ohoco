<?php	// last change: 2018-11-26
class clsOHoCo {
	var $config;
	var $log_items;
	var $status;
	var $status_has_changed;
	var $config_has_changed;
	var $client;

	function __construct($read_config = true) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';

		$this->config = array();
		$this->status = array();
		$this->log_items = array();
		$this->status_has_changed = false;
		$this->config_has_changed = false;
		$this->client = CLIENT;

		if ($read_config) {
			$this->config_read_from_ram();
			if (!isset($this->config['core']['config-saved']))
				$this->config_read_from_file();
		}

		// update internal (virtual) sensors
		if (isset($this->config['sensor']['']))
			unset($this->config['sensor']['']);
		$this->config['sensor']['time']['type'] = 'time';
		$this->config['sensor']['time']['name'] = TXTTIME;
		$this->config['sensor']['time']['value'] = date('H:i');
		$this->config['sensor']['time']['change'] = time();
		$this->config['sensor']['day']['type'] = 'date';
		$this->config['sensor']['day']['name'] = 'Tag';
		$this->config['sensor']['day']['value'] = date('D');
		$this->config['sensor']['day']['change'] = time();
		$this->config['sensor']['weekend']['type'] = 'switch';
		$this->config['sensor']['weekend']['name'] = 'Wochenende';
		if ((date('N') > 5) && (date('N') < 8))
			$this->config['sensor']['weekend']['value'] = 'on';
		else
			$this->config['sensor']['weekend']['value'] = 'off';
		$this->config['sensor']['weekend']['change'] = time();
	}

	function __destruct() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		
// 		unset($this->config['types']);
// 		unset($this->config['sensor']['']);
// 		$this->config_has_changed = true;
		
		if ($this->config_has_changed) {
			if ((time() - intval($this->config['core']['config-saved'])) > 3600) {
				$this->config['core']['config-saved'] = time();
				$this->config_write_to_file();
// 				echo "\n\n\n\n\nXXX WRITE TO FILE XXX\n\n\n\n\n\n";
			}
			$this->config_write_to_ram();
// 			usleep(500000);
// 			echo "xxx write to ram xxx\n";
		}
// 		if ($this->status_has_changed)
// 			$this->status_write_to_file();
	}

	function debug() {
// 		echo highlight_string(print_r($this->status, true));
		echo highlight_string(print_r($this->config, true));
	}

	function log($section, $id, $value, $info = '') {
		$line = time()."\t".$this->client."\t".$section."\t".$id."\t".$value."\t".$info;
		file_put_contents(ABSPATH.'/config/ohoco.log', $line."\r\n", FILE_APPEND);
	}

	function log_read() {
		$file = ABSPATH.'/config/ohoco.log';

		if (!is_file($file)) {
			file_put_contents($file, time()."\t".$this->client."\tconfig\tLogfile\tcreated\t\r\n", FILE_APPEND);
			chmod($file, CHMODMASK);
			return array();
		}

		$lines = file($file);
		krsort($lines);

		$arr = array();
		$cnt = 0;
		foreach ($lines as $line) {
			if (!empty($line)) {
				$fields = explode("\t", trim($line));

				$arr[$cnt]['time']		= $fields[0];
				$arr[$cnt]['client']	= $fields[1];
				$arr[$cnt]['section']	= $fields[2];
				$arr[$cnt]['id']		= $fields[3];
				$arr[$cnt]['value']		= $fields[4];
				$arr[$cnt]['info']		= $fields[5];
				$cnt++;
			}
		}

		$this->log_items = $arr;
	}

	function log_resize() {
		$file = ABSPATH.'/config/ohoco.log';
		$lines = file($file); // reads the file into an array by line
		$keep = array_slice($lines, (MAXLOG * -1), MAXLOG); // keep the last n elements of the array
		file_put_contents($file, implode("", $keep)); // combine array and write it back to file
		$this->log('cronjob', 'Resize Logfile', 'executed');
	}

	function config_read_from_file() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';

		$file = ABSPATH.'/config/config.ser';
		if (file_exists($file)) {
			$serialized_data = file_get_contents($file);
			$this->config = unserialize($serialized_data);
		}
	}

	function config_write_to_file() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';

		$file = ABSPATH.'/config/config.ser';
		$serialized_data = serialize($this->config);
		file_put_contents($file, $serialized_data);
		chmod($file, CHMODMASK);

		$backup_file = $file.'.kw'.date('W');
		file_put_contents($backup_file, $serialized_data);
		chmod($backup_file, CHMODMASK);
	}

	function config_read_from_ram() {
		$shm_key = ftok(__FILE__, 'A');
		$shm_reshandle = shm_attach($shm_key, 2097152, 0666);
		$var = 1;
		if (shm_has_var($shm_reshandle, $var))
			$data = (array) shm_get_var($shm_reshandle, $var);
		else
			$data = array();
		$this->config = $data['ohoco'];
		shm_detach($shm_reshandle);
	}

	function config_write_to_ram() {
		/*
			ipcs -m --human
			ipcrm -m %id%
		*/
		$shm_key = ftok(__FILE__, 'A');
// 		$shm_semaphor = sem_get($shm_key, 1024, 0644 | IPC_CREAT);
		$shm_reshandle = shm_attach($shm_key, 2097152, 0666);
		$var = 1;
// 		if (shm_has_var($shm_reshandle, $var)) {
// 		    // If so, read the data
// 		    $data = (array) shm_get_var($shm_reshandle, $var);
// 		} else {
// 		    // If the data was not there.
// 		    $data = array();
// 		}
// 		sem_acquire($shm_semaphor); // Put a lock
		// Save the in the resulting array value
		$data['ohoco'] = $this->config;
		shm_put_var($shm_reshandle, $var, $data); 
// 		sem_release($shm_semaphor); // Releases the lock
		shm_detach($shm_reshandle);
	}

	function status_read_from_file() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';

		$file = ABSPATH.'/config/status.ser';
		if (file_exists($file)) {
			$serialized_data = file_get_contents($file);
			$this->status = unserialize($serialized_data);
		}
	}

	function status_write_to_file() {
		echo __CLASS__.'::'.__FUNCTION__.'<br>';

		$file = ABSPATH.'/config/status.ser';
		$serialized_data = serialize($this->status);
		file_put_contents($file, $serialized_data);
		chmod($file, CHMODMASK);
	}

	function device_register($device_id, $values) {
		echo __CLASS__.'::'.__FUNCTION__.'<br>';

		$items = explode(':', $values);
		print_r($items);
	}

	function device_unregister($device_id) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		foreach ($this->config['switch'] as $switch_id => $switch_info) {
			if ($switch_info['device'] == $device_id) {
				if (isset($this->config['sensor'][$switch_id])) {
					$this->log('config', $switch_id, 'Sensor removed');
					unset($this->config['sensor'][$switch_id]);
				}
				if (isset($this->config['switch'][$switch_id])) {
					$this->log('config', $switch_id, 'Switch removed');
					unset($this->config['switch'][$switch_id]);
				}
			}
		}
		foreach ($this->config['notify'] as $notify_id => $notify_array) {
			if (count($notify_array) > 0) {
				foreach ($notify_array as $notify_device_id => $subscribed) {
					if ($notify_device_id == $device_id) {
						$this->log('config', $device_id, 'device from notify '.$notify_device_id.' removed');
						unset($this->config['notify'][$notify_id][$notify_device_id]);
					}
				}
			}
		}
		$this->log('config', $device_id, 'Device removed');
		unset($this->config['device'][$device_id]);
		$this->config_has_changed = true;
	}

	function device_set_info($device_id, $type, $value) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		if ($this->config['device'][$device_id][$type] != $value) {
			// if value has changed, update config
			$this->config['device'][$device_id][$type] = $value;
			if ($type == 'connect') {
				$this->log('device', $device_id, 'connected');
			}
			if ($type == 'ip') {
				$this->config['device'][$device_id]['protocol'] = 'UDP';
				$this->config['device'][$device_id]['timeout'] = 3630;
			}
			if (($type == 'uptime') || ($type == 'connect')) {
				$this->config['device'][$device_id]['alive'] = time();
				if (!empty($_SERVER['REMOTE_ADDR']))
					$this->config['device'][$device_id]['ip'] = $_SERVER['REMOTE_ADDR'];
			}
			$this->config_has_changed = true;
		}
		return true;
	}

	function device_set_config($device_id, $config_array) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$config_array['upd'] = time();
		$this->config['device'][$device_id]['config'] = $config_array;
		$this->config_has_changed = true;
		return true;
	}

	function device_send_udp_command($remote_ip, $remote_port, $command) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		if ($socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP)) {
			socket_sendto($socket, $command, strlen($command), 0, $remote_ip, $remote_port);
			return true;
		}
		return false;
	}

	function device_send_control_command($device_id, $command) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		if (array_key_exists($device_id, $this->config['device'])) {
			$protocol = $this->config['device'][$device_id]['protocol'];
			switch ($protocol) {
				case 'MQTT':
					include_once('class.mqtt.php');
					$topic = 'ohoco/device/'.$device_id.'/callback';
					$MQTT = new phpMQTT(MQTT_BROKER_ADDR, MQTT_BROKER_PORT, 'MqttPubRelay');
					if ($MQTT->connect(true, NULL, MQTT_USERNAME, MQTT_PASSWORD)) {
						$MQTT->publish($topic, $command, 1, false);
						$MQTT->close();
						$this->log('config', $device_id, $command, $topic);
					}
					else {
					    $this->log('config', $device_id, 'MQTT timeout');
					}
					break;
				default:
					$remote_ip = $this->config['device'][$device_id]['ip'];
					$remote_port = 18266;
					if ($this->device_send_udp_command($remote_ip, $remote_port, $command))
						$this->log('config', $device_id, $command, $remote_ip.":".$remote_port);
					else
						$this->log('config', $device_id, 'Cant create UDP socket');
			}
		}
		return true;
	}

// 	function sensor_set_status($sensor_id, $value) {
// 		echo __CLASS__.'::'.__FUNCTION__.'<br>';
// 
// 		$this->status[$sensor_id]['value'] = $value;
// 		$this->status[$sensor_id]['change'] = time();
// 		$this->status_has_changed = true;
// 	}
// 
// 	function sensor_get_status($sensor_id) {
// 		echo __CLASS__.'::'.__FUNCTION__.'<br>';
// 
// 		return $this->status[$sensor_id]['value'];
// 	}
// 
// 	function sensor_set_name($sensor_id, $value) {
// 		echo __CLASS__.'::'.__FUNCTION__.'<br>';
// 
// 		$this->config['name'][$sensor_id] = $value;
// 		$this->config_has_changed = true;
// 	}

	function sensor_register($sensor_id, $values) {
		echo __CLASS__.'::'.__FUNCTION__.'<br>';

// 		$items = explode(':', $values);
// 		$this->config['type'][$sensor_id] = $items[0]; // sensor-type
// 		$this->config['unit'][$sensor_id] = $items[1]; // sensor-unit
// 		$this->config_has_changed = true;
	}

	function sensor_unregister($sensor_id) {
		if (isset($this->config['sensor'][$sensor_id])) {
			$this->log('config', $sensor_id, 'Sensor removed');
			unset($this->config['sensor'][$sensor_id]);
		}
		if (isset($this->config['switch'][$sensor_id])) {
			$this->log('config', $sensor_id, 'Switch removed');
			unset($this->config['switch'][$sensor_id]);
		}
		$this->config_has_changed = true;
	}

	function sensor_set_type($sensor_id, $value) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		if ($this->config['sensor'][$sensor_id]['type'] != $value) {
			// if value has changed, update config
			$this->config['sensor'][$sensor_id]['type'] = $value;
			if (!isset($this->config['sensor'][$switch_id]['value']))
				$this->config['sensor'][$switch_id]['value'] = 'null';
			$this->config_has_changed = true;
		}
		return true;
	}

	function sensor_set_unit($sensor_id, $value) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		if ($this->config['sensor'][$sensor_id]['unit'] != $value) {
			// if value has changed, update config
			$this->config['sensor'][$sensor_id]['unit'] = $value;
			$this->config_has_changed = true;
		}
		return true;
	}

	function sensor_set_name($sensor_id, $value) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		if ($this->config['sensor'][$sensor_id]['name'] != $value) {
			// if value has changed, update config
			$this->config['sensor'][$sensor_id]['name'] = $value;
			$this->config_has_changed = true;
		}
		return true;
	}

	function sensor_set_value($sensor_id, $value) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		if ($this->config['sensor'][$sensor_id]['value'] != $value) {
			// if value has changed, update config
			$this->config['sensor'][$sensor_id]['value'] = $value;
			$this->config['sensor'][$sensor_id]['change'] = time();
			$this->config_has_changed = true;
		}
		$this->log('status', $sensor_id, $value);
		return true;
	}

	function switch_register($switch_id, $values, $protocol, $on_url = '', $off_url = '') {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$items = explode(':', $values);
		$type = $items[0];
		$device_id = $items[1];
		if ($this->config['sensor'][$switch_id]['type'] != $type) {
			// if value has changed, update config
			$this->config['sensor'][$switch_id]['type'] = $type;
			if (!isset($this->config['sensor'][$switch_id]['value']))
				$this->config['sensor'][$switch_id]['value'] = 'off';
			$this->config_has_changed = true;
		}

unset($this->config['switch'][$switch_id]);

// 		if (!empty($on_url))
// 			$this->config['sensor'][$switch_id]['on_url'] = "".$on_url;
// 		if (!empty($off_url))
// 			$this->config['sensor'][$switch_id]['off_url'] = "".$off_url;
// 
// 		if ($this->config['switch'][$switch_id] != $protocol.$device_id) {
// 			// if value has changed, update config
// 			$this->config['switch'][$switch_id] = $protocol.$device_id;
// 			$this->config_has_changed = true;
// 		}

		$this->config['switch'][$switch_id]['device'] = $device_id;
		$this->config['switch'][$switch_id]['protocol'] = $protocol;
		$this->config['switch'][$switch_id]['on_url'] = $on_url;
		$this->config['switch'][$switch_id]['off_url'] = $off_url;
		$this->config_has_changed = true;
		
		return true;
	}

	function switch_turn_on($switch_id) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		switch ($this->config['switch'][$switch_id]['protocol']) {
			case 'UDP':
				$device_id = $this->config['switch'][$switch_id]['device'];
				$remote_ip = $this->config['device'][$device_id]['ip'];
				$remote_port = 18266;
				$command = $switch_id.':on';
				if ($this->device_send_udp_command($remote_ip, $remote_port, $command))
					$this->log('config', $device_id, $command, $remote_ip.":".$remote_port);
				else {
					$this->log('config', $device_id, 'Cant create UDP socket');
					return false;
				}
				break;
			case 'HTTP':
				$this->helper_switch_http($switch_id, 'on', $this->config['switch'][$switch_id]['on_url']);
				usleep(500000);
				break;
			case 'MQTT':
				if ($this->helper_send_mqtt_message('ohoco/switch/'.$switch_id, 'on')) {
					usleep(500000);
					$this->log('status', $switch_id, 'on');
				}
				else {
		    		$this->log('error', $switch_id, 'Cant connect to MQTT broker');
		    		return false;
		    	}
				break;
		}
		return true;
	}

	function switch_turn_off($switch_id) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		switch ($this->config['switch'][$switch_id]['protocol']) {
			case 'UDP':
				$device_id = $this->config['switch'][$switch_id]['device'];
				$remote_ip = $this->config['device'][$device_id]['ip'];
				$remote_port = 18266;
				$command = $switch_id.':off';
				if ($this->device_send_udp_command($remote_ip, $remote_port, $command))
					$this->log('config', $device_id, $command, $remote_ip.":".$remote_port);
				else
					$this->log('config', $device_id, 'Cant create UDP socket');
	//			usleep(500000);
				break;
			case 'HTTP':
				$this->helper_switch_http($switch_id, 'off', $this->config['switch'][$switch_id]['off_url']);
				usleep(500000);
				break;
			case 'MQTT':
				if ($this->helper_send_mqtt_message('ohoco/switch/'.$switch_id, 'off')) {
					usleep(500000);
					$this->log('status', $switch_id, 'off');
				}
				else {
		    		$this->log('error', $switch_id, 'Cant connect to MQTT broker');
		    		return false;
		    	}
				break;
		}
		return true;
	}

	function switch_toggle_onoff($switch_id) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		if ($this->config['sensor'][$switch_id]['value'] == 'off')
			return $this->switch_turn_on($switch_id);
		else
			return $this->switch_turn_off($switch_id);
	}

	function trigger_register($trigger_id, $values) {
		echo __CLASS__.'::'.__FUNCTION__.'<br>';
	}

	function trigger_activate($trigger_id, $payload) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$this->log('trigger', $trigger_id, 'activated');
		$this->client = 'trigger';
		if (array_key_exists($trigger_id, $this->config['trigger'])) {
			if (count($this->config['trigger'][$trigger_id]['actions']) > 0) {
				foreach ($this->config['trigger'][$trigger_id]['actions'] as $n => $action) {
					debugarr($action);
					switch ($action['type']) {
						case 'SWITCH':
							if ($action['val'] == 'on')
								$this->switch_turn_on($action['id']);
							else
								$this->switch_turn_off($action['id']);
							break;
						case 'NOTIFY':
							if ($action['id'] == 'telegram')
								$this->helper_send_telegram_message($action['val']);
							else
								$this->notify_activate($action['id'], $action['val']);
							break;
						case 'COMMAND':
							break;
						case 'MQTT':
							break;
					}
				}
			}
			$this->config['trigger'][$trigger_id]['fired'] = time();
			$this->config_has_changed = true;
		}
		else {
			$this->config['trigger'][$trigger_id]['fired'] = time();
			$this->config['trigger'][$trigger_id]['name'] = $trigger_id;
			$this->config['trigger'][$trigger_id]['actions'] = array();
			$this->config_has_changed = true;
		}
		return true;
	}

	function notify_register($notify_id, $device_id) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$this->config['notify'][$notify_id][$device_id] = time();
		$this->config_has_changed = true;
		return true;
	}

	function notify_activate($notify_id, $message) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$this->log('notify', $notify_id, 'activated');
		if (array_key_exists($notify_id, $this->config['notify'])) {
			if (count($this->config['notify'][$notify_id]) > 0) {
				foreach ($this->config['notify'][$notify_id] as $device_id => $subscription_time) {
					$this->device_send_control_command($device_id, 'notify:'.$message);
				}
			}
		}
		return true;
	}

	function rules_check_single_condition($type, $sensor_id, $is_value, $check_value) {
		$is_value_float = floatval(preg_replace('/[^0-9.,]+/', '', $is_value));
		$check_value_float  = floatval(preg_replace('/[^0-9.,]+/', '', $check_value));
		if ($type == 'OLD')
			$is_value = $this->config['sensor'][$sensor_id]['change'];

		switch ($type) {
			case 'EQU': if ($is_value == $check_value) { return true; } break;
			case 'NEQ': if ($is_value != $check_value) { return true; } break;
			case 'LSS': if ($is_value_float < $check_value_float) { return true; } break;
			case 'LEQ': if ($is_value_float <= $check_value_float) { return true; } break;
			case 'GTR': if ($is_value_float > $check_value_float) { return true; } break;
			case 'GEQ': if ($is_value_float >= $check_value_float) { return true; } break;
			case 'OLD': if ((time() - $is_value) > $check_value) { return true; } break;
		}
		return false;
	}

	function rules_check_conditions() {
		if (count($this->config['rules']) > 0) {
			$this->client = 'rule';
			foreach ($this->config['rules'] as $rid => $rule) {
				if ($rule['active'] == 1) {
					$cond2check = count($rule['conditions']);
					$cond2run = 0;

					if ($cond2check > 0) {
						foreach ($rule['conditions'] as $cid => $condition) {
							if ($this->rules_check_single_condition($condition['type'], $condition['status'], $this->config['sensor'][$condition['status']]['value'], $condition['value']))
								$cond2run++;
// 							echo $cond2run,"<br>";

							if ($cond2run >= $cond2check) {
								$this->log('cronjob', $rule['name'], 'executed');

								if (!empty($rule['actions']['trigger']) && ($rule['actions']['trigger'] != '-')) {
									$this->trigger_activate($rule['actions']['trigger'], 'fire');
// 									echo "{{FIRE TRIGGER :: ",$rule['actions']['trigger'],"}}<br>";
								}
								if (!empty($rule['actions']['status']['id']) && ($rule['actions']['status']['id'] != '-')) {
									$this->sensor_set_value($rule['actions']['status']['id'], $rule['actions']['status']['value']);
// 									echo "{{SET ",$rule['actions']['status']['id']," &rarr; ",$rule['actions']['status']['value'],"}}<br>";
								}
							}
						}
					}
// 					echo "\$cond2check: ",$cond2check," / \$cond2run: ",$cond2run,"<br>";
// 					echo "<hr>";
				}
// 				else
// 					echo "RULE-DBG: rule ",$rid," not active<br>";
			}
		}
	}

	function plugins_load() {
		$files = @glob(ABSPATH.'/plugins/*.php');
		if (is_array($files) && (count($files) > 0)) {
			foreach ($files as $file) {
				include_once($file);
			}
		}
	}

	function plugins_get_config($plugin_id, $key) {
		if (isset($this->config['plugin'][$plugin_id][$key]))
			return $this->config['plugin'][$plugin_id][$key];
		else
			return false;
	}

	function plugins_set_config($plugin_id, $key, $val) {
		$this->config['plugin'][$plugin_id][$key] = $val;
		$this->config_has_changed = true;
	}

	function helper_parse_message_variables($message) {
		preg_match_all("/%(.*?)%/", $message, $matches);
		if (is_array($matches[0]) && (count($matches[0]) > 0)) {
			foreach ($matches[0] as $m => $match) {
				if (array_key_exists($matches[1][$m], $this->config['sensor']))
					$replacement = time_diff(time() - $this->config['sensor'][$matches[1][$m]]['change'], false);
				else
					$replacement = '';
				$message = str_replace($match, $replacement, $message);
			}
		}
		preg_match_all("/@(.*?)@/", $message, $matches);
		if (is_array($matches[0]) && (count($matches[0]) > 0)) {
			foreach ($matches[0] as $m => $match) {
				if (array_key_exists($matches[1][$m], $this->config['sensor'])) {
					if (!empty($this->config['sensor'][$matches[1][$m]]['unit']))
						$replacement = $this->config['sensor'][$matches[1][$m]]['value'].' '.$this->config['sensor'][$matches[1][$m]]['unit'];
					else
						$replacement = $this->config['sensor'][$matches[1][$m]]['value'];
				}
				else
					$replacement = '';
				$message = str_replace($match, $replacement, $message);
			}
		}
		return $message;
	}

	function helper_send_telegram_message($message) {
		$message_to_send = $this->helper_parse_message_variables($message);

		include_once('class.telegram.php');
		$telegram = new clsTelegram(TELEGRAM_API_KEY);
		$telegram->sendMessage(TELEGRAM_CHAT_ID, $message_to_send);
		$this->log('notify', 'Telegram Message', 'sent');
	}

	function helper_send_mqtt_message($topic, $payload) {
		include_once('class.mqtt.php');
		$MQTT = new phpMQTT(MQTT_BROKER_ADDR, MQTT_BROKER_PORT, MQTT_CLIENT_NAME);
		if ($MQTT->connect(true, NULL, MQTT_USERNAME, MQTT_PASSWORD)) {
			$MQTT->publish($topic, $payload, 1, false);
			$MQTT->close();
			return true;
		}
		return false;
	}

	function helper_switch_http($switch_id, $new_status, $url) {
		$response = '';
		$urlarr = parse_url($url);
		if (!isset($urlarr['port']))
			$urlarr['port'] = 80;
		$url = $urlarr['scheme'].'://'.$urlarr['host'].$urlarr['path'];
		if (!empty($urlarr['query']))
			$url .= '?'.$urlarr['query'];
		if (!empty($urlarr['user'])) {
			$context = stream_context_create(array(
			    'http' => array('header'  => "Authorization: Basic ".base64_encode($urlarr['user'].':'.$urlarr['pass']))
			));
			$response = file_get_contents($url, false, $context);
		}
		else
			$response = file_get_contents($url, false);

		if ($response === false) {
			$this->log('error', $this->config['names'][$switch_id].' : switch_change_http failed', 'error');
			echo 'ERR';
		}
		else {
			$this->sensor_set_value($switch_id, $new_status);
// 			echo $new_status;
		}
	}

	function helper_status2number($status) {
// 		echo "((",$status,"))&rarr;";
		switch ($status) {
			case 'on':
			case 'yes':
			case 'open':
			case 'home':
			case 'here':
			case 'working':
				$ret = 1;
				break;
			case 'off':
			case 'no':
			case 'closed':
			case 'away':
			case 'idle':
			case 'charging':
				$ret = 0;
				break;
			default:
				$ret = str_replace(',','.',$status);
				$ret = preg_replace("/[^0-9.]/", '', $ret);
		}
// 		echo "[[",$ret,"]]";
		return floatval($ret);
	}
}
?>