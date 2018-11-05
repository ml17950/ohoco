<?php
// last change: 2018-05-30
class clsCronjobInterface {
	var $rift3;

	function __construct(&$rift3) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$this->rift3 = $rift3;
	}

	function __destruct() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
	}

	function select_sensors($id, $name, $preselected) {
		$items =  array('time' => 'Uhrzeit',
						'wtime' => 'Uhrzeit (Mo-Fr)',
						'ftime' => 'Uhrzeit (Sa-So)');

		echo "<select id='",$id,"' name='",$name,"' size='1'>";
// 		echo "<option value=''>-</option>";
		foreach ($items as $nid => $name) {
			if ($nid == $preselected)
				echo "<option value='",$nid,"' selected>",$name,"</option>";
			else
				echo "<option value='",$nid,"'>",$name,"</option>";
		}
		echo "</select><br>";
	}

	function select_action($id, $name, $sel_type, $sel_node, $sel_action) {
		$sel_key = $sel_type.':'.$sel_node.':'.$sel_action;

		echo "<select id='",$id,"' name='",$name,"' size='1'>";
		echo "<option value='-:-:-'>Dieses Cronjob löschen</option>";
		foreach ($this->rift3->trigger as $iid => $iname) {
			$cur_key = 'trigger:'.$iid.':';
			if ($cur_key == $sel_key)
				echo "<option value='",$cur_key,"' selected>",$iid,"</option>";
			else
				echo "<option value='",$cur_key,"'>",$iid,"</option>";
		}

// 		foreach ($this->rift3->config['switch'] as $iid => $iname) {
// 			$cur_key = 'switch:'.$iid.':on';
// 			if ($cur_key == $sel_key)
// 				echo "<option value='",$cur_key,"' selected>schalte ",$this->rift3->config['names'][$iid]," an</option>";
// 			else
// 				echo "<option value='",$cur_key,"'>schalte ",$this->rift3->config['names'][$iid]," an</option>";
// 
// 			$cur_key = 'switch:'.$iid.':off';
// 			if ($cur_key == $sel_key)
// 				echo "<option value='",$cur_key,"' selected>schalte ",$this->rift3->config['names'][$iid]," aus</option>";
// 			else
// 				echo "<option value='",$cur_key,"'>schalte ",$this->rift3->config['names'][$iid]," aus</option>";
// 		}
		echo "</select><br>";
	}

	function display() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$this->rift3->cronjobs_read();

		asort($this->rift3->cronjobs);

		echo "<div class='js-cronjobs cronjobs-container'>";

		echo "<div class='form-container'>";
		echo "<form method='POST' action='cronjobs.php' accept-charset='utf-8'>";
		echo "<input type='hidden' name='do' value='save-cronjobs'>";

		foreach ($this->rift3->cronjobs as $jobid => $job) {
			echo "<label for='chk-sensor'>Wenn Sensor</label>";
			$this->select_sensors('chk-sensor', $jobid."[chk-sensor]", $job['chk-sensor']);
			echo "<label for='chk-value'>ist gleich</label>";
			echo "<input type='text' id='chk-value' name='",$jobid,"[chk-value]' value='",$job['chk-value'],"'><br>";
			echo "<label for='run-param'>dann</label>";
			$this->select_action('run-param', $jobid."[run-param]", $job['run-type'], $job['run-node'], $job['run-action']);
			echo "<hr>";
		}

		echo "<label for='chk-sensor'>Wenn Sensor</label>";
		$this->select_sensors('chk-sensor', "new[chk-sensor]", '');
		echo "<label for='chk-value'>ist gleich</label>";
		echo "<input type='text' id='chk-value' name='new[chk-value]' value=''><br>";
		echo "<label for='run-param'>dann</label>";
		$this->select_action('run-param', "new[run-param]", '', '', '');

		echo "<input type='submit' value='Speichern'>";
		echo "</form>";
		echo "</div>"; // .form-container

		echo "</div>"; // .cronjobs-container

// 		echo "<div class='debug'>";
// 		debugarr($this->rift3->cronjobs);
// 		echo "</div>";
	}
}
?>