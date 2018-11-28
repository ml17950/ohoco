<?php	// last change: 2018-11-28
class clsRuleInterface extends clsUserInterface {
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
		echo "<div class='js-rules rule-container'>";

		if (is_array($this->ohoco->config['rules'])) {
			foreach ($this->ohoco->config['rules'] as $ruleid => $rule) {
				echo "<div class='rule-box'>";
				if ($rule['active'] == 1)
					echo "<div class='rule-status'><a href='index.php?view=rules&do=deactivate-rule&id=",$ruleid,"'><img src='res/img/ui/is-on.png' width='40' height='40'></a></div>";
				else
					echo "<div class='rule-status'><a href='index.php?view=rules&do=activate-rule&id=",$ruleid,"'><img src='res/img/ui/is-off.png' width='40' height='40'></a></div>";
				echo "<div class='rule-name'>",$rule['name'],"</div>";
				if ($rule['active'] == 0)
					echo "<div class='rule-options'><a href='index.php?view=rules&id=",$ruleid,"&do=delete'><img src='res/img/ui/delete.png' width='24' height='24' alt='delete' title='delete'></a></div>";
				echo "<div class='rule-options'><a href='index.php?view=rule-edit&id=",$ruleid,"'><img src='res/img/ui/edit.png' width='24' height='24' alt='edit' title='edit'></a></div>";

// 				echo "<div class='rule-conditions'>";
// 				foreach ($rule['conditions'] as $type => $condition) {
// 					if (!empty($condition['status']) && ($condition['status'] != '-'))
// 						echo "&bull; ",$condition['status']," &rarr; ",$condition['type']," // ",$condition['value'],"<br>";
// 				}
// 				echo "</div>"; // .rule-conditions

				if ($rule['active'] == 1) {
					echo "<div class='rule-actions'>";
					if (!empty($rule['action']['status']['id']) && ($rule['action']['status']['id'] != '-'))
						echo "&rarr; ",TXTSET," ",$rule['action']['status']['id']," ",TXTSETTO," ",$rule['action']['status']['value'],"<br>";
					if (!empty($rule['action']['trigger']['id']) && ($rule['action']['trigger']['id'] != '-'))
						echo "&rarr; ",TXTACTIVATE," ",TXTTRIGGER,": ",$rule['action']['trigger']['id'],"<br>";
					echo "</div>"; // .rule-actions
				}

				echo "</div>"; // .rule-box
			}
		}

		// -----------------------------------------------------------------

		echo "<div class='form-container'>";
		echo "<form method='POST' action='index.php?view=rules' accept-charset='utf-8'>";
		echo "<input type='hidden' name='do' value='create-rule'>";
		echo "<input type='submit' value='",TXTCREATENEWRULE,"'>";
		echo "</form>";
		echo "</div>"; // .form-container

		echo "</div>"; // .rules-container

// 		echo "<div class='debug'>";
// 		debugarr($this->ohoco->config['rules']);
// 		echo "</div>";
	}

	function edit($rule_id) {
		$rule = $this->ohoco->config['rules'][$rule_id];

		echo "<div class='form-container'>";

		echo "<form method='POST' action='index.php?view=rule-edit' accept-charset='utf-8'>";
		echo "<input type='hidden' name='do' value='save-rule'>";
		echo "<input type='hidden' name='ruleid' value='",$rule_id,"'>";

		echo "<label for='currid'>",TXTRULEID,"</label>";
		echo "<span id='currid'>",$rule_id,"</span><br>";

		echo "<label for='rulename'>",TXTRULENAME,"</label>";
		echo "<input type='text' name='rulename' value='",$rule['name'],"'><br>";

		// ---------------------------------------------------------------------

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

		// ---------------------------------------------------------------------

		for ($cond_index=1; $cond_index<4; $cond_index++) {
			if ($cond_index == 1)
				echo "<br>";
			else
				echo "<br>",TXTAND,"<br><br>";

			echo "<label for='cond_status_",$cond_index,"'>",TXTIF,"</label>";

			echo "<select id='cond_status_",$cond_index,"' name='cond_status_",$cond_index,"' size='1'>";
			echo "<option value='-'>---</option>";
			foreach ($sensor_array as $sensor_key => $sensor) {
				if ($sensor['id'] == $rule['conditions'][$cond_index]['status'])
					echo "<option value='",$sensor['id'],"' selected>",$sensor['name'],"</option>";
				else
					echo "<option value='",$sensor['id'],"'>",$sensor['name'],"</option>";
			}
			echo "</select><br>";

			$condchecks['EQU'] = TXTRULECONDEQU;
			$condchecks['NEQ'] = TXTRULECONDNEQ;
			$condchecks['LSS'] = TXTRULECONDLSS;
			$condchecks['LEQ'] = TXTRULECONDLEQ;
			$condchecks['GTR'] = TXTRULECONDGTR;
			$condchecks['GEQ'] = TXTRULECONDGEQ;
			$condchecks['OLD'] = TXTRULECONDOLD;

			echo "<label for='cond_type_",$cond_index,"'>",TXTIS,"</label>";

			echo "<select id='cond_type_",$cond_index,"' name='cond_type_",$cond_index,"' size='1'>";
			echo "<option value='-'>---</option>";
			foreach ($condchecks as $cond_id => $cond_name) {
				if ($cond_id == $rule['conditions'][$cond_index]['type'])
					echo "<option value='",$cond_id,"' selected>",$cond_name,"</option>";
				else
					echo "<option value='",$cond_id,"'>",$cond_name,"</option>";
			}
			echo "</select><br>";
// echo "<br>";
 
			echo "<label for='cond_value_",$cond_index,"'>",TXTASAS,"</label>";
			echo "<input type='text' name='cond_value_",$cond_index,"' value='",$rule['conditions'][$cond_index]['value'],"'><br>";

			$is_sensor_id = $rule['conditions'][$cond_index]['status'];
			$is_value = $this->ohoco->config['sensor'][$is_sensor_id]['value'];
			$check_value = $rule['conditions'][$cond_index]['value'];

			echo "<label for='cond_current_",$cond_index,"'>",TXTCURRENT,"</label>";
			if (!empty($rule['conditions'][$cond_index]['type'])) {
				if ($this->ohoco->rules_check_single_condition($rule['conditions'][$cond_index]['type'], $is_sensor_id, $is_value, $check_value))
					echo "<img src='res/img/ui/is-on.png' height='26' align='absmiddle'> ";
				else
					echo "<img src='res/img/ui/is-off.png' height='26' align='absmiddle'> ";
				echo $is_value;
				echo " ",TXTBEFORE," ",(time() - $this->ohoco->config['sensor'][$rule['conditions'][$cond_index]['status']]['change'])," ",TXTSECONDS,"<br>";
			}
			else
				echo "---<br>";
		}
		
		// ---------------------------------------------------------------------
		
		echo "<br>",TXTACTIVATETRIGGER,"<br><br>";

		echo "<label for='triggeridold'>",TXTAVAILABLE,"</label>";
		echo "<select name='triggeridold' size='1'>";
		echo "<option value='-'>---</option>";
		foreach ($this->ohoco->config['trigger'] as $trigger_id => $trigger_change) {
			if ($trigger_id == $rule['actions']['trigger'])
				echo "<option value='",$trigger_id,"' selected>",$trigger_id,"</option>";
			else
				echo "<option value='",$trigger_id,"'>",$trigger_id,"</option>";
		}
		echo "</select><br>";

		echo "<label for='triggeridnew'>",TXTORNEW,"</label>";
		echo "<input type='text' name='triggeridnew' value=''><br>";

		// ---------------------------------------------------------------------
		
		echo "<br>",TXTORSETSTATUS,"<br><br>";

		echo "<label for='statusid'>",TXTAVAILABLE,"</label>";
		echo "<select id='statusid' name='statusid' size='1'>";
		echo "<option value='-'>---</option>";
		foreach ($sensor_array as $sensor_id => $sensor) {
			if (!empty($sensor['name']))
				$status_name = $sensor['name'];
			else
				$status_name = $sensor_id;
			if ($sensor_id == $rule['actions']['status']['id'])
				echo "<option value='",$sensor_id,"' selected>",$status_name,"</option>";
			else
				echo "<option value='",$sensor_id,"'>",$status_name,"</option>";
		}
		echo "</select><br>";

		echo "<label for='statusidnew'>",TXTORNEW,"</label>";
		echo "<input type='text' name='statusidnew' value=''><br>";

		echo "<label for='statusval'>",TXTSETTO,"</label>";
		echo "<input type='text' name='statusval' value='",$rule['actions']['status']['value'],"'><br>";
		
		// ---------------------------------------------------------------------

		echo "<input type='submit' value='",TXTSAVE,"'>";
		echo "</form>";

		echo "</div>"; // .form-container

// 		echo "<div class='debug'>";
// 		debugarr($this->ohoco->config['rules'][$rule_id]);
// 		echo "</div>";
	}
}
?>