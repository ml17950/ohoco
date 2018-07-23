ADC_MODE(ADC_VCC);

#include <OHoCo.h>
#include "./credentials.h"

const char*  SKETCH_VERSION     = "2018-07-23";
const char*  WIFI_DEVICE_NAME   = "ESP-Movement-Detector";

#define SENSOR_PIN              D2
#define SOMEONE_IS_HERE         1
#define NOBODY_IS_HERE          0
#define MS_BETWEEN_DETECTIONS   3200

OHoCo ohoco;

// global variables
unsigned long LAST_CHECK_MILLIES;
int DETECTION_COUNTER;
int MOTION_COUNTER;
int LAST_STATE;

long LAST_CHANGE;

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
    ohoco.config.checkInterval         = 180000;
    ohoco.config.minValue              = 1;
    ohoco.config.maxValue              = 9999;
    strcpy(ohoco.config.dataTopic,       "");
    strcpy(ohoco.config.inTrigger,       "SOMEONE_HERE");
    strcpy(ohoco.config.outTrigger,      "NOBODY_HERE");
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

  pinMode(SENSOR_PIN, INPUT);

  // attach an interrupt handler to run when the input is going low
//  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), sensorHandlerActive, RISING);

  LAST_CHECK_MILLIES = millis() - ohoco.config.checkInterval;
  DETECTION_COUNTER = ohoco.config.checkInterval;
  MOTION_COUNTER = 0;
  LAST_STATE = NOBODY_IS_HERE;
  
  ohoco.wifi_connect();
  
  if (ohoco.config.useMQTT == 1) {
    ohoco.mqtt_setup();
    ohoco.mqtt_connect();
  }
  else {
    ohoco.http_setup();
  }
  
  ohoco.register_device(SKETCH_VERSION);
  ohoco.register_sensor("human", "presence");

//  ohoco.on_message(ohoco_callback);
  
  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();
  
  if ((millis() - LAST_CHECK_MILLIES) > MS_BETWEEN_DETECTIONS) {
    int THIS_STATE = digitalRead(SENSOR_PIN);
    
    ohoco.debuginline("DC: ");
    ohoco.debuginline(DETECTION_COUNTER);
    ohoco.debuginline(" / MC: ");
    ohoco.debuginline(MOTION_COUNTER);
    ohoco.debuginline(" / LS: ");
    ohoco.debuginline(LAST_STATE);
    ohoco.debuginline(" / TS: ");
    ohoco.debug(THIS_STATE);

    if (THIS_STATE == SOMEONE_IS_HERE) {
      if (LAST_STATE == SOMEONE_IS_HERE) {
        ohoco.debug("someone still here - reset DETECTION_COUNTER");
        DETECTION_COUNTER = ohoco.config.checkInterval;
      }
      else {
        MOTION_COUNTER++;

        if (MOTION_COUNTER > 1) {
          ohoco.debug("new detection (2 times) - fire trigger & set status");
          
          ohoco.trigger_activate(ohoco.config.inTrigger);
          ohoco.set_sensor_value("human", "here", "");
          ohoco.led_on();

          DETECTION_COUNTER = ohoco.config.checkInterval;
          MOTION_COUNTER = 0;
          LAST_STATE = SOMEONE_IS_HERE;
        }
        else {
          ohoco.debug("new detection - check twice");
        }
      }
    }
    else {
      if (LAST_STATE == SOMEONE_IS_HERE) {
        ohoco.debug("someone was here - decrease DETECTION_COUNTER");
        DETECTION_COUNTER -= MS_BETWEEN_DETECTIONS;

        if (DETECTION_COUNTER <= 0) {
          ohoco.debug("someone was here for some time - fire trigger & set status");
          
          ohoco.trigger_activate(ohoco.config.outTrigger);
          ohoco.set_sensor_value("human", "away", "");
          ohoco.led_off();

          LAST_STATE = NOBODY_IS_HERE;
          DETECTION_COUNTER = 0;
        }
      }
      else {
        ohoco.debug("still nobody here");
        if (MOTION_COUNTER > 0) {
          ohoco.debug("reset MOTION_COUNTER");
          MOTION_COUNTER = 0;
        }
      }
    }


//    if (THIS_STATE != LAST_STATE) {
//      LAST_STATE = THIS_STATE;
//
//      if (THIS_STATE == NOBODY_IS_HERE) {
//        int xxx = (millis() - LAST_CHANGE);
//        ohoco.debug(xxx);
//      }
//
//      LAST_CHANGE = millis();
//    }



//    if (THIS_STATE == HIGH) {
//      DETECTION_COUNTER = ohoco.config.checkInterval;
//      ohoco.debug("MOTION DETECTED - COUNTER RESET");
//    }
//    
//    if (DETECTION_COUNTER <= 0) {
//      if (LAST_STATE == SOMEONE_IS_HERE) {
//        ohoco.debug("FIRE TRIGGER AWAY");
//  
//        ohoco.trigger_activate(ohoco.config.outTrigger);
//        ohoco.set_sensor_value("human", "away", "");
//        ohoco.led_off();
//        
//        LAST_STATE = NOBODY_IS_HERE;
//      }
//      DETECTION_COUNTER = 0;
//    }
//    else {
//      if (LAST_STATE == NOBODY_IS_HERE) {
//        ohoco.debug("FIRE TRIGGER SOMEONE");
//  
//        ohoco.trigger_activate(ohoco.config.inTrigger);
//        ohoco.set_sensor_value("human", "here", "");
//        ohoco.led_on();
//        
//        LAST_STATE = SOMEONE_IS_HERE;
//      }
//    }
//  
//    DETECTION_COUNTER -= 1000;
//
    LAST_CHECK_MILLIES = millis();
  }
}

void sensorHandlerActive() {
    // indicate the new sensor state - active
    ohoco.led_on();
    
    // attach an interrupt handler to run when the input is going high
    detachInterrupt(digitalPinToInterrupt(SENSOR_PIN));
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), sensorHandlerIdle, FALLING);

    DETECTION_COUNTER = ohoco.config.checkInterval;
}

void sensorHandlerIdle() {
    // indicate the new sensor state - idle
    ohoco.led_off();
    
    // attach an interrupt handler to run when the input is going low
    detachInterrupt(digitalPinToInterrupt(SENSOR_PIN));
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), sensorHandlerActive, RISING);
}

