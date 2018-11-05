<?php	// last change: 2018-09-13
	include_once('../../../_sysconfig.php');
	include_once('../../../lib/common.php');
	include_once('../../../lib/class.ohoco.php');

	$trigger_id	= param('id');
	$payload	= param('payload');

// 	echo "\$trigger_id: ",$trigger_id,"<br>";
// 	echo "\$payload: ",$payload,"<br>";

	if (!empty($trigger_id)) {
		$ohoco = new clsOHoCo();
		if ($ohoco->trigger_activate($trigger_id, $payload))
			echo "ACK";
		else
			echo "ERR";
	}
	else
		echo "ERR:missing id";
?>