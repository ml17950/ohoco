ADC_MODE(ADC_VCC);

#include <OHoCo.h>

const char*  SKETCH_VERSION     = "18.10.06";
const char*  WIFI_DEVICE_NAME   = "ESP-Car-Detector";

#define HCSR04_ECHO_PIN     D5
#define HCSR04_TRIG_PIN     D6
#define DOOR_CHECK_PIN      D7

#define CAR_IS_HOME         1
#define CAR_IS_AWAY         2

#define DOOR_IS_CLOSED      0
#define DOOR_IS_OPEN        1

#include "./credentials.h"
#include "./HCSR04_functions.h"

OHoCo ohoco;

// global variables
unsigned long LAST_CHECK_MILLIES;
int LAST_CAR_STATUS;
int LAST_DOOR_STATUS;
int CHANGE_COUNTER;

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
    ohoco.config.checkInterval         = 15000;
    ohoco.config.minValue              = 50;
    ohoco.config.maxValue              = 150;
    strcpy(ohoco.config.dataTopic,       "");
    strcpy(ohoco.config.inTrigger,       "");
    strcpy(ohoco.config.outTrigger,      "");
  }
  ohoco.config_display();

  // ----------------------------------------
  
  pinMode(HCSR04_ECHO_PIN, INPUT);
  pinMode(HCSR04_TRIG_PIN, OUTPUT);
  pinMode(DOOR_CHECK_PIN, INPUT);

  LAST_CAR_STATUS = CAR_IS_AWAY;
  LAST_CHECK_MILLIES = millis() - ohoco.config.checkInterval;
  CHANGE_COUNTER = 1;

  // ----------------------------------------
  
  ohoco.wifi_connect();

  if ((ohoco.config.useMQTT == 1) || (ohoco.config.controller_port == 1883)) {
    ohoco.mqtt_setup();
    ohoco.mqtt_connect();
  }
  else {
    ohoco.http_setup();
  }

  ohoco.register_device(SKETCH_VERSION);
  ohoco.register_sensor("car", "car", "");
  ohoco.register_sensor("garage", "garage", "");

  ohoco.on_message(ohoco_callback);
  
  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();

  if ((millis() - LAST_CHECK_MILLIES) > ohoco.config.checkInterval) {
    long distance1 = HCSR04_read_distance();
    
    int THIS_CAR_STATUS  = LAST_CAR_STATUS;
    int THIS_DOOR_STATUS = -1;

    if ((distance1 >= ohoco.config.minValue) && (distance1 <= ohoco.config.maxValue))
      CHANGE_COUNTER++;
    else
      CHANGE_COUNTER--;

    if (CHANGE_COUNTER > 3) {
      THIS_CAR_STATUS = CAR_IS_HOME;
      CHANGE_COUNTER = 3;
    }
    else if (CHANGE_COUNTER < 0) {
      THIS_CAR_STATUS = CAR_IS_AWAY;
      CHANGE_COUNTER = 0;
    }

    if (digitalRead(DOOR_CHECK_PIN) == LOW)
      THIS_DOOR_STATUS = DOOR_IS_OPEN;
    else
      THIS_DOOR_STATUS = DOOR_IS_CLOSED;

    if (THIS_CAR_STATUS != LAST_CAR_STATUS) {
      if (THIS_CAR_STATUS == CAR_IS_HOME) {
        ohoco.println("HCSR04 -> found object");

        ohoco.trigger_activate("GAR-CAR-HOME");
        ohoco.sensor_update("car", "home");
        
        if (THIS_DOOR_STATUS == DOOR_IS_OPEN) {
          ohoco.sensor_update("garage", "home-open");
        }
        else {
          ohoco.sensor_update("garage", "home-closed");
        }
      }
      else {
        ohoco.println("HCSR04 -> no object");

        ohoco.trigger_activate("GAR-CAR-AWAY");
        ohoco.sensor_update("car", "away");
        
        if (THIS_DOOR_STATUS == DOOR_IS_OPEN) {
          ohoco.sensor_update("garage", "away-open");
        }
        else {
          ohoco.sensor_update("garage", "away-closed");
        }
      }
    }
    else if (THIS_DOOR_STATUS != LAST_DOOR_STATUS) {
      if (THIS_DOOR_STATUS == DOOR_IS_OPEN) {
        ohoco.trigger_activate("GAR-DOOR-OPEN");
        
        if (THIS_CAR_STATUS == CAR_IS_HOME) {
          ohoco.sensor_update("garage", "home-open");
        }
        else {
          ohoco.sensor_update("garage", "away-open");
        }
      }
      else {
        ohoco.trigger_activate("GAR-DOOR-CLOSED");
        
        if (THIS_CAR_STATUS == CAR_IS_HOME) {
          ohoco.sensor_update("garage", "home-closed");
        }
        else {
          ohoco.sensor_update("garage", "away-closed");
        }
      }
    }
    else {
      ohoco.println("HCSR04 -> no change");
    }

    ohoco.led_flash(2, 100);

    LAST_CAR_STATUS  = THIS_CAR_STATUS;
    LAST_DOOR_STATUS = THIS_DOOR_STATUS;
    LAST_CHECK_MILLIES = millis();
  }
}

void send_distance(long dist_in_cm) {
  char cm[12] = {0};
  ltoa(dist_in_cm, cm, 10);
  ohoco.sensor_update("DEBUG", cm);
}

void ohoco_callback(String topic, String payload) {
  ohoco.println("ohoco_callback (" + payload + ") on (" + topic + ")");

  if (payload == "DEBUG") {
    long distance1 = HCSR04_read_distance();
    send_distance(distance1);
  }
}
