ADC_MODE(ADC_VCC);

#include <RCSwitch.h>
#include <OHoCo.h>
#include "./credentials.h"

const char*  SKETCH_VERSION     = "18.10.06";
const char*  WIFI_DEVICE_NAME   = "ESP-RC-Controller";

#define TRANSMITTER_PIN         D5
#define RECEIVER_PIN            D6

OHoCo ohoco;
RCSwitch myRCSendSwitch = RCSwitch();
RCSwitch myRCRecvSwitch = RCSwitch();

bool enableReceiving;

#include "./ritter_8341c.h"
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
  }
  ohoco.config_display();

  // ----------------------------------------
  
  ohoco.println("SYS  >> Initialize RCSwitch");
  myRCSendSwitch.enableTransmit(TRANSMITTER_PIN);  // Required set output pin
  myRCSendSwitch.setProtocol(1);          // Optional set protocol (default is 1, will work for most outlets)
  myRCSendSwitch.setPulseLength(313);     // Optional set pulse length.
//  myRCSendSwitch.setRepeatTransmit(3);    // Optional set number of transmission repetitions.

  myRCRecvSwitch.enableReceive(RECEIVER_PIN);
  enableReceiving = true;

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
  ohoco.register_switch("socket1", "light");
  ohoco.register_switch("socket2", "light");
  ohoco.register_switch("socket3", "light");
  ohoco.register_switch("socket4", "light");
  ohoco.register_switch("socket5", "light");
  ohoco.register_switch("socket6", "light");
  ohoco.register_switch("socket7", "light");
  ohoco.register_switch("socket8", "light");
  //ohoco.register_switch("socket9", "light");
  ohoco.register_switch("socketA", "light");
  ohoco.register_switch("socketB", "light");
  ohoco.register_switch("socketC", "light");

  ohoco.on_message(ohoco_callback);
  
  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();
//  myWebServer.handleClient();

  if (myRCRecvSwitch.available()) {
    if (enableReceiving)
      PowerSocketCommandReceived(myRCRecvSwitch.getReceivedValue(), myRCRecvSwitch.getReceivedBitlength());
    myRCRecvSwitch.resetAvailable();
  }
}

void ohoco_callback(String topic, String payload) {
  ohoco.println("callback: " + topic + " -> " + payload);

  // disable receiving until sending is complete
  myRCRecvSwitch.disableReceive();

//  ohoco.sensor_update("DEBUG", payload.c_str());
  
  int pos;
  String sSocket;
  String sAction;
  
  if (topic == "UDP") {
    pos = payload.indexOf(':');
    sSocket = payload.substring(0, pos);
    sAction = payload.substring(pos+1);
  }
  else {
    pos = topic.lastIndexOf('/');
    sSocket = topic.substring(pos + 1);
    sAction = payload;
  }

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

  delay(500);
  
  // re-enable receiving
  myRCRecvSwitch.enableReceive(RECEIVER_PIN);

  ohoco.led_flash(3, 50);
}

