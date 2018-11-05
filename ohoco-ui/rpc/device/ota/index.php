<?php	// last change: 2018-10-06

//	https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html

	include_once('../../../_sysconfig.php');
	include_once('../../../lib/common.php');
	include_once('../../../lib/class.ohoco.php');

	function check_header($name, $value = false) {
	    if(!isset($_SERVER[$name])) {
	        return false;
	    }
	    if($value && $_SERVER[$name] != $value) {
	        return false;
	    }
	    return true;
	}

	$device_id	= param('id');
	$curr_vers	= param('currver');
	$info_type	= basename(dirname(__FILE__));


// file_put_contents(ABSPATH.'/config/debug.log', print_r($_SERVER, true));


	header('Content-type: text/plain; charset=utf8', true);

	if (!check_header('HTTP_USER_AGENT', 'ESP8266-http-Update')) {
		header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
		echo "only for ESP8266 updater (1)\n";
		exit();
	}

	if (
		!check_header('HTTP_X_ESP8266_STA_MAC') ||
		!check_header('HTTP_X_ESP8266_AP_MAC') ||
		!check_header('HTTP_X_ESP8266_FREE_SPACE') ||
		!check_header('HTTP_X_ESP8266_SKETCH_SIZE') ||
		!check_header('HTTP_X_ESP8266_SKETCH_MD5') ||
		!check_header('HTTP_X_ESP8266_CHIP_SIZE') ||
		!check_header('HTTP_X_ESP8266_SDK_VERSION')
	) {
		header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
		echo "only for ESP8266 updater (2)\n";
		exit();
	}

	if (empty($device_id)) {
		header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
		echo "missing device_id (3)\n";
		exit();
	}

	$ota_file = ABSPATH.'/ota-files/'.$device_id.'-'.$curr_vers.'.bin';

	if (!is_file($ota_file)) {
		header($_SERVER["SERVER_PROTOCOL"].' 500 no version for this device', true, 500);
		exit();
	}

	if ($_SERVER['HTTP_X_ESP8266_FREE_SPACE'] <= filesize($ota_file)) {
		header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
		echo "not enough free space (5)\n";
		exit();
	}

	if ($_SERVER["HTTP_X_ESP8266_SKETCH_MD5"] != md5_file($ota_file)) {
	    header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
	    header('Content-Type: application/octet-stream', true);
	    header('Content-Disposition: attachment; filename='.basename($ota_file));
	    header('Content-Length: '.filesize($ota_file), true);
	    header('x-MD5: '.md5_file($ota_file), true);
	    readfile($ota_file);
	    exit();
	}
	else {
		header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
		exit();
	}

// 	header($_SERVER["SERVER_PROTOCOL"].' 500 no version for ESP MAC', true, 500);
?>