// last change: 2018-11-05

var lastView = 0;

function log(msg) {
	var theTime = new Date().toTimeString().replace(/.*(\d{2}:\d{2}:\d{2}).*/, "$1");
	console.log(theTime+': '+msg);
// 	alert(msg);
}

var ohococonfig = {
	updval: function(device_id, cfg_key, cfg_val) {
		log("ohococonfig.updval: ["+device_id+"/"+cfg_key+"/"+cfg_val.trim()+"]");
		log("ohococonfig.updval= ["+encodeURIComponent(cfg_val)+"]");
		$.ajax({
			url:  'rpc/device/command/?id='+device_id,
			data: 'payload=CFG:'+cfg_key+'='+encodeURIComponent(cfg_val),
			type: 'POST',
			success: function(data) {
// 				alert(data);
				$('#js-'+cfg_key).css('background','green');
			},
			error: function (xhr, textStatus, thrownError) {
	        	alert(textStatus);
			}
		});
		
		return false;
	},
	writecfg: function(device_id) {
		log("ohococonfig.writecfg: "+device_id);
		$.ajax({
			url:  'rpc/device/command/?id='+device_id,
			data: 'payload=WRITECFG',
			type: 'POST',
			success: function(data) {
				window.location.href = 'config.php';
			},
			error: function (xhr, textStatus, thrownError) {
	        	alert(textStatus);
			}
		});
		return false;
	}
};

var ohocolog = {
	search: function() {
		var find = $('#js-search').val().toLowerCase();

		$('.log-box').each(function(i, obj) {
			var content = $(this).text();
			var pos = content.toLowerCase().indexOf(find);
			if (pos > -1) {
				if ($(this).hasClass('log-box-hidden')) {
					console.log("remove class log-box-hidden");
					$(this).removeClass('log-box-hidden');
				}
			}
			else {
				if (!$(this).hasClass('log-box-hidden')) {
					console.log("add class log-box-hidden");
					$(this).addClass('log-box-hidden');
				}
			}
		});
		return false;
	}
};

var ohocoswitch = {
	on: function(id, type) {
		log("ohocoswitch.on :: "+id+" / "+type);
		
		$('#device-'+id+'-icon').attr('src', 'res/img/ui/switching.gif');
		
// 		var now = Math.round((new Date()).getTime() / 1000);
		
		$.ajax({
			url:  'rpc/switch/on/?id='+id,
			data: 'payload=on',
			type: 'POST',
			success: function(data) {
				ohocoswitch.update_state(id, type, data);
			},
			error: function (xhr, textStatus, thrownError) {
	        	ohocoswitch.update_state(id, type, 'error');
			}
		});
	},
	
	off: function(id, type) {
		log("ohocoswitch.off :: "+id+" / "+type);
		
		$('#device-'+id+'-icon').attr('src', 'res/img/ui/switching.gif');
		
// 		var now = Math.round((new Date()).getTime() / 1000);
		
		$.ajax({
			url:  'rpc/switch/off/?id='+id,
			data: 'payload=off',
			type: 'POST',
			success: function(data) {
				ohocoswitch.update_state(id, type, data);
			},
			error: function (xhr, textStatus, thrownError) {
	        	ohocoswitch.update_state(id, type, 'error');
			}
		});
	},
	
	toggle: function(id, type) {
		var current_state = $('#device-'+id+'-icon').attr('data-state');
		
		log("ohocoswitch.toggle :: "+id+" / "+type+" == "+current_state);
		
		if ((current_state == 'off') || (current_state == 'closed'))
			ohocoswitch.on(id, type);
		else
			ohocoswitch.off(id, type);
		
		return false;
	},
	
	update_state: function(id, type, state) {
		log("ohocoswitch.update_state not ready :: "+id+" / "+type+" / "+state);
		
		$('#device-'+id+'-icon').attr('data-state', state);
		
		switch (state) {
			case 'on':
			case 'off':
				$('#device-'+id+'-icon').attr('src', 'res/img/sensors/'+type+'-'+state+'.png');
				break;
			case 'error':
				$('#device-'+id+'-icon').attr('src', 'res/img/ui/error.png');
				break;
			default:
				$('#device-'+id+'-icon').attr('src', 'res/img/ui/unknown.png');
		}
	}
};

var ohocochart = {
	toggle: function(id, name) {
		var current_state = $('#js-'+id).css('display');

		var chart_options = {
			series: {
				lines: { show: true, steps: true, fill: false },
				points: { show: false }
			},
			//colors: ['#DC143C'],
			xaxis: {
				mode: "time",
				timezone: "browser"
			},
			grid: {
				hoverable: true,
				clickable: true
			},
			zoom: {
				interactive: true
			},
			pan: {
				interactive: true
			}
		};
		var chart_data = [];

		if (current_state == 'none') {
			// show and load chart
			$('#js-'+id).show();

			var placeholder = $('#js-'+id);
			var plot = $.plot(placeholder, [ chart_data ], chart_options);

			placeholder.bind("plothover", function (event, pos, item) {
				if (item) {
					var x = item.datapoint[0].toFixed(2),
						y = item.datapoint[1].toFixed(2);
					var d = new Date(x / 1);
					$("#tooltip").html(y + " @ " + d.toLocaleString('de-DE'))
						.css({top: item.pageY+5, left: item.pageX+5})
						.fadeIn(200);
				} else {
					$("#tooltip").hide();
				}
			});
			
			$.ajax({
				url:  'rpc/json/chart/?id='+id,
				data: 'v=0',
				type: 'POST',
				success: function(data) {
					$.plot(placeholder, [ data ], chart_options);
				},
				error: function (xhr, textStatus, thrownError) {
		        	alert(textStatus);
				}
			});
		}
		else {
			// hide
			$('#js-'+id).html('');
			$('#js-'+id).hide();
		}

		return false;
	}
};

function toggle_receipe_trigger(id) {
	$('#js-'+id).slideToggle('slow', function() { });
	return false;
}
