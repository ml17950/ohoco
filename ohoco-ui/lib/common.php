<?php	// last change: 2018-09-21
	function param($key, $dflt = '') {
		if (isset($_GET[$key]) && ($_GET[$key] != ''))
			return trim($_GET[$key]);
		else {
			if (isset($_POST[$key]) && ($_POST[$key] != '')) {
				if (is_array($_POST[$key]))
					return $_POST[$key];
				else
					return trim($_POST[$key]);
			}
			else
				return $dflt;
		}
	}

	function param_int($key, $dflt = 0) {
		if (isset($_POST[$key]) && ($_POST[$key] != '')) {
			if (is_array($_POST[$key]))
				return $_POST[$key];
			else
				return intval(trim($_POST[$key]));
		}
		elseif (isset($_GET[$key]) && ($_GET[$key] != ''))
			return intval(trim($_GET[$key]));
		else
			return $dflt;
	}

	function debugarr(&$arr) {
		echo highlight_string(print_r($arr, true)),"<hr>";
	}

	function getLastNDays($days, $format = 'd/m'){
	    $m = date("m"); $de= date("d"); $y= date("Y");
	    $dateArray = array();
	    for($i=0; $i<=$days-1; $i++){
	    	$key = date('Y-m-d', mktime(0,0,0,$m,($de-$i),$y));
	        $dateArray[$key]['date'] = date($format, mktime(0,0,0,$m,($de-$i),$y));
	        $dateArray[$key]['value'] = 0;
	    }
	    return array_reverse($dateArray);
	}

	function time_diff($seconds, $show_sec = true) {
		$sec = $seconds;
		$days = floor($sec / 86400);
		$sec -= ($days * 86400);
		$hours = floor($sec / 3600);
		$sec -= ($hours * 3600);
		$minutes = floor($sec / 60);
		$sec -= ($minutes * 60);
		$sec = round($sec, 0);

		$ret = '';
		if ($days > 0)
			$ret .= $days.' '.TXTDAYS.', ';
		if ($hours > 0)
			$ret .= $hours.' '.TXTHOURS.', ';
		if ($minutes > 0)
			$ret .= $minutes.' '.TXTMINUTES.', ';
		if ($show_sec) {
			if ($sec > 0)
				$ret .= $sec.' '.TXTSECONDS;
		}
		else {
			if ($minutes == 0)
				$ret .= $sec.' '.TXTSECONDS;
		}

		if (substr($ret, -2, 2) == ', ')
			$ret = substr($ret, 0, strlen($ret)-2);

		$p = strrpos($ret, ',');
		if ($p !== false)
			$ret = substr($ret, 0, $p).' '.TXTAND.substr($ret, $p+1);

		return $ret;
	}

	function dtstr($datetime, $long_format = 'd.m.y H:i:s', $short_format = 'H:i') {
		if (is_int($datetime))
			$timestamp = $datetime;
		else
			$timestamp = strtotime($datetime);
		if ($timestamp >= TODAY)
			return TXTTODAY.date(' '.$short_format, $timestamp);
		elseif ($timestamp >= YESTERDAY)
			return TXTYESTERDAY.date(' '.$short_format, $timestamp);
		else
			return date($long_format, $timestamp);
	}

	function get_unique_id() {
		$m = microtime(true);
		return sprintf("%8x%05x",floor($m),($m-floor($m))*1000000);
	}

	function virtual_device($on_param, $off_param, $onoff) {
	}
?>