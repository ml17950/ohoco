ADC_MODE(ADC_VCC);

#include "OHoCo.h"

const char*  SKETCH_VERSION     = "2018-04-14";
const char*  WIFI_DEVICE_NAME   = "OHoCo-Libary-Test";

#define MODULE_PIN     5

OHoCo ohoco;

int counter;
unsigned long LAST_CHECK_MILLIES;
int LAST_DOOR_STATUS;

void setup() {
  // ohoco.debugmode(LED);
  ohoco.debugmode(SERIAL);
  
  ohoco.initialize();
  ohoco.config_display();
  
//  ohoco.led_on();
//  ohoco.led_off();
//  ohoco.led_flash(3, 200);

//  ohoco.wifi_config("ssid", "pass");
//  ohoco.wifi_config("ssid", "pass", "192.168.178.240", "192.168.178.254", "192.168.178.254");
//  ohoco.wifi_connected()
//  ohoco.wifi_disconnect();

//  ohoco.mqtt_config("broker", 1883, "", "");
//  ohoco.mqtt_connect();
//  ohoco.mqtt_disconnect();
//  ohoco.mqtt_connected();

  ohoco.wifi_config(WIFI_DEVICE_NAME, "HoCoNet", "HoCoNet.99");
  ohoco.wifi_connect();

  ohoco.http_config("192.168.178.250", 1883, "", "");
//  ohoco.mqtt_config("192.168.178.250", 1883, "", "");
//  ohoco.mqtt_connect();

  ohoco.on_message(ohoco_callback);
  
  ohoco.register_device(SKETCH_VERSION);
  ohoco.register_sensor("Test", "pir");

  
//  ohoco.register_sensor("Garage", "gate");

//  ohoco.set_sensor_value("Garage", "open", " °C");
//  delay(2000);
//  ohoco.set_sensor_value("Garage", "moving", " %");
//  delay(2000);
//  ohoco.set_sensor_value("Garage", "closed", "");

  pinMode(MODULE_PIN, INPUT);
  
  LAST_CHECK_MILLIES = millis() - ohoco._ohoco_cfg.checkInterval;
  
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

  if ((millis() - LAST_CHECK_MILLIES) > ohoco._ohoco_cfg.checkInterval) {
    int THIS_DOOR_STATUS  = -1;
    
    ohoco.debug("Starting measurement...");

    if (digitalRead(MODULE_PIN) == LOW)
      THIS_DOOR_STATUS = LOW;
    else
      THIS_DOOR_STATUS = HIGH;

    if (THIS_DOOR_STATUS != LAST_DOOR_STATUS) {
      ohoco.debuginline("*********** CHANGED ***********");
      switch (THIS_DOOR_STATUS) {
        case HIGH: ohoco.debug(" => CLOSED "); break;
        case LOW:   ohoco.debug(" => OPEN "); break;
      }
      
      if (THIS_DOOR_STATUS == LOW) {
        ohoco.trigger_activate("IR-GAR-OPEN");
        ohoco.set_sensor_value("Garage", "open", "");
      }
      else {
        ohoco.trigger_activate("IR-GAR-CLOSED");
        ohoco.set_sensor_value("Garage", "closed", "");
      }
      
      LAST_DOOR_STATUS = THIS_DOOR_STATUS;
    }
  
    ohoco.led_flash(2, 100);
    
    LAST_CHECK_MILLIES = millis();
  }
  
  delay(500);
}

void ohoco_callback(String cmd) {
  if (cmd == "debug") {
    LAST_CHECK_MILLIES = millis() - ohoco._ohoco_cfg.checkInterval;
  }
}
