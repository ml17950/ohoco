<?php	// last change: 2018-10-04
	include_once('_sysconfig.php');
	include_once('lib/common.php');
	include_once('lib/class.ui.php');

	$ui = new clsUserInterface();

	$view = param('view', 'home');

	include_once('index.actions.php');

	switch ($view) {
		case 'debug':
			$ui->meta(false);
			$ui->header(TXTDEBUG);
			$ui->navigation();
			echo "<div class='debug'>";

// 			debugarr($ui->ohoco->log_items);
// 			
// 			$ui->ohoco->log('trigger', 'SOMEONE-HERE', 'msg', 'val', 'info');
			
// 			$ui->ohoco->log_read_from_ram();
// 			debugarr($ui->ohoco->log_items);

// 			echo "<hr>";
// 			$ui->ohoco->plugins_load();
// 			echo "<hr>";

// 			$ui->ohoco->rules_check_conditions();
			echo "<hr>";
			echo "<hr>";

			$ui->ohoco->debug();
// 			$ui->ohoco->notify_activate("garage-alarm", "garage-open");
			echo "</div>";
			$ui->footer();
			break;
		case 'home':
			$ui->meta();
			$ui->header(TXTHOME);
			$ui->navigation();
			$ui->home->display_widgets();
			$ui->home->display_switches();
			$ui->footer();
			break;
		case 'sensors':
			$ui->meta();
			$ui->header(TXTSENSORS);
			$ui->navigation();
			$ui->sensors->display();
			$ui->footer();
			break;
		case 'sensor-edit':
			$id = param('id');

			$ui->meta();
			$ui->header(TXTSENSORS);
			$ui->navigation();
			$ui->sensors->edit($id);
			$ui->footer();
			break;
		case 'trigger':
			$ui->meta();
			$ui->header(TXTRULES);
			$ui->navigation();
			$ui->trigger->display();
			$ui->footer();
			break;
		case 'trigger-edit':
			$id = param('id');

			$ui->meta();
			$ui->header(TXTRULES);
			$ui->navigation();
			$ui->trigger->edit($id);
			$ui->footer();
			break;
		case 'rules':
			$ui->meta();
			$ui->header(TXTRULES);
			$ui->navigation();
			$ui->rules->display();
			$ui->footer();
			break;
		case 'rule-edit':
			$id = param('id');

			$ui->meta();
			$ui->header(TXTRULES);
			$ui->navigation();
			$ui->rules->edit($id);
			$ui->footer();
			break;
		case 'log':
			$ui->meta();
			$ui->header(TXTLOG);
			$ui->navigation();
			$ui->log->search();
			$ui->log->display();
			$ui->footer();
			break;
		case 'config':
			$ui->meta(false);
			$ui->header(TXTCONFIG);
			$ui->navigation();
			$ui->config->display_devices();
			$ui->config->formHeader('save-systemcfg');
			$ui->config->display_widgets();
			$ui->config->formFooter();
// 			$ui->config->display_system();
// 			$ui->config->display_mqtt();
// 			$ui->config->display_types();
			$ui->footer();
			break;
		case 'config-edit':
			$id = param('id');

			$ui->meta(false);
			$ui->header(TXTCONFIG);
			$ui->navigation();
			$ui->config->formHeader('save-devicecfg');
			$ui->config->display_device_configuration($id);
			$ui->config->formFooter();
			$ui->footer();
			break;
		default:
			echo "ERR:unknown view: ",$view;
	}
?>