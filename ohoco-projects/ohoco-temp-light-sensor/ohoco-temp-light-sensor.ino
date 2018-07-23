ADC_MODE(ADC_VCC);

#include <OHoCo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "./credentials.h"

const char*  SKETCH_VERSION     = "2018-07-23";
const char*  WIFI_DEVICE_NAME   = "ESP-Temp-Light-Sensor";

#define ONE_WIRE_BUS_PIN          5
#define LDR_PIN                   4

// Setup OHoCo Libary
OHoCo ohoco;
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS_PIN);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);

// global variables
char TemperatureCString[6];
char TemperatureFString[6];
unsigned long LAST_CHECK_MILLIES;
int LDR_LAST_STATE;

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

  pinMode(LDR_PIN, INPUT);
  
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
  ohoco.register_sensor("TempOutside", "temperature");
  ohoco.register_sensor("DayNight", "daynight");

  ohoco.on_message(ohoco_callback);
  
  ohoco.setup_ready();
}

void loop() {
  ohoco.keepalive();

  if ((millis() - LAST_CHECK_MILLIES) > ohoco.config.checkInterval) {
    getTemperature();
    //ohoco.debug("Temperature  is " + String(TemperatureCString) + " °C");
    ohoco.set_sensor_value("TempOutside", TemperatureCString, " °C");

    // --------------------------

    int LDR_THIS_STATE = digitalRead(LDR_PIN);

    if (LDR_THIS_STATE != LDR_LAST_STATE) {
      if (LDR_THIS_STATE == HIGH) {
        ohoco.trigger_activate("Sunset");
        ohoco.set_sensor_value("DayNight", "off", "");
      }
      else {
        ohoco.trigger_activate("Sunrise");
        ohoco.set_sensor_value("DayNight", "on", "");
      }
      
      LDR_LAST_STATE = LDR_THIS_STATE;
    }
    
    // --------------------------
    
    ohoco.led_flash(2, 100);
    
    LAST_CHECK_MILLIES = millis();
  }

  delay(1000);
}

void ohoco_callback(String cmd) {
  if (cmd == "debug") {
    getTemperature();
    ohoco.set_sensor_value("TempOutside", TemperatureCString, " °C");

    if (digitalRead(LDR_PIN) == HIGH) {
      ohoco.set_sensor_value("DayNight", "off", "");
    }
    else {
      ohoco.set_sensor_value("DayNight", "on", "");
    }
  }
}

void ConvertPointToComma(char* buf) {
  for (int i=0; i<strlen(buf); i++) {
    if (buf[i] == '.')
      buf[i] = ',';
  }
}

// #####################################################
// read temperature from DS18B20
// #####################################################
void getTemperature() {
  float tempC;

  DS18B20.requestTemperatures();
  tempC = DS18B20.getTempCByIndex(0);
  if ((tempC < 85) && (tempC > -127))
    dtostrf(tempC, 2, 1, TemperatureCString); //dtostrf(tempC, 2, 2, TemperatureCString);
  else
    sprintf(TemperatureCString, "%s", "-.-");
  ConvertPointToComma(TemperatureCString);
  delay(100);
}
