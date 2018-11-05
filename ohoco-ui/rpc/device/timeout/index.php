<?php	// last change: 2018-09-15
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
		if ($ohoco->device_set_info($device_id, $info_type, $payload))
			echo "ACK";
		else
			echo "ERR";
	}
	else
		echo "ERR:missing id";
?>