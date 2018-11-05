<?php	// last change: 2018-09-13
	include_once('../../../_sysconfig.php');
	include_once('../../../lib/common.php');
	include_once('../../../lib/class.ohoco.php');

	$ohoco = new clsOHoCo();
	$json = json_encode($ohoco->config);
	header('Content-Type: application/json');
	echo $json;
?>