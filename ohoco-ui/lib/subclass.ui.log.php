<?php	// last change: 2018-10-06
class clsLogInterface {
	var $ohoco;

	function __construct(&$ohoco) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		$this->ohoco = $ohoco;
	}

	function __destruct() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
	}

	function search() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		echo "<div class='log-search'>";
		echo "<input type='text' id='js-search' value=''  onkeyup='return ohocolog.search()' />";
		echo "</div>"; // .log-search
	}

	function display() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
		echo "<div class='js-log log-container'>";

		$this->ohoco->log_read();
		
// debugarr($this->ohoco->log_items);
// debugarr($this->ohoco->config['device']);

		if (count($this->ohoco->log_items) > 0) {
			foreach ($this->ohoco->log_items as $log_entry) {
				switch ($log_entry['section']) {
					case 'status':
						if (!empty($this->ohoco->config['sensor'][$log_entry['id']]['name']))
							$name = $this->ohoco->config['sensor'][$log_entry['id']]['name'];
						else
							$name = $log_entry['id'];
						$unit = ' '.$this->ohoco->config['sensor'][$log_entry['id']]['unit'];
						break;
					case 'trigger':
						if (!empty($this->ohoco->config['trigger'][$log_entry['id']]['name']))
							$name = $this->ohoco->config['trigger'][$log_entry['id']]['name'];
						else
							$name = $log_entry['id'];
						$unit = '';
						break;
					case 'notify':
						$name = $log_entry['id'];
						$unit = '';
						break;
					case 'device':
						if (!empty($this->ohoco->config['device'][$log_entry['id']]['name']))
							$name = $this->ohoco->config['device'][$log_entry['id']]['name'];
						else
							$name = $log_entry['id'];
						$unit = '';
						break;
					case 'config':
						if (!empty($this->ohoco->config['device'][$log_entry['id']]['name']))
							$name = $this->ohoco->config['device'][$log_entry['id']]['name'];
						else
							$name = $log_entry['id'];
						$unit = '';
						break;
					default:
						$name = 'TODO: '.$log_entry['section'];
						$unit = '';
				}

				echo "<div class='log-box log-box-",$log_entry['section'],"'>";
				echo "<div class='log-client'><img src='res/img/clients/",$log_entry['client'],".png' width='16' height='16' alt='",$log_entry['client'],"' title='",$log_entry['client'],"'></div>";
				echo "<div class='log-time'>",dtstr(intval($log_entry['time'])),"</div>";
// 				if (strlen($log_entry['t']) > 1)
// 					echo "<div class='log-trigger'>",$log_entry['t'],"</div>";
				if (!empty($log_entry['info']))
					echo "<div class='log-message'>",$name,"<span class='log-info'>",$log_entry['info'],"</span></div>";
				else
					echo "<div class='log-message'>",$name,"</div>";
				if ($log_entry['value'] != '') {
					if ($log_entry['value'] == 'on')
						echo "<div class='log-value is-on'>",$log_entry['value'],"</div>";
					else if ($log_entry['value'] == 'off')
						echo "<div class='log-value is-off'>",$log_entry['value'],"</div>";
					else if ($log_entry['value'] == 'nochg')
						echo "<div class='log-value is-notchanged'>",$log_entry['value'],"</div>";
// 					elseif (($log_entry['value'] == ON2OFF) || ($log_entry['value'] == OFF2ON))
// 						echo "<div class='log-value is-changed'>",$log_entry['value'],"</div>";
					else {
						if ($log_entry['info'] == 'connect')
							echo "<div class='log-value is-val'><a href='http://",$log_entry['value'],"/status'>",$log_entry['value'],"</a></div>";
						else
							echo "<div class='log-value is-val'>",$log_entry['value'],$unit,"</div>";
					}
				}
				echo "</div>";
			}
		}

		echo "</div>"; // .log-container
	}
}
?>