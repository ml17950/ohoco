<?php	// last change: 2018-11-05
class clsUserInterface {
	var $ohoco;
	var $home;
	var $sensors;
	var $rules;
	var $trigger;
	var $log;
	var $config;
	var $cronjobs;

	function __construct() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';

		include_once('lib/class.ohoco.php');
		$this->ohoco = new clsOHoCo();

		include_once('lib/subclass.ui.home.php');
		$this->home = new clsHomeInterface($this->ohoco);

		include_once('lib/subclass.ui.sensors.php');
		$this->sensors = new clsSensorInterface($this->ohoco);

		include_once('lib/subclass.ui.rules.php');
		$this->rules = new clsRuleInterface($this->ohoco);

		include_once('lib/subclass.ui.trigger.php');
		$this->trigger = new clsTriggerInterface($this->ohoco);

		include_once('lib/subclass.ui.log.php');
		$this->log = new clsLogInterface($this->ohoco);

		include_once('lib/subclass.ui.config.php');
		$this->config = new clsConfigInterface($this->ohoco);

		include_once('lib/subclass.ui.cronjobs.php');
		$this->cronjobs = new clsCronjobInterface($this->ohoco);
	}

	function __destruct() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';
	}

	function meta($with_autoreload = true) {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';

		ob_start('ob_gzhandler');

		echo "<html>";
		echo "<head>";
		echo "	<meta charset='UTF-8'>";
		echo "	<meta http-equiv='Content-Type' content='text/html; charset=utf8'>";
		echo "	<meta name='generator' content='PHP/PsPad'>";
		echo "	<meta name='robots' content='noarchive'>";
		echo "	<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'>";
		echo "	<meta name='apple-mobile-web-app-capable' content='yes'>";
		echo "	<meta name='mobile-web-app-capable' content='yes'>";
		echo "	<link href='touch-icon.png' type='image/png' rel='shortcut icon'>";
		echo "	<link href='touch-icon.png' rel='apple-touch-icon'>";
		echo "	<title>OHoCo</title>";
		echo "	<link rel='stylesheet' type='text/css' href='res/css/layout.css'>";
		echo "	<script type='text/javascript' src='res/js/jquery-1.11.3.min.js'></script>";
		echo "	<script type='text/javascript' src='res/js/ohoco.js?",VERSION,"'></script>";
		if ($with_autoreload) {
			echo "	<script type='text/javascript'>\n";
			echo "	document.addEventListener('visibilitychange', function () {\n";
			echo "	    // fires when user switches tabs, apps, goes to homescreen, etc.\n";
			echo "	    if (document.visibilityState === 'hidden') { lastView = Date.now() / 1000 | 0; }\n";
			echo "	    // fires when app transitions from prerender, user returns to the app / tab.\n";
			echo "	    if (document.visibilityState === 'visible') {\n";
			echo "	    	var now = Date.now() / 1000 | 0;\n";
			echo "	    	if (lastView > 0) { if ((now - lastView) > 120) { location.reload(); } }\n";
			echo "	      lastView = now;\n";
			echo "	    }\n";
			echo "	});\n";
			echo "	</script>\n";
		}
		echo "</head>";
	}

	function header($title = '') {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';

		echo "<body>";
		echo "<div id='content' class='js-content'>";

		echo "<h1 id='js-title'>",$title," | ",date('d.m.y H:i:s'),"</h1>";
	}

	function navigation() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';

		echo "<div id='navigation'>";
// 		echo "	<!--<a href='#' onclick='return getConfig();'><img src='res/img/config.png' border='0' width='48' height='48' hspace='3'></a>-->";
		echo "	<a href='index.php?view=debug'><img src='res/img/ui/debug.png' border='0' width='46' height='46' hspace='3'></a>";
		echo "	<a href='index.php?view=config'><img src='res/img/ui/config.png' border='0' width='46' height='46' hspace='3'></a>";
		echo "	<a href='index.php?view=log'><img src='res/img/ui/log.png' border='0' width='46' height='46' hspace='3'></a>";
// 		echo "	<a href='cronjobs.php'><img src='res/img/ui/cronjobs.png' border='0' width='46' height='46' hspace='3'></a>";
		echo "	<a href='index.php?view=rules'><img src='res/img/ui/rules.png' border='0' width='46' height='46' hspace='3'></a>";
		echo "	<a href='index.php?view=trigger'><img src='res/img/ui/trigger.png' border='0' width='46' height='46' hspace='3'></a>";
		echo "	<a href='index.php?view=sensors'><img src='res/img/ui/sensors.png' border='0' width='46' height='46' hspace='3'></a>";
		echo "	<a href='index.php?view=home'><img src='res/img/ui/home.png' border='0' width='46' height='46' hspace='3'></a>";
		echo "</div>";
	}

	function footer() {
		//echo __CLASS__.'::'.__FUNCTION__.'<br>';

		echo "</div>"; // #content

		echo "</body>";
		echo "</html>";
	}

	function success_message($msg, $goto = '') {
		echo "<div class='success-msg'>",$msg,"</div>";
		if (!empty($goto))
			echo "<meta http-equiv='refresh' content='3; URL=",$goto,"'>";
		else
			echo "<meta http-equiv='refresh' content='3; URL=home.php'>";
	}

	function error_message($msg, $goto = '') {
		echo "<div class='error-msg'>",$msg,"</div>";
		if (!empty($goto))
			echo "<meta http-equiv='refresh' content='3; URL=",$goto,"'>";
		else
			echo "<meta http-equiv='refresh' content='3; URL=home.php'>";
	}

	function get_icon_by_type($type, $status) {
		switch ($type) {
			case 'unknown':
				return $type.'.png';
				break;
			case 'time':
			case 'date':
				return $type.'.png';
				break;
			case 'temperature':
			case 'lightintensity':
			case 'voltage':
			case 'currency':
				return $type.'.png';
				break;
			case 'humidity':
				$raw_val = str_replace(' %', '', $status);
				if ($raw_val < 40)
					return 'humidity-low.png';
				elseif ($raw_val > 60)
					return 'humidity-high.png';
				else
					return 'humidity-ok.png';
				break;
// 				case 'daynight':	$sensor_image = 'daynight-'.$status.'.png'; break;
// 				case 'weather':		$sensor_image = 'weather-'.$status.'.png'; break;
// 				case 'gate':		$sensor_image = 'gate-'.$status.'.png'; break;
// 				case 'light':		$sensor_image = 'light-'.$status.'.png'; break;
			default:
				if (!empty($status))
					return $type.'-'.$status.'.png';
				else
					return $type.'-off.png';
				break; //$sensor_image = 'unknown.png'; break;
		}
	}

	function get_value_type($status) {
		switch ($status) {
			case 'on':
			case 'open':
			case 'home':
			case 'working':
				return 'is-on';
				break;
			case 'off':
			case 'closed':
			case 'away':
			case 'charging':
				return 'is-off';
				break;
			default:
				return 'is-val';
		}
	}

	function is_chart_enabled($type, $value) {
		switch ($type) {
			case 'temperature':
			case 'humidity':
			case 'lightintensity':
			case 'voltage':
			case 'currency':
				//echo "\n<!-- TYP ",$type," -->\n";
				return true;
		}
		switch ($value) {
			case 'on':
			case 'off':
			case 'open':
			case 'closed':
			case 'home':
			case 'away':
			case 'working':
			case 'charging':
				//echo "\n<!-- VAL ",$value," -->\n";
				return true;
		}
		//echo "\n<!-- TYP ",$type," / VAL ",$value," -->\n";
		return true;
	}

	function get_sonsors_by_name() {
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
debugarr($sensor_array);
		return $sensor_array;
	}
}
?>