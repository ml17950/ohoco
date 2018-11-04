ADC_MODE(ADC_VCC);

#include "OHoCo.h"
#include "./credentials.h"

const char*  SKETCH_VERSION     = "18.10.05";
const char*  WIFI_DEVICE_NAME   = "OHoCo-Libary-Test";

// setup libaries
OHoCo ohoco;

// global variables
unsigned long LAST_CHECK_MILLIES;
int COUNTER;

void setup() {
  //ohoco.debugmode(LED);
  ohoco.debugmode(SERIAL);

  ohoco.initialize(false);
  
//  ohoco.config_reset();

  if (!ohoco.config_read()) {
    // Configuration not set or not valid -> set defaults, but don't save to EEPROM
    strcpy(ohoco.config.displayName,     WIFI_DEVICE_NAME);
    strcpy(ohoco.config.wifi_ssid,       WIFI_SSID);
    strcpy(ohoco.config.wifi_pass,       WIFI_PASS);
    strcpy(ohoco.config.controller_ip,   CONTROLLER_IP);
    ohoco.config.controller_port       = CONTROLLER_PORT;
    strcpy(ohoco.config.controller_user, CONTROLLER_USER);
    strcpy(ohoco.config.controller_pass, CONTROLLER_PASS);
    ohoco.config.useMQTT               = 0;
    ohoco.config.checkInterval         = 10000;
    ohoco.config.minValue              = 1;
    ohoco.config.maxValue              = 9999;
    strcpy(ohoco.config.dataTopic,       "");
    strcpy(ohoco.config.inTrigger,       "");
    strcpy(ohoco.config.outTrigger,      "");
    strcpy(ohoco.config.genericValue01,  "");
    strcpy(ohoco.config.genericValue02,  "");
    strcpy(ohoco.config.genericValue03,  "");
    strcpy(ohoco.config.genericValue04,  "");
    strcpy(ohoco.config.genericValue05,  "");
    strcpy(ohoco.config.genericValue06,  "");
    strcpy(ohoco.config.genericValue07,  "");
    strcpy(ohoco.config.genericValue08,  "");
    strcpy(ohoco.config.genericValue09,  "");
    strcpy(ohoco.config.genericValue10,  "");
  }
  ohoco.config_display();

ohoco.config.useMQTT               = 0;

  LAST_CHECK_MILLIES = millis() - ohoco.config.checkInterval + 3000;
  COUNTER = 0;

  ohoco.wifi_connect();
  
  if ((ohoco.config.useMQTT == 1) || (ohoco.config.controller_port == 1883)) {
    //ohoco.mqtt_setup();
    ohoco.mqtt_setup("ohoco/will/esp", "off", 0, false);
    ohoco.mqtt_connect();
  }
  else {
    ohoco.http_setup();
  }

  ohoco.register_device(SKETCH_VERSION);
  ohoco.register_sensor("test-sensor", "voltage", "V");
//  ohoco.register_switch("test-socket", "light");
//  ohoco.mqtt_publish("ohoco/will/esp", "on", false);  ohoco.register_notify("test-notify");  
  ohoco.on_message(ohoco_callback);

  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();

  ohoco.led_on();
  delay(1000);
  ohoco.led_off();
  delay(1000);

  if ((millis() - LAST_CHECK_MILLIES) > ohoco.config.checkInterval) {
    ohoco.led_flash(3, 100);

    

    ohoco.sensor_update("test-sensor", "3.5");


    
    //ohoco.notify("test-notify", "alarm");

    // =============================================================================
    
    LAST_CHECK_MILLIES = millis();
  }
}

void ohoco_callback(String topic, String payload) {
  ohoco.println("ohoco_callback (" + payload + ") on (" + topic + ")");
  
  if (payload == "DEBUG") {
    LAST_CHECK_MILLIES = millis() - ohoco.config.checkInterval;
    COUNTER = 0;
  }
}
