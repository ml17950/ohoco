ADC_MODE(ADC_VCC);

#include <OHoCo.h>
#include "./credentials.h"

const char*  SKETCH_VERSION     = "2018-07-23";
const char*  WIFI_DEVICE_NAME   = "ESP-3DPrn-Sensor";

#define IR_DETECT_PIN           D5

OHoCo ohoco;

// global variables
unsigned long LAST_CHECK_MILLIES;
int LAST_STATUS;

void setup() {
  ohoco.debugmode(LED);
  //ohoco.debugmode(SERIAL);

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

  pinMode(IR_DETECT_PIN, INPUT);

  LAST_CHECK_MILLIES = millis() - ohoco.config.checkInterval;

  ohoco.wifi_connect();
  
  if (ohoco.config.useMQTT == 1) {
    ohoco.mqtt_setup();
    ohoco.mqtt_connect();
  }
  else {
    ohoco.http_setup();
  }

  ohoco.register_device(SKETCH_VERSION);
  ohoco.register_sensor("3D-Drucker", "3d-printer");

  ohoco.setup_ready();

  for (int i=0; i<10; i++) {
    ohoco.led_flash(1, 100);
    delay(1000);
  }

  LAST_STATUS = digitalRead(IR_DETECT_PIN);

  if (LAST_STATUS == LOW) {
    ohoco.set_sensor_value("3D-Drucker", "idle", "");
  }
  else {
    ohoco.set_sensor_value("3D-Drucker", "working", "");
  }
}

void loop() {
  ohoco.keepalive();

  if ((millis() - LAST_CHECK_MILLIES) > ohoco.config.checkInterval) {
    int THIS_STATUS = digitalRead(IR_DETECT_PIN);

    if (THIS_STATUS != LAST_STATUS) {
      if (THIS_STATUS == LOW) {
        ohoco.debug("GPIO LOW  -> no object");

        ohoco.trigger_activate("IR-3DPRN-IDLE");
        ohoco.set_sensor_value("3D-Drucker", "idle", "");

        ohoco.led_flash(2, 100);
      }
      else {
        ohoco.debug("GPIO HIGH -> found object");

        ohoco.trigger_activate("IR-3DPRN-WORK");
        ohoco.set_sensor_value("3D-Drucker", "working", "");

        ohoco.led_flash(3, 100);
      }
    }
    else {
      ohoco.debug("GPIO      -> no change");
    }

    LAST_STATUS = THIS_STATUS;
    LAST_CHECK_MILLIES = millis();
  }
}
