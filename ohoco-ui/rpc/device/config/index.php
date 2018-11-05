<?php	// last change: 2018-10-04
	include_once('../../../_sysconfig.php');
	include_once('../../../lib/common.php');
	include_once('../../../lib/class.ohoco.php');

	$device_id	= param('id');
	$payload	= param('payload');
	$info_type	= basename(dirname(__FILE__));

// 	echo "\$device_id: ",$device_id,"<br>";
// 	echo "\$payload: ",$payload,"<br>";
// 	echo "\$info_type: ",$info_type,"<br>";

	if (!empty($device_id)) {
		$ohoco = new clsOHoCo();
		$config_array = json_decode($payload, true);
		if ($ohoco->device_set_config($device_id, $config_array))
			echo "ACK";
		else
			echo "ERR";
	}
	else
		echo "ERR:missing id";
?>