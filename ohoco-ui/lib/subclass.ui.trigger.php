<?php	// last change: 2018-10-16
class clsTriggerInterface {
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
		echo "<div class='js-trigger trigger-container'>";

		if (is_array($this->ohoco->config['trigger'])) {
			arsort($this->ohoco->config['trigger']);

			if (is_array($this->ohoco->config['trigger']) && (count($this->ohoco->config['trigger']) > 0)) {
				foreach ($this->ohoco->config['trigger'] as $trigger_id => $trigger) {
// 					$trigger_hash = md5($trigger_id);
					if (!empty($trigger['name']))
						$trigger_name = $trigger['name'];
					else
						$trigger_name = $trigger_id;

// 					if (array_key_exists($trigger_id, $this->ohoco->config['trigger'])) {
// 						$has_actions = true;
// 					}
// 					else {
// 						$has_actions = false;
// 					}

					echo "<div class='trigger-box'>";

					echo "<div class='trigger-icon'><img src='res/img/ui/trigger.png' width='24' height='24' alt='trigger' title='",$trigger_name,"'></div>";
					echo "<div class='trigger-name'>",$trigger_name,"</div>";

					echo "<div class='trigger-change'>",dtstr($trigger['fired']),"</div>";
					echo "<div class='trigger-options'><a href='index.php?view=trigger&id=",$trigger_id,"&do=activate'><img src='res/img/ui/activate.png' width='24' height='24' alt='activate' title='activate trigger now'></a></div>";
	 				echo "<div class='trigger-options'><a href='index.php?view=trigger-edit&id=",$trigger_id,"'><img src='res/img/ui/edit.png' width='24' height='24' alt='edit' title='edit'></a></div>";
// 					echo "<div class='trigger-options'><a href='index.php?view=trigger-edit&",$trigger_id,"&do=delete'><img src='res/img/ui/delete.png' width='24' height='24' alt='delete' title='delete'></a></div>";

					if (count($trigger['actions']) > 0) {
						echo "<div class='trigger-actions'>";
						foreach ($trigger['actions'] as $n => $action) {
							switch ($action['type']) {
								case 'SWITCH':	echo ACTSWITCH," "; break;
								case 'NOTIFY':	echo ACTNOTIFY," "; break;
								case 'COMMAND':	echo ACTCOMMAND," "; break;
								default: echo $action['type']," ";
							}
							if (!empty($this->ohoco->config['sensor'][$action['id']]['name']))
								$action_name = $this->ohoco->config['sensor'][$action['id']]['name'];
							else
								$action_name = $action['id'];
							echo $action_name," &rarr; ";
							switch ($action['val']) {
								case 'on':	echo TXTON; break;
								case 'off':	echo TXTOFF; break;
								default: echo $action['val'];
							}
							echo "<br>";
						}
						echo "</div>";
					}

					echo "</div>"; // .trigger-box
				}
			}
		}

// echo "<div class='debug'>";
// debugarr($this->ohoco->config['trigger']);
// debugarr($this->ohoco->config['notify']);
// echo "</div>";

		echo "</div>"; // .trigger-container
	}

	function edit_switch_row($preslected_id, $preslected_state, $can_be_removed) {
		if (count($this->ohoco->config['switch']) > 0) {
			echo "<label for='newname'>Schalte</label>";
			echo "<select name='switch_id[]' size='1'>";
			echo "<option value='-'>---</option>";
			foreach ($this->ohoco->config['switch'] as $switch_id => $device_item) {
				if (!empty($this->ohoco->config['sensor'][$switch_id]['name']))
					$switch_name = $this->ohoco->config['sensor'][$switch_id]['name'];
				else
					$switch_name = $switch_id;

				if ($switch_id == $preslected_id)
					echo "<option value='",$switch_id,"' selected>",$switch_name,"</option>";
				else
					echo "<option value='",$switch_id,"'>",$switch_name,"</option>";
			}
			echo "</select> ";
			$state_array = array('on','off');
			echo "<select name='switch_state[]' size='1'>";
			foreach ($state_array as $state) {
				if ($state == $preslected_state)
					echo "<option value='",$state,"' selected>",$state,"</option>";
				else
					echo "<option value='",$state,"'>",$state,"</option>";
			}
			echo "</select><br>";
		}
	}

	function edit_notify_row($preslected_id, $preslected_text, $can_be_removed) {
		if ((count($this->ohoco->config['notify']) > 0) || (!empty(TELEGRAM_API_KEY))) {
			echo "<label for='newname'>notify via</label>";
			echo "<select name='notify_type[]' size='1'>";
			echo "<option value='-'>---</option>";
			if (!empty(TELEGRAM_API_KEY)) {
				if ($preslected_id == 'telegram')
					echo "<option value='telegram' selected>Telegram</option>";
				else
					echo "<option value='telegram'>Telegram</option>";
			}
			foreach ($this->ohoco->config['notify'] as $notify_id => $device_item) {
				if ($notify_id == $preslected_id)
					echo "<option value='",$notify_id,"' selected>",$notify_id,"</option>";
				else
					echo "<option value='",$notify_id,"'>",$notify_id,"</option>";
			}
			echo "</select> ";
			echo "<input type='text' name='notify_text[]' value='",$preslected_text,"'><br>";
		}
	}

	function edit($trigger_id) {
		echo "<div class='form-container'>";

		echo "<form method='POST' action='index.php?view=trigger-edit' accept-charset='utf-8'>";
		echo "<input type='hidden' name='do' value='save-trigger'>";
		echo "<input type='hidden' name='id' value='",$trigger_id,"'>";

		echo "<label for='currid'>Trigger-ID</label>";
		echo "<span id='currid'>",$trigger_id,"</span><br>";

		echo "<label for='newname'>Name</label>";
		echo "<input type='text' name='newname' value='",$this->ohoco->config['trigger'][$trigger_id]['name'],"'><br>";

// 		echo "<label for='newname'>Name</label>";
// 		echo "<select id='cond_status_",$cond_index,"' name='cond_status_",$cond_index,"' size='1'>";
// 		echo "<option value='SWITCH'>SWITCH</option>";
// 		echo "<option value='NOTIFY'>NOTIFY</option>";
// 		echo "<option value='COMAND'>COMMAND</option>";
// 		echo "</select><br>";

		foreach ($this->ohoco->config['trigger'][$trigger_id]['actions'] as $n => $action) {
			if ($action['type'] == 'SWITCH') {
				$this->edit_switch_row($action['id'], $action['val'], true);
			}
			elseif ($action['type'] == 'NOTIFY') {
				$this->edit_notify_row($action['id'], $action['val'], true);
			}
		}

		$this->edit_switch_row('', '', false);
		$this->edit_notify_row('', '', false);

		echo "<input type='submit' value='Speichern'>";
		echo "</form>";

// 		echo "<em>",TXTAPILINKS,"<br><br>";
// 		if (array_key_exists($sensor_id, $this->ohoco->config['switch'])) {
// 			echo BASEURL.'/rpc/switch/on/?id='.$sensor_id;
// 			echo "<br><br>";
// 			echo BASEURL.'/rpc/switch/off/?id='.$sensor_id;
// 			echo "<br><br>";
// 			echo BASEURL.'/rpc/switch/toggle/?id='.$sensor_id;
// 		}
// 		else
// 			echo BASEURL.'/rpc/sensor/value/?id='.$sensor_id.'&payload=%val%';
// 		echo "</em>";

		echo "</div>";

// echo "<div class='debug'>";
// debugarr($this->ohoco->config['trigger'][$trigger_id]);
// debugarr($this->ohoco->config['notify']);
// // debugarr($this->ohoco->config);
// echo "</div>";
	}
}
?>