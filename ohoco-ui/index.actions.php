<?php	// last change: 2018-10-16
	$do = param('do');

	switch ($view) {
		case 'config':
			if ($do == 'save-systemcfg') {
				for ($i=0; $i<9; $i++) {
					$ui->ohoco->config['widgets'][$i] = $_POST['widget'][$i];
				}
				$ui->ohoco->config_has_changed = true;
				header('Location: index.php?view=config');
				exit;
			}
			elseif ($do == 'sendcmd') {
				$id = param('id');
				$cmd = param('cmd');
				$ui->ohoco->device_send_control_command($id, $cmd);
// 				if ($view != 'overview') {
// 					usleep(500000); // wait for 0,5 seconds
// 					header('Location: config.php?view='.$view.'&id='.$id);
// 				}
// 				else
					header('Location: index.php?view=config');
				exit;
			}
			elseif ($do == 'delete') {
				$id = param('id');
				$ui->ohoco->device_unregister($id);
				header('Location: index.php?view=config');
				exit;
			}
			break;

		case 'config-edit':
			if ($do == 'xxxx') {
				$id = param('id');
				$cmd = param('cmd');
// 				$ui->ohoco->device_send_control_command($id, $cmd);
			}
			break;

		case 'sensors':
			if ($do == 'delete') {
				$id = param('id');
				$ui->ohoco->sensor_unregister($id);
				header('Location: index.php?view=sensors');
				exit;
			}
			break;

		case 'sensor-edit':
			if ($do == 'save-sensor') {
				$id = param('id');
				$newname = param('newname');
				$newvalue = param('newvalue');
				$ui->ohoco->sensor_set_name($id, $newname);
				$ui->ohoco->sensor_set_value($id, $newvalue);
				header('Location: index.php?view=sensors');
				exit;
			}
			break;

		case 'trigger':
			if ($do == 'activate') {
				$id = param('id');
				$ui->ohoco->trigger_activate($id, 'fire');
				header('Location: index.php?view=trigger');
				exit;
			}
			break;

		case 'trigger-edit':
			if ($do == 'save-trigger') {
				$id = param('id');
				$newname = param('newname');
// unset($ui->ohoco->config['trigger']['']);
				$ui->ohoco->config['trigger'][$id]['name'] = $newname;
				$ui->ohoco->config['trigger'][$id]['actions'] = array();

				$action_counter = 0;
				if (isset($_POST['switch_id']) && (count($_POST['switch_id']) > 0)) {
					foreach ($_POST['switch_id'] as $n => $switch_id) {
						if ($switch_id != '-') {
							$ui->ohoco->config['trigger'][$id]['actions'][$action_counter]['type'] = 'SWITCH';
							$ui->ohoco->config['trigger'][$id]['actions'][$action_counter]['id'] = $switch_id;
							$ui->ohoco->config['trigger'][$id]['actions'][$action_counter]['val'] = $_POST['switch_state'][$n];
							$action_counter++;
						}
					}
				}
				if (isset($_POST['notify_type']) && (count($_POST['notify_type']) > 0)) {
					foreach ($_POST['notify_type'] as $n => $notify_id) {
						if ($notify_id != '-') {
							$ui->ohoco->config['trigger'][$id]['actions'][$action_counter]['type'] = 'NOTIFY';
							$ui->ohoco->config['trigger'][$id]['actions'][$action_counter]['id'] = $notify_id;
							$ui->ohoco->config['trigger'][$id]['actions'][$action_counter]['val'] = $_POST['notify_text'][$n];
							$action_counter++;
						}
					}
				}
				$ui->ohoco->config_has_changed = true;
// debugarr($_POST);
// debugarr($ui->ohoco->config['trigger'][$id]['actions']);
// exit;
				header('Location: index.php?view=trigger-edit&id='.$id);
				exit;
			}
			break;

		case 'rules':
			if ($do == 'activate-rule') {
				$id = param('id');
				$ui->ohoco->config['rules'][$id]['active'] = 1;
				$ui->ohoco->config_has_changed = true;
				header('Location: index.php?view=rules');
				exit;
			}
			elseif ($do == 'deactivate-rule') {
				$id = param('id');
				$ui->ohoco->config['rules'][$id]['active'] = 0;
				$ui->ohoco->config_has_changed = true;
				header('Location: index.php?view=rules');
				exit;
			}
			elseif ($do == 'create-rule') {
				$id = get_unique_id();
// unset($ui->ohoco->config['rules']);
				$ui->ohoco->config['rules'][$id]['name'] = $id;
				$ui->ohoco->config['rules'][$id]['active'] = 0;
				$ui->ohoco->config['rules'][$id]['conditions'] = array();
				$ui->ohoco->config['rules'][$id]['actions'] = array();
				$ui->ohoco->config_has_changed = true;

				header('Location: index.php?view=rule-edit&id='.$id);
				exit;
			}
			break;

		case 'rule-edit':
			if ($do == 'save-rule') {
				$id = param('ruleid');

				$ui->ohoco->config['rules'][$id]['name'] = param('rulename');

				for ($cond_index=1; $cond_index<4; $cond_index++) {
					if (($_POST['cond_status_'.$cond_index] != '-') && ($_POST['cond_type_'.$cond_index] != '-')) {
						$ui->ohoco->config['rules'][$id]['conditions'][$cond_index]['status'] = param('cond_status_'.$cond_index);
						$ui->ohoco->config['rules'][$id]['conditions'][$cond_index]['type'] = param('cond_type_'.$cond_index);
						$ui->ohoco->config['rules'][$id]['conditions'][$cond_index]['value'] = param('cond_value_'.$cond_index);
					}
					else
						unset($ui->ohoco->config['rules'][$id]['conditions'][$cond_index]);
				}

				if (!empty($_POST['triggeridnew'])) {
					$trigger_id = param('triggeridnew');
					$ui->ohoco->config['rules'][$id]['actions']['trigger'] = $trigger_id;
					$ui->ohoco->config['trigger'][$trigger_id]['fired'] = 0;
					//$ui->ohoco->config['trigger'][$trigger_id]['name'] = '';
					$ui->ohoco->config['trigger'][$trigger_id]['actions'] = array();
				}
				else
					$ui->ohoco->config['rules'][$id]['actions']['trigger'] = param('triggeridold');

				if (!empty($_POST['statusidnew']))
					$sensor_id = param('statusidnew');
				else
					$sensor_id = param('statusid');

				$ui->ohoco->config['rules'][$id]['actions']['status']['id'] = $sensor_id;
				$ui->ohoco->config['rules'][$id]['actions']['status']['value'] = param('statusval');

				if (!isset($ui->ohoco->config['sensor'][$sensor_id])) {
					$ui->ohoco->config['sensor'][$sensor_id]['value'] = '-';
					$ui->ohoco->config['sensor'][$sensor_id]['change'] = time();
				}

				$ui->ohoco->config_has_changed = true;
				header('Location: index.php?view=rule-edit&id='.$id);
				exit;
			}
			break;
	}

			if (count($_POST) > 0) {
				debugarr($_POST);
				exit;
			}
// 			if (count($_GET) > 0) {
// 				debugarr($_GET);
// 				exit;
// 			}
?>