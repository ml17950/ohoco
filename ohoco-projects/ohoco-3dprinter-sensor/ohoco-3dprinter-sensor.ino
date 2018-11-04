ADC_MODE(ADC_VCC);

#include <RCSwitch.h>
#include <OHoCo.h>
#include "./credentials.h"

const char*  SKETCH_VERSION     = "18.11.03";
const char*  WIFI_DEVICE_NAME   = "ESP-3DPrn-Sensor";

#define IR_DETECT_PIN           D5
#define TRANSMITTER_PIN         D6
#define RED_LED_PIN             D7
#define GREEN_LED_PIN           D0

OHoCo ohoco;
RCSwitch myRCSendSwitch = RCSwitch();

#include "./rc_functions.h"

// global variables
unsigned long LAST_CHECK_MILLIES;
int LAST_STATUS;

void setup() {
  ohoco.debugmode(LED, GREEN_LED_PIN);
  //ohoco.debugmode(SERIAL);

  ohoco.initialize();

  //ohoco.config_reset();

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
    ohoco.config.minValue              = 1;
    ohoco.config.maxValue              = 9999;
    strcpy(ohoco.config.dataTopic,       "");
    strcpy(ohoco.config.inTrigger,       "IR-3DPRN-IDLE");
    strcpy(ohoco.config.outTrigger,      "IR-3DPRN-WORK");
  }
  ohoco.config_display();

  // ----------------------------------------

  ohoco.println("SYS  >> Initialize IR-Sensor");
  pinMode(IR_DETECT_PIN, INPUT);

  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);

  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, HIGH);

  ohoco.println("SYS  >> Initialize RC-Switch");
  myRCSendSwitch.enableTransmit(TRANSMITTER_PIN);  // Required set output pin
  myRCSendSwitch.setProtocol(1);          // Optional set protocol (default is 1, will work for most outlets)
  myRCSendSwitch.setPulseLength(313);     // Optional set pulse length.

  LAST_CHECK_MILLIES = millis() - ohoco.config.checkInterval;

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
  ohoco.register_sensor("3D-Printer", "3d-printer", "");
  ohoco.register_switch("socket9", "socket");

  ohoco.on_message(ohoco_callback);
  
  ohoco.setup_ready();

  for (int i=0; i<10; i++) {
    ohoco.led_flash(1, 100);
    delay(1000);
  }

  LAST_STATUS = digitalRead(IR_DETECT_PIN);

  if (LAST_STATUS == LOW) {
    ohoco.println("GPIO >> LOW -> found object");
    ohoco.sensor_update("3D-Printer", "off");
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
  else {
    ohoco.println("GPIO >> HIGH -> no object");
    ohoco.sensor_update("3D-Printer", "on");
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
  }
}

void loop() {
  ohoco.keepalive();

  if ((millis() - LAST_CHECK_MILLIES) > ohoco.config.checkInterval) {
    int THIS_STATUS = digitalRead(IR_DETECT_PIN);

    if (THIS_STATUS != LAST_STATUS) {
      if (THIS_STATUS == LOW) {
        ohoco.println("GPIO >> LOW -> found object");

        ohoco.trigger_activate(ohoco.config.inTrigger);
        ohoco.sensor_update("3D-Printer", "off");
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(GREEN_LED_PIN, LOW);
      }
      else {
        ohoco.println("GPIO >> HIGH -> no object");

        ohoco.trigger_activate(ohoco.config.outTrigger);
        ohoco.sensor_update("3D-Printer", "on");
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(GREEN_LED_PIN, HIGH);
      }
    }
    else {
      ohoco.println("GPIO >> no change");
    }

    LAST_STATUS = THIS_STATUS;
    LAST_CHECK_MILLIES = millis();
  }
}

void ohoco_callback(String topic, String payload) {
  ohoco.println("ohoco_callback (" + payload + ") on (" + topic + ")");

  int pos = payload.indexOf(':');
  String sSocket = payload.substring(0, pos);
  String sAction = payload.substring(pos+1);
  
  if (sAction == "on") {
    turnPowerSocketON(sSocket);
  }
  else if (sAction == "off") {
    turnPowerSocketOFF(sSocket);
  }

  // set new status on controller
  char cSocket[strlen(sSocket.c_str())+1];
  sSocket.toCharArray(cSocket, strlen(sSocket.c_str())+1);
  char cAction[strlen(sAction.c_str())+1];
  sAction.toCharArray(cAction, strlen(sAction.c_str())+1);
  ohoco.sensor_update(cSocket, cAction);

  ohoco.led_flash(3, 50);
}

