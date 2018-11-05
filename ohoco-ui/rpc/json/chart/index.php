<?php	// last change: 2018-11-05
	include_once('../../../_sysconfig.php');
	include_once('../../../lib/common.php');
	include_once('../../../lib/class.ohoco.php');

	$display_id = param('id');

	$ohoco = new clsOHoCo();

	$ohoco->log_read();
	$tmpArr = array();
	$tmpCnt = 0;
	foreach ($ohoco->log_items as $i => $log) {
		if ($log['id'] == $display_id) {
			$key = $log['time'];
			$tmpArr[$key] = array($key * 1000, $ohoco->helper_status2number($log['value']));
		}
	}
	$key = time();
	$tmpArr[$key] = array($key * 1000, $ohoco->helper_status2number($ohoco->config['sensor'][$display_id]['value']));
	ksort($tmpArr);
	$retArr = array();
	foreach ($tmpArr as $key => $arr) {
		$retArr[] = $arr;
	}
	header('Content-Type: application/json');
	echo json_encode($retArr);
?>