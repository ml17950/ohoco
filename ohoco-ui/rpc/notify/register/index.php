<?php	// last change: 2018-09-13
	include_once('../../../_sysconfig.php');
	include_once('../../../lib/common.php');
	include_once('../../../lib/class.ohoco.php');

	$notify_id	= param('id');
	$payload	= param('payload');

// 	echo "\$notify_id: ",$notify_id,"<br>";
// 	echo "\$payload: ",$payload,"<br>";

	if (!empty($notify_id)) {
		$ohoco = new clsOHoCo();
		if ($ohoco->notify_register($notify_id, $payload))
			echo "ACK";
		else
			echo "ERR";
	}
	else
		echo "ERR:missing id";
?>