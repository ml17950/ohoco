ADC_MODE(ADC_VCC);

#include <OHoCo.h>

const char*  SKETCH_VERSION     = "2018-07-23";
const char*  WIFI_DEVICE_NAME   = "ESP-Robby-Monitor";

#define HCSR04_ECHO_PIN     D6
#define HCSR04_TRIG_PIN     D5

#define ROBBY_IS_HOME       1
#define ROBBY_IS_AWAY       2

#include "./credentials.h"
#include "./HCSR04_functions.h"

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
    ohoco.config.minValue              = 3;
    ohoco.config.maxValue              = 30;
    strcpy(ohoco.config.dataTopic,       "");
    strcpy(ohoco.config.inTrigger,       "US-ROBBY-HOME");
    strcpy(ohoco.config.outTrigger,      "US-ROBBY-AWAY");
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

  pinMode(HCSR04_ECHO_PIN, INPUT);
  pinMode(HCSR04_TRIG_PIN, OUTPUT);

  LAST_STATUS = ROBBY_IS_AWAY;
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
  ohoco.register_sensor("Robby", "mower");

  ohoco.on_message(ohoco_callback);
  
  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();

  if ((millis() - LAST_CHECK_MILLIES) > ohoco.config.checkInterval) {
    // read distance 3 times in 1 second
    // to eliminate detection errors
    long distance1 = HCSR04_read_distance();
    delay(300);
    long distance2 = HCSR04_read_distance();
    delay(300);
    long distance3 = HCSR04_read_distance();

//    String strDebug = "";
//    strDebug = "MIN:" + String(ohoco.config.minValue, DEC) + " / MAX:" + String(ohoco.config.maxValue, DEC) + " / D1:" + String(distance1, DEC) + "cm / D2:" + String(distance2, DEC) + "cm / D3:" + String(distance3, DEC) + "cm";
//    ohoco.debug(strDebug);

    // set Robby working/away
    int THIS_STATUS = ROBBY_IS_AWAY;
    
    // check, if one of the three measurements is above MIN_DISTANCE
    if ((distance1 >= ohoco.config.minValue) || (distance2 >= ohoco.config.minValue) || (distance3 >= ohoco.config.minValue)) {
      // check, if one of the three measurements is below MAX_DISTANCE
      if ((distance1 <= ohoco.config.maxValue) || (distance2 <= ohoco.config.maxValue) || (distance3 <= ohoco.config.maxValue)) {
        // Robby charging/home
        THIS_STATUS = ROBBY_IS_HOME;
      }
    }
    
    if (THIS_STATUS != LAST_STATUS) {
      if (THIS_STATUS == ROBBY_IS_HOME) {
        ohoco.debug("HCSR04 -> found object");

        ohoco.trigger_activate(ohoco.config.inTrigger);
        ohoco.set_sensor_value("Robby", "charging", "");
      }
      else {
        ohoco.debug("HCSR04 -> no object");

        ohoco.trigger_activate(ohoco.config.outTrigger);
        ohoco.set_sensor_value("Robby", "working", "");
      }
    }
    else {
      ohoco.debug("HCSR04 -> no change");
    }

    ohoco.led_flash(2, 100);

    LAST_STATUS = THIS_STATUS;
    LAST_CHECK_MILLIES = millis();
  }

  delay(1000);
}

void ohoco_callback(String cmd) {
  if (cmd == "DEBUG") {
    long distance1 = HCSR04_read_distance();
    char cm[12] = {0};
    ltoa(distance1, cm, 10);
    ohoco.set_sensor_value("DEBUG", cm, " cm");
  }
}
