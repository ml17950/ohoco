<?php	// last change: 2018-10-05
	include_once('../../_sysconfig.php');
	include_once('../../lib/common.php');
	include_once('../../lib/class.ohoco.php');

	$device_id	= param('id');
	$payload	= param('payload');

// 	echo "\$sensor_id: ",$sensor_id,"<br>";
// 	echo "\$payload: ",$payload,"<br>";

	if (!empty($device_id)) {
		$ohoco = new clsOHoCo();
		$ohoco->log('device', $device_id, $payload);
		echo "ACK";
	}
	else
		echo "ERR:missing id";
?>