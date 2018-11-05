<?php	// last change: 2018-10-01
	include_once('_sysconfig.php');
	include_once('lib/common.php');
	include_once('lib/class.ohoco.php');

	$ohoco = new clsOHoCo();

	$ohoco->plugins_load();

	$ohoco->rules_check_conditions();

	if (date('H:i') == '02:16')
		$ohoco->log_resize();
?>