ADC_MODE(ADC_VCC);

#include <RCSwitch.h>
#include <OHoCo.h>
#include "./credentials.h"

const char*  SKETCH_VERSION     = "2018-07-23";
const char*  WIFI_DEVICE_NAME   = "ESP-RC-Controller";

#define RC_PIN                  D5

OHoCo ohoco;
RCSwitch myRCSendSwitch = RCSwitch();
RCSwitch myRCRecvSwitch = RCSwitch();

unsigned long ulRequestCount;

#include "./rc_functions.h"

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

  ohoco.debug("SYS >> Initialize RCSwitch");
  myRCSendSwitch.enableTransmit(RC_PIN);  // Required set output pin
  myRCSendSwitch.setProtocol(1);          // Optional set protocol (default is 1, will work for most outlets)
  myRCSendSwitch.setPulseLength(313);     // Optional set pulse length.
//  myRCSendSwitch.setRepeatTransmit(3);    // Optional set number of transmission repetitions.

  myRCRecvSwitch.enableReceive(D6);

  ohoco.wifi_connect();
  
  if (ohoco.config.useMQTT == 1) {
    ohoco.mqtt_setup();
    ohoco.mqtt_connect();
  }
  else {
    ohoco.http_setup();
  }

  ohoco.register_device(SKETCH_VERSION);
  ohoco.register_switch("socket1", "light");
  ohoco.register_switch("socket2", "light");
  ohoco.register_switch("socket3", "light");
  ohoco.register_switch("socket4", "light");
  ohoco.register_switch("socket5", "light");
  ohoco.register_switch("socket6", "light");
  ohoco.register_switch("socket7", "light");
  ohoco.register_switch("socket8", "light");
  ohoco.register_switch("socket9", "light");

  ohoco.on_message(ohoco_callback);
  
  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();
//  myWebServer.handleClient();

  if (myRCRecvSwitch.available()) {
    PowerSocketCommandReceived(myRCRecvSwitch.getReceivedValue(), myRCRecvSwitch.getReceivedBitlength());
    myRCRecvSwitch.resetAvailable();
  }
}

void ohoco_callback(String cmd) {
//  ohoco.debug("callback: " + cmd);

  int pos = cmd.indexOf(':');
  String sSocket = cmd.substring(0, pos);
  String sAction = cmd.substring(pos+1);
  
  if (sAction == "on") {
    turnPowerSocketON(sSocket);
  }
  else if (sAction == "off") {
    turnPowerSocketOFF(sSocket);
  }

//  char cSocket[strlen(sSocket.c_str())+1];
//  sSocket.toCharArray(cSocket, strlen(sSocket.c_str())+1);
//
//  char cAction[strlen(sAction.c_str())+1];
//  sAction.toCharArray(cAction, strlen(sAction.c_str())+1);
//  
//  ohoco.set_sensor_value(cSocket, cAction, "");

  ohoco.led_flash(3, 50);
}

