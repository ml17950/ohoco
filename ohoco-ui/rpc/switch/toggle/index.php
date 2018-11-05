<?php	// last change: 2018-09-13
	include_once('../../../_sysconfig.php');
	include_once('../../../lib/common.php');
	include_once('../../../lib/class.ohoco.php');

	$switch_id	= param('id');
	$payload	= param('payload');

// 	echo "\$switch_id: ",$switch_id,"<br>";
// 	echo "\$payload: ",$payload,"<br>";

	if (!empty($switch_id)) {
		$ohoco = new clsOHoCo();
		if ($ohoco->switch_toggle_onoff($switch_id))
			echo "ACK";
		else
			echo "ERR";
	}
	else
		echo "ERR:missing id";
?>