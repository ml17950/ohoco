<?php	// last change: 2018-11-28

	//error_reporting(E_ALL);
	error_reporting(E_ALL & ~E_NOTICE);
	ini_set('display_errors', 1);

	define('VERSION', '2018-11-28');

	define('ABSPATH', dirname(__FILE__));
	define('CHMODMASK', 0775);

// echo $_SERVER['HTTP_USER_AGENT']; exit;
// print_r($_SERVER); exit;

//     [CONTENT_TYPE] => application/x-www-form-urlencoded; charset=UTF-8
//     [HTTP_X_REQUESTED_WITH] => XMLHttpRequest
// HTTP_REFERER
// REQUEST_URI
// file_put_contents(ABSPATH.'/config/debug.log', date('c')."\t".$_SERVER['HTTP_X_REQUESTED_WITH']."\t".$_SERVER['CONTENT_TYPE']."\r\n", FILE_APPEND);
// chmod(ABSPATH.'/config/debug.log', CHMODMASK);

	if (strtolower(substr($_SERVER['HTTP_USER_AGENT'], 0, 4)) == 'wget')
		define('CLIENT', 'crond');
	elseif (strtolower(substr($_SERVER['HTTP_USER_AGENT'], 0, 3)) == 'esp')
		define('CLIENT', 'esp');
	elseif ($_SERVER['HTTP_X_REQUESTED_WITH'] == 'XMLHttpRequest')
		define('CLIENT', 'ui');
	elseif (strpos($_SERVER['REQUEST_URI'], 'ifttt') !== false)
		define('CLIENT', 'ifttt');
	else
		define('CLIENT', 'web');

	if (empty($_SERVER['REQUEST_SCHEME']))
		define('BASEURL', 'http://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['SCRIPT_NAME']));
	else
		define('BASEURL', $_SERVER['REQUEST_SCHEME'].'://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['SCRIPT_NAME']));

	// ###########################################
	// ### change only defines below this line ###
	// ###########################################

	date_default_timezone_set('Europe/Berlin');

	define('MAXLOG', 1000);

	define('UI_LANGUAGE', 'de');

	define('TODAY', mktime(0,0,0));
	define('YESTERDAY', TODAY - 86400);

	include_once('lib/lang_'.UI_LANGUAGE.'.php');
	include_once('_mqtt.php');
	include_once('_telegram.php');
?>