ADC_MODE(ADC_VCC);

#include <OneWire.h>
#include <DallasTemperature.h>
#include <OHoCo.h>
#include "./credentials.h"

const char*  SKETCH_VERSION     = "18.10.06";
const char*  WIFI_DEVICE_NAME   = "ESP-Fan-Controller";

#define IR_SEND_PIN             D6
#define DS18B20_PIN             D3
#define SENSOR_PIN              D2
#define SOMEONE_IS_HERE         1
#define NOBODY_IS_HERE          0
#define MS_BETWEEN_DETECTIONS   3200

OneWire oneWire(DS18B20_PIN);  // on pin 10 (a 4.7K resistor is necessary)
DallasTemperature DS18B20(&oneWire);  // Pass our oneWire reference to Dallas Temperature. 
OHoCo ohoco;

#include <ESP8266WebServer.h>
ESP8266WebServer myWebServer(80);

#include "./ir_sender.h"
#include "./honeywell_ho_5500re.h"

// global variables
unsigned long LAST_TEMP_CHECK_MILLIES;
unsigned long LAST_MOTION_CHECK_MILLIES;
int example_counter = 0;
float LAST_TEMPERATURE;
int LAST_DETECTION_STATE;
int DETECTION_COUNTER;
int MOTION_COUNTER;

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
    ohoco.config.checkInterval         = 600000; // 10 min.
    ohoco.config.minValue              = 2;      // number of positive detections
    ohoco.config.maxValue              = 300000; // 5 min.
    strcpy(ohoco.config.dataTopic,       "");
    strcpy(ohoco.config.inTrigger,       "SOMEONE-HERE");
    strcpy(ohoco.config.outTrigger,      "NOBODY-HERE");
  }
  ohoco.config_display();

  // ----------------------------------------
  
  pinMode(IR_SEND_PIN, OUTPUT);
  digitalWrite(IR_SEND_PIN, HIGH);

  pinMode(SENSOR_PIN, INPUT);

  init_software_pwm(38);

  LAST_TEMP_CHECK_MILLIES = millis() - 15000;
  LAST_MOTION_CHECK_MILLIES = millis() - 15000;

  DETECTION_COUNTER = ohoco.config.maxValue;
  MOTION_COUNTER = 0;
  LAST_DETECTION_STATE = NOBODY_IS_HERE;

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
  ohoco.register_switch("fan", "fan");
  ohoco.register_sensor("fantemp", "temperature", "°C");
  ohoco.register_sensor("human", "presence", "");

  ohoco.on_message(ohoco_callback);

  // Start the server
  ///////////////////////////// Request commands
  myWebServer.on("/", handleRootRequest);
  myWebServer.onNotFound(handleNotFoundRequest);  // Handle when a client requests an unknown URI for example something other than "/")
  ///////////////////////////// End of Request commands
  myWebServer.begin();
  ohoco.println("HTTP server started");
  
  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();
  myWebServer.handleClient();

  unsigned long now = millis();

  if ((now - LAST_TEMP_CHECK_MILLIES) > ohoco.config.checkInterval) {
    ohoco.println("reading DS18B20...");
    ohoco.led_flash(2, 100);

    float temperature = 0;
    
    DS18B20.requestTemperatures();
    temperature = DS18B20.getTempCByIndex(0);
    if ((temperature < 85) && (temperature > -127)) {
      if (temperature != LAST_TEMPERATURE) {
        char TemperatureCString[6];
        dtostrf(temperature, 2, 1, TemperatureCString);
        ConvertPointToComma(TemperatureCString);
        ohoco.sensor_update("fantemp", TemperatureCString);
        LAST_TEMPERATURE = temperature;
      }
      else {
        ohoco.println("same Temperature is " + String(temperature) + " °C");
      }
    }
    else {
      ohoco.print("reading DS18B20 failed");
    }

    LAST_TEMP_CHECK_MILLIES = millis();
  }

  if ((now - LAST_MOTION_CHECK_MILLIES) > MS_BETWEEN_DETECTIONS) {
    int THIS_DETECTION_STATE = digitalRead(SENSOR_PIN);
    
//    ohoco.debuginline("DC: ");
//    ohoco.debuginline(DETECTION_COUNTER);
//    ohoco.debuginline(" / MC: ");
//    ohoco.debuginline(MOTION_COUNTER);
//    ohoco.debuginline(" / LS: ");
//    ohoco.debuginline(LAST_DETECTION_STATE);
//    ohoco.debuginline(" / TS: ");
//    ohoco.debug(THIS_DETECTION_STATE);

    if (THIS_DETECTION_STATE == SOMEONE_IS_HERE) {
      if (LAST_DETECTION_STATE == SOMEONE_IS_HERE) {
        DETECTION_COUNTER = ohoco.config.maxValue;
        ohoco.print("someone still here             - reset DETECTION_COUNTER to ");
        ohoco.println(DETECTION_COUNTER);
      }
      else {
        MOTION_COUNTER++;

        if (MOTION_COUNTER >= ohoco.config.minValue) {
          ohoco.println("new detection (2 times)        - fire TRIGGER & set STATUS");
          
          ohoco.trigger_activate(ohoco.config.inTrigger);
          ohoco.sensor_update("human", "here");
          ohoco.led_on();

          DETECTION_COUNTER = ohoco.config.maxValue;
          MOTION_COUNTER = 1;
          LAST_DETECTION_STATE = SOMEONE_IS_HERE;
        }
        else {
          ohoco.println("new detection                  - check TWICE");
        }
      }
    }
    else {
      if (LAST_DETECTION_STATE == SOMEONE_IS_HERE) {
        DETECTION_COUNTER -= MS_BETWEEN_DETECTIONS;
        ohoco.print("someone was here               - decrease DETECTION_COUNTER to ");
        ohoco.println(DETECTION_COUNTER);

        if (DETECTION_COUNTER <= 0) {
          ohoco.println("someone was here for some time - fire TRIGGER & set STATUS");
          
          ohoco.trigger_activate(ohoco.config.outTrigger);
          ohoco.sensor_update("human", "away");
          ohoco.led_off();

          LAST_DETECTION_STATE = NOBODY_IS_HERE;
          DETECTION_COUNTER = 0;
        }
      }
      else {
        ohoco.println("still nobody here");
        if (MOTION_COUNTER > 0) {
          ohoco.println("reset MOTION_COUNTER");
          MOTION_COUNTER = 0;
        }
      }
    }
    
    LAST_MOTION_CHECK_MILLIES = millis();
  }
}

void executeCommand(String cmd) {
//  int pos = cmd.indexOf(':');
//  String sSocket = cmd.substring(0, pos);
//  String sAction = cmd.substring(pos+1);

  if (cmd == "fan:on") {
    HO5500RE_POWER();
    ohoco.sensor_update("fan", "on");
  }
  else if (cmd == "fan:off") {
    HO5500RE_POWER();
    ohoco.sensor_update("fan", "off");
  }
  else if (cmd == "fan:speed") {
    HO5500RE_SPEED();
  }
  else if (cmd == "fan:move") {
    HO5500RE_MOVER();
  }
  else if (cmd == "fan:timer") {
    HO5500RE_TIMER();
  }
  else if (cmd == "fan:scene") {
    HO5500RE_SCENE();
  }
  else if (cmd == "DEBUG") {
    float temperature = 0;
    
    DS18B20.requestTemperatures();
    temperature = DS18B20.getTempCByIndex(0);
    
    char TemperatureCString[6];
    
    dtostrf(temperature, 2, 1, TemperatureCString);
    
    ohoco.sensor_update("fantemp", TemperatureCString);
  }

  ohoco.led_flash(3, 50);
}

void handleRootRequest() {
  ohoco.println("handleRootRequest");

  if (myWebServer.args() > 0) {
    String cmd;
    cmd = myWebServer.arg("cmd");
  
    executeCommand(cmd);
  }

  IPAddress ip = WiFi.localIP();
  char strIP[24];
  sprintf(strIP, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
  
  String resp;

  resp  = "<html><header><meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'><title>Status</title>";
  resp += "<style>";
  resp += "span { display: inline-block; width: 145px; }";
  resp += "</style>";
  resp += "</header><body>";
  resp += "<h3>System Status</h3>";
  resp += "<fieldset><legend>Device Information</legend>";
  resp += "<span>Firmware Version:</span> " + String(SKETCH_VERSION) + "</br>";
  resp += "<span>Device Name:</span> " + String(WIFI_DEVICE_NAME) + "</br>";
  resp += "<span>Connected SSID:</span> " + WiFi.SSID() + "</br>";
  resp += "<span>Device IP:</span> " + String(strIP) + "</br>";
  resp += "<span>Uptime:</span> " + String(millis()) + " ms</br>";
  resp += "<span>Uptime:</span> " + String((millis() / 8640000)) + " days</br>";
  resp += "</fieldset><br>";

  resp += "<fieldset><legend>Actions</legend>";
  resp += "<a href='?cmd=fan:on'>Switch Fan on</a></br></br>";
  resp += "<a href='?cmd=fan:off'>Switch Fan off</a></br></br>";
  resp += "<a href='?cmd=fan:speed'>Toggle Speed</a></br></br>";
  resp += "<a href='?cmd=fan:move'>Start/Stop moving</a></br></br>";
  resp += "<a href='?cmd=fan:timer'>Set Timer</a></br></br>";
  resp += "<a href='?cmd=fan:scene'>Set Scene</a></br></br>";
  resp += "<a href='?cmd=DEBUG'>DEBUG</a></br></br>";
  resp += "</fieldset><br>";

  resp += "</body></html>";

  myWebServer.send(200, "text/html", resp);
}

void handleNotFoundRequest(){
  ohoco.println("handleNotFoundRequest: " + myWebServer.uri());
  myWebServer.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void ohoco_callback(String topic, String payload) {
  ohoco.println("ohoco_callback (" + payload + ") on (" + topic + ")");

  executeCommand(payload);
}

void ConvertPointToComma(char* buf) {
  for (int i=0; i<strlen(buf); i++) {
    if (buf[i] == '.')
      buf[i] = ',';
  }
}

