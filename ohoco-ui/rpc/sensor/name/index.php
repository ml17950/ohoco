<?php	// last change: 2018-09-13
	include_once('../../../_sysconfig.php');
	include_once('../../../lib/common.php');
	include_once('../../../lib/class.ohoco.php');

	$sensor_id	= param('id');
	$payload	= param('payload');

// 	echo "\$sensor_id: ",$sensor_id,"<br>";
// 	echo "\$payload: ",$payload,"<br>";

	if (!empty($sensor_id)) {
		$ohoco = new clsOHoCo();
		if ($ohoco->sensor_set_name($sensor_id, $payload))
			echo "ACK";
		else
			echo "ERR";
	}
	else
		echo "ERR:missing id";
?>