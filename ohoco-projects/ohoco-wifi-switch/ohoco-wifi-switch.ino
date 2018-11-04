ADC_MODE(ADC_VCC);

#include <OHoCo.h>
#include "./credentials.h"

const char*  SKETCH_VERSION     = "18.11.04";
const char*  WIFI_DEVICE_NAME   = "ESP-WiFi-Switch";

#define RELAIS_PIN           D7

OHoCo ohoco;

// global variables
unsigned long LAST_CHECK_MILLIES;
int LAST_STATUS;

void setup() {
  ohoco.debugmode(LED);
  ohoco.debugmode(SERIAL);

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
    ohoco.config.checkInterval         = 0;
    ohoco.config.minValue              = 1;
    ohoco.config.maxValue              = 9999;
    strcpy(ohoco.config.dataTopic,       "");
    strcpy(ohoco.config.inTrigger,       "");
    strcpy(ohoco.config.outTrigger,      "");
  }
  ohoco.config_display();

  // ----------------------------------------

  ohoco.println("SYS  >> Initialize GPIOs");

  pinMode(RELAIS_PIN, OUTPUT);
  digitalWrite(RELAIS_PIN, LOW);

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
  ohoco.register_switch("relais1", "socket");

  ohoco.on_message(ohoco_callback);
  
  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();
}

void ohoco_callback(String topic, String payload) {
  ohoco.println("ohoco_callback (" + payload + ") on (" + topic + ")");

  int pos = payload.indexOf(':');
  String sSocket = payload.substring(0, pos);
  String sAction = payload.substring(pos+1);
  
//  if (sAction == "on") {
//    turnPowerSocketON(sSocket);
//  }
//  else if (sAction == "off") {
//    turnPowerSocketOFF(sSocket);
//  }
//
//  // set new status on controller
//  char cSocket[strlen(sSocket.c_str())+1];
//  sSocket.toCharArray(cSocket, strlen(sSocket.c_str())+1);
//  char cAction[strlen(sAction.c_str())+1];
//  sAction.toCharArray(cAction, strlen(sAction.c_str())+1);
//  ohoco.sensor_update(cSocket, cAction);
//
//  ohoco.led_flash(3, 50);
}

void turnRelaisON(String socket_id) {
  ohoco.println("GPIO >> turnRelaisON => " + socket_id);

//  if (socket_id == "socket9") { myRCSendSwitch.sendTriState("FFF000FFFF0F"); }
//  else { ohoco.println("unknown socket"); }

  digitalWrite(RELAIS_PIN, HIGH);
}

void turnRelaisOFF(String socket_id) {
  ohoco.println("GPIO >> turnRelaisOFF => " + socket_id);

//  if (socket_id == "socket9") { myRCSendSwitch.sendTriState("FFF000FFFFF0"); }
//  else { ohoco.println("unknown socket"); }

  digitalWrite(RELAIS_PIN, LOW);
}
