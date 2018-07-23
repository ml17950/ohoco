ADC_MODE(ADC_VCC);

#include "OHoCo.h"
#include "./credentials.h"

const char*  SKETCH_VERSION     = "2018-07-23";
const char*  WIFI_DEVICE_NAME   = "OHoCo-Libary-Test";

OHoCo ohoco;

int counter;
unsigned long LAST_CHECK_MILLIES;
int LAST_DOOR_STATUS;

void setup() {
  // ohoco.debugmode(LED);
  ohoco.debugmode(SERIAL);
  
  ohoco.initialize();
  
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
    ohoco.config.checkInterval         = 60000;
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

  ohoco.wifi_connect();
//  ohoco.wifi_connected();
//  ohoco.wifi_disconnect();

  if (ohoco.config.useMQTT == 1) {
    ohoco.mqtt_setup();
    ohoco.mqtt_connect();
//  ohoco.mqtt_disconnect();
//  ohoco.mqtt_connected();
  }
  else {
    ohoco.http_setup();
  }
  
//  ohoco.led_on();
//  ohoco.led_off();
//  ohoco.led_flash(3, 200);

  // set callback function
  ohoco.on_message(ohoco_callback);
  
  ohoco.register_device(SKETCH_VERSION);
  ohoco.register_sensor("Test", "pir");
  
  LAST_CHECK_MILLIES = millis() - ohoco.config.checkInterval;

  //pinMode(D5, OUTPUT);
  //digitalWrite(D5, LOW);
  
//  ohoco.mqtt_subscribe("asdf");

//  ohoco.config_set("asd=123");
//  ohoco.config_set("sdf=123=abc");
  
//  
//
//  ohoco.switch_on();
//  ohoco.switch_off();
//  ohoco.switch_toggle();

//  ohoco.sensor_set_value();
//
//  ohoco.trigger_activate();
//
//  ohoco.config_read();
//  ohoco.config_write();
  

//  delay(20000);
//  ohoco.reboot();


  counter = 0;

  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();

  if ((millis() - LAST_CHECK_MILLIES) > ohoco.config.checkInterval) {
    ohoco.debug("Starting measurement...");

    //digitalWrite(D5, !digitalRead(D5));
      
    ohoco.led_flash(2, 100);
    
    LAST_CHECK_MILLIES = millis();
  }
  
  delay(500);
}

void ohoco_callback(String cmd) {
  if (cmd == "debug") {
    LAST_CHECK_MILLIES = millis() - ohoco.config.checkInterval;
  }
}
