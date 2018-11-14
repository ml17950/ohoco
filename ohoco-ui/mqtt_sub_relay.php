<?php	// last change: 2018-11-14
/*
	https://github.com/bluerhinos/phpMQTT
	nohup /usr/bin/php /var/www/html/ohoco/mqtt_sub_relay.php >/dev/null 2>&1 &
	ps -axu | grep mqtt_msg_relay.php

	https://www.phpclasses.org/blog/post/276-PHP-IPC-with-Daemon-Service-using-Message-Queues-Shared-Memory-and-Semaphores.html
*/
// 	ignore_user_abort(1);
// 	set_time_limit(0);

	include_once('_sysconfig.php');
	include_once('lib/common.php');
	include_once('lib/class.ohoco.php');
	include_once('lib/class.mqtt.php');

	define('MQTT_CLIENT_NAME',	'OHoCoMqttSubRelay');

	$ohoco = new clsOHoCo();
	$ohoco->client = 'mqtt';

 	$topics['ohoco/#'] = array('qos' => 0, 'function' => 'onMessageReceived');
//	$topics['#'] = array('qos' => 0, 'function' => 'onMessageReceived');

	// =========================================================================
	// =========================================================================

	echo date('d.m.y H:i:s')," mqtt_sub_relay.php running\n";

	$topic_payloads = array();

	$MQTT = new phpMQTT(MQTT_BROKER_ADDR, MQTT_BROKER_PORT, MQTT_CLIENT_NAME);

	if(!$MQTT->connect(true, NULL, MQTT_USERNAME, MQTT_PASSWORD)) {
		exit(1);
	}

	echo date('d.m.y H:i:s')," connected... waiting for messages\n";

	$MQTT->subscribe($topics, 1);
	while($MQTT->proc()){
	}
	$MQTT->close();

	// =========================================================================
	// =========================================================================

	function onMessageReceived($topic, $payload) {
		global $ohoco;

		if (substr($topic, 0, 13) == 'ohoco/device/') {
			$p = strpos($topic, '/', 13);
			$device_id = substr($topic, 13, $p-13);
			$device_value = substr($topic, $p+1);

			if ($device_value != 'callback') {
				$ohoco->config_read_from_ram();
				$ohoco->device_set_info($device_id, $device_value, $payload);
				if ($device_value == 'ip')
					$ohoco->device_set_info($device_id, 'protocol', 'MQTT'); // overwrite value
				$ohoco->config_write_to_ram();
			}

// 			$logmsg = date('d.m.y H:i:s')." # DEVICE  # ".str_pad($device_id, 20)." :: ".str_pad($device_value, 10)." -> ".$payload."\n";
		}
		elseif (substr($topic, 0, 13) == 'ohoco/sensor/') {
			$p = strpos($topic, '/', 13);
			$sensor_id = substr($topic, 13, $p-13);
			$sensor_value = substr($topic, $p+1);

			$ohoco->config_read_from_ram();
			switch ($sensor_value) {
				case 'value':	$ohoco->sensor_set_value($sensor_id, $payload);	break;
				case 'type':	$ohoco->sensor_set_type($sensor_id, $payload);	break;
				case 'unit':	$ohoco->sensor_set_unit($sensor_id, $payload);	break;
			}
			$ohoco->config_write_to_ram();

// 			$logmsg = date('d.m.y H:i:s')." # SENSOR  # ".str_pad($sensor_id, 20)." :: ".str_pad($sensor_value, 10)." -> ".$payload."\n";
		}
		elseif (substr($topic, 0, 14) == 'ohoco/connect/') {
			$device_id = substr($topic, 14);

			$ohoco->config_read_from_ram();
			$ohoco->device_set_info($device_id, 'connect', $payload);
			$ohoco->config_write_to_ram();

// 			$logmsg = date('d.m.y H:i:s')." # CONNECT # ".str_pad($device_id, 20)." ::            -> ".$payload."\n";
		}
		elseif (substr($topic, 0, 13) == 'ohoco/config/') {
			$device_id = substr($topic, 13);
			$config_array = json_decode($payload, true);
			$ohoco->config_read_from_ram();
			foreach ($config_array as $key => $val) {
				$ohoco->config['device'][$device_id]['config'][$key] = $val;
			}
			$ohoco->config['device'][$device_id]['config']['upd'] = time();
			$ohoco->config_write_to_ram();

// 			$logmsg = date('d.m.y H:i:s')." # CONFIG  # ".str_pad($device_id, 20)." ::            -> ".$payload."\n";
		}
		elseif (substr($topic, 0, 13) == 'ohoco/switch/') {
			$switch_id = substr($topic, 13);

			$ohoco->config_read_from_ram();
			switch ($payload) {
				case 'on':
				case 'off':
				case 'toggle':
					// ignore command. will be executed by node
					break;
				// register
				default:
					if (strpos($payload, ':') !== false) {
						// register command
						echo "REG msg\n";
						$ohoco->switch_register($switch_id, $payload, 'MQTT'); break;
						$ohoco->config_write_to_ram();
					}
					break;
			}

// 			$logmsg = date('d.m.y H:i:s')." # SWITCH  # ".str_pad($switch_id, 20)." ::            -> ".$payload."\n";
		}
		elseif (substr($topic, 0, 14) == 'ohoco/trigger/') {
			$trigger_id = substr($topic, 14);

			$ohoco->config_read_from_ram();
			$ohoco->trigger_activate($trigger_id, $payload);
			$ohoco->config_write_to_ram();

// 			$logmsg = date('d.m.y H:i:s')." # TRIGGER # ".str_pad($trigger_id, 20)." ::            -> ".$payload."\n";
		}
		elseif (substr($topic, 0, 13) == 'ohoco/notify/') {
			$notify_id = substr($topic, 13);

			$ohoco->config_read_from_ram();
			$ohoco->notify_activate($notify_id, $payload);
			$ohoco->config_write_to_ram();

// 			$logmsg = date('d.m.y H:i:s')." # NOTIFY  # ".str_pad($notify_id, 20)." ::            -> ".$payload."\n";
		}
		elseif (substr($topic, 0, 10) == 'ohoco/log/') {
			$device_id = substr($topic, 10);

			$ohoco->log('error', $device_id, $payload);

// 			$logmsg = date('d.m.y H:i:s')." # LOG     # ".str_pad($notify_id, 20)." ::            -> ".$payload."\n";
		}
		else {
			$logmsg = date('d.m.y H:i:s')." # ??????? # ".$topic." -> ".$payload."\n";
		}

		if (!empty($logmsg)) {
			echo $logmsg;
			file_put_contents('config/mqtt.log', $logmsg, FILE_APPEND);
		}

// 
// 		if (substr($topic, 0, 13) == 'ohoco/device/') {
// 			$device_id = substr($topic, 13);
// // 			$rift3->device_register($device_id, $payload, 'MQTT');
// 			$logmsg = date('d.m.y H:i:s')." # DEVICE  # ".$device_id." :: ".$payload."\n";
// 		}
// 		elseif (substr($topic, 0, 13) == 'ohoco/config/') {
// 			$device_id = substr($topic, 13);
// // 			$rift3->device_config_save($device_id, $payload);
// 			$logmsg = date('d.m.y H:i:s')." # CONFIG  # ".$device_id." is now ".$payload."\n";
// 		}
// 		elseif (substr($topic, 0, 11) == 'ohoco/ping/') {
// 			$device_id = substr($topic, 11);
// // 			$rift3->device_alive($device_id, $payload);
// 			$logmsg = date('d.m.y H:i:s')." # PING    # ".$device_id." is now ".$payload."\n";
// 		}
// 		elseif (substr($topic, 0, 14) == 'ohoco/connect/') {
// 			$device_id = substr($topic, 14);
// // 			$rift3->device_reconnected($device_id, $payload);
// 			$logmsg = date('d.m.y H:i:s')." # CONNECT # [".$payload."]\n";
// 		}
// 		elseif (substr($topic, 0, 12) == 'ohoco/debug/') {
// 			$logmsg = date('d.m.y H:i:s')." # DEBUG   # ".substr($topic, 12)." :: ".$payload."\n";
// 			echo $logmsg;
// 			$logmsg = '';
// 		}

	}

	function serialize_tpoic_array() {
		global $topic_payloads;

		$topic_tree = array();

		if (count($topic_payloads) > 0) {
			foreach ($topic_payloads as $t => $p) {
				$topic_fields = explode('/', $t);
				switch (count($topic_fields)) {
					case 3:
						$topic_tree[$topic_fields[1]][$topic_fields[2]] = $p;
						break;
					case 4:
						$topic_tree[$topic_fields[1]][$topic_fields[2]][$topic_fields[3]] = $p;
						break;
					default:
						echo $t," :: ",count($topic_fields),"\n";
				}
			}
		}

		return $topic_tree;
	}
?>