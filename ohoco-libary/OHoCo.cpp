#include "OHoCo.h"

const char*  OHOCO_VERSION     = "2018-07-23";

WiFiClient WiFiClient;
PubSubClient MQTTClient(WiFiClient);
WiFiUDP UdpServer;

OHoCo::OHoCo() {
  this->DEBUGMODE = SERIAL;
  this->LED_PIN = BUILTIN_LED;
  this->_WIFI_DHCP_MODE = true;
  this->_USE_MQTT = false;
  this->_is_wifi_connected = false;
  this->_is_mqtt_connected = false;
  this->_CLIENT_ID = "";
  this->_CallbackFunction = NULL;
}

String macToStr(const uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += '-';
  }
  return result;
}

void OHoCo::initialize() {
  if (this->DEBUGMODE == SERIAL) {
    Serial.begin(115200);
    Serial.setTimeout(2000);
    Serial.println();
    Serial.println();
    Serial.println("---------------------------------------------");
    Serial.println("SYS  >> Serial output initialized");
    delay(10);
  }
  else if (this->DEBUGMODE == LED) {
    pinMode(this->LED_PIN, OUTPUT);
    digitalWrite(this->LED_PIN, LOW); // turn led on
  }

  // generate CLIENT_ID
  String clientName;
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += "esp-";
  for (int i = 0; i < 6; ++i) {
    clientName += String(mac[i], 16);
  }
  
  this->_CLIENT_ID = clientName;
  this->debug("SYS  >> CLIENT_ID is " + clientName);

  this->config_read();
}

void OHoCo::setup_ready() {
  if (this->DEBUGMODE == SERIAL) {
    Serial.println("SYS  >> Setup ready");
    Serial.println("---------------------------------------------");
  }
  else if (this->DEBUGMODE == LED) {
    this->led_flash(5, 100);
  }
}

void OHoCo::debugmode(int dbgmode) {
   this->DEBUGMODE = dbgmode;
}

void OHoCo::debugmode(int dbgmode, int ledpin) {
   this->DEBUGMODE = dbgmode;
   this->LED_PIN = ledpin;
}

void OHoCo::debug(String msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.println(msg);
}

void OHoCo::debug(int msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.println(msg);
}

void OHoCo::debug(long msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.println(msg);
}

void OHoCo::debug(float msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.println(msg);
}

void OHoCo::debuginline(String msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.print(msg);
}

void OHoCo::debuginline(int msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.print(msg);
}

void OHoCo::led_on() {
  if (this->DEBUGMODE == LED)
    digitalWrite(this->LED_PIN, LOW); // turn led on
  else if (this->DEBUGMODE == SERIAL)
    Serial.println("LED ON");
}

void OHoCo::led_off() {
  if (this->DEBUGMODE == LED)
    digitalWrite(this->LED_PIN, HIGH); // turn led off
  else if (this->DEBUGMODE == SERIAL)
    Serial.println("LED OFF");
}

void OHoCo::led_flash(int cnt, int delayms = 500) {
  if (this->DEBUGMODE == LED) {
    for (int i=0; i<cnt; i++) {
      digitalWrite(this->LED_PIN, LOW); // turn led on
      delay(delayms);
      digitalWrite(this->LED_PIN, HIGH); // turn led off
      delay(delayms);
    }
  }
  else if (this->DEBUGMODE == SERIAL)
    Serial.println("LED FLASHING " + String(cnt) + " TIMES");
}

void OHoCo::wifi_connect() {
  int connect_timeout_sec = 20;

  WiFi.mode(WIFI_STA);
  WiFi.hostname(this->config.displayName);
  
  this->debuginline("SYS  >> Initialize WiFi - trying to connect to " + String(this->config.wifi_ssid) + " ");
  WiFi.begin(this->config.wifi_ssid, this->config.wifi_pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    this->debuginline(".");
    connect_timeout_sec--;
    if (connect_timeout_sec <= 0) {
      this->debug(" WiFi connection failed");
      this->_is_wifi_connected = false;
      return;
    }
  }
  this->debug(" WiFi connected");

//  if (this->_WIFI_DHCP_MODE == false) {
//    WiFi.config(this->_WIFI_IP, this->_WIFI_GATEWAY, this->_WIFI_SUBNET);
//    //WiFi.setDNS(this->_WIFI_DNS);
//  }
  
  IPAddress ip = WiFi.localIP();
  char strIP[24];
  sprintf(strIP, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
  this->debug("IP     " + String(strIP));

//  if (this->_WIFI_DHCP_MODE == false) {
//    sprintf(strIP, "%d.%d.%d.%d", this->_WIFI_SUBNET[0],this->_WIFI_SUBNET[1],this->_WIFI_SUBNET[2],this->_WIFI_SUBNET[3]);
//    this->debug("SUBNET " + String(strIP));
//    sprintf(strIP, "%d.%d.%d.%d", this->_WIFI_GATEWAY[0],this->_WIFI_GATEWAY[1],this->_WIFI_GATEWAY[2],this->_WIFI_GATEWAY[3]);
//    this->debug("GATWAY " + String(strIP));
//    //sprintf(strIP, "%d.%d.%d.%d", this->_WIFI_DNS[0],this->_WIFI_DNS[1],this->_WIFI_DNS[2],this->_WIFI_DNS[3]);
//    //this->debug("DNS     " + String(strIP));
//  }

  this->_is_wifi_connected = true;
}

void OHoCo::wifi_disconnect() {
  this->debug("Disable WiFi");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(10);
  
  this->_is_wifi_connected = false;
}

bool OHoCo::wifi_connected() {
  if (WiFi.status() == WL_CONNECTED)
    this->_is_wifi_connected = true;
  else
    this->_is_wifi_connected = false;
  return this->_is_wifi_connected;
}

void OHoCo::http_setup() {
  this->_USE_MQTT = false;
  this->_API_HOST = this->config.controller_ip;
  this->_API_PORT = this->config.controller_port;
  this->_API_USER = this->config.controller_user;
  this->_API_PASS = this->config.controller_pass;

  this->debug("UDP  >> Starting UDP Server on port 18266");
  UdpServer.begin(18266);
}

void OHoCo::http_publish(char* topic, char* payload) {
  String url = "http://" + String(this->_API_HOST) + "/rift3/api/" + String(topic);

  String strPayload = String(payload);
  strPayload.replace("+", "%2B");
  
  //this->debug(url);
  //this->debug(payload);
  //this->debug(strPayload);

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("payload=" + strPayload);
  String response = http.getString();

  if (response != "OK") {
    this->debug(response);
  }
  
  http.end();
}

void OHoCo::mqtt_setup() {
  this->_USE_MQTT = true;
  this->_API_HOST = this->config.controller_ip;
  this->_API_PORT = this->config.controller_port;
  this->_API_USER = this->config.controller_user;
  this->_API_PASS = this->config.controller_pass;

  IPAddress addr;
  if (addr.fromString(this->_API_HOST)) {
    // it was a valid address, do something with it 
    MQTTClient.setServer(addr, this->_API_PORT);
    MQTTClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->mqtt_callback(topic, payload, length); });
  }
  else
    this->debug("mqtt_setup: invalid ip adress");
}

void OHoCo::mqtt_connect() {
  this->debuginline("SYS  >> Connecting to MQTT Broker ");
  this->debuginline(this->_API_HOST);
  this->debuginline(":");
  this->debuginline(this->_API_PORT);
  this->debuginline(" ");

  int connect_timeout_sec = 10;
  
  while (!MQTTClient.connected()) {
    //MQTTClient.connect(this->_CLIENT_ID.c_str(), this->_API_USER, this->_API_PASS); //, "/debug/lwt", 0, 0, "off");
    MQTTClient.connect(this->_CLIENT_ID.c_str());

    if (MQTTClient.connected())
      break;
    
    delay(1000);
    this->debuginline(".");
    connect_timeout_sec--;
    if (connect_timeout_sec <= 0) {
      this->debuginline(" connection failed, responsecode = ");
      this->debug(MQTTClient.state());
      this->_is_mqtt_connected = false;
      return;
    }
  }
  this->debug(" MQTT client connected");
  
  if (MQTTClient.connected()) {
    this->_MQTT_DEVICE_TOPIC = "ohoco/callback/" + this->_CLIENT_ID;
    MQTTClient.subscribe(this->_MQTT_DEVICE_TOPIC.c_str());
    this->debug("MQTT >> topic subscribed " + this->_MQTT_DEVICE_TOPIC);

//    MQTTClient.subscribe("ntp/OUT");
//    Publish("debug", String(CLIENT_ID) + " [LOGGED IN]");
//    Publish("ntp/IN", JsonString(String(MQTT_SUBSCRIBE_TOPIC), ""));
  }

  this->_is_mqtt_connected = MQTTClient.connected();
}

void OHoCo::mqtt_disconnect() {
  this->debug("MQTT >> Disconnectiong");
  MQTTClient.disconnect();
  delay(10);
  this->_is_mqtt_connected = MQTTClient.connected();
}

bool OHoCo::mqtt_connected() {
  this->_is_mqtt_connected = MQTTClient.connected();
  return _is_mqtt_connected;
}

void OHoCo::mqtt_callback(char* rtopic, byte* rpayload, unsigned int length) {
  // Generate String from reveived topic
  String topic = rtopic;
  // Generate String from received payload 
  String payload = String();
  for (int i = 0; i < length; i++) {
    char input_char = (char)rpayload[i];
    payload += input_char;
  }

  this->debug("MQTT MESSAGE RECEIVED");
  this->debug("TOPIC   [" + topic + "]");
  this->debug("PAYLOAD [" + payload + "]");

  if (topic == this->_MQTT_DEVICE_TOPIC) {
    this->debug("DEVICE COMMAND");
    this->config_command(String(payload));
  }
}

void OHoCo::mqtt_subscribe(char* topic) {
  if (MQTTClient.connected()) {
    if (MQTTClient.subscribe(topic))
      this->debug("MQTT >> topic subscribed " + String(topic));
    else
      this->debug("MQTT >> subscribe error :: return == false");
  }
  else {
    this->debug("MQTT >> subscribe error :: mqtt not connected");
  }
}

void OHoCo::mqtt_publish(char* topic, char* payload, boolean retained) {
  if (MQTTClient.connected()) {
    if (strlen(payload) <= 106) {
      if (MQTTClient.publish(topic, payload, retained)) {
        if (retained)
          this->debug("MQTT >> publish [" + String(topic) + "] -> " + String(payload) + " (RETAINED)");
        else
          this->debug("MQTT >> publish [" + String(topic) + "] -> " + String(payload));
      }
      else
        this->debug("MQTT >> publish error :: return == false");
    }
    else
      this->debug("MQTT >> publish error :: payload greater 106 Bytes");
  }
  else {
    this->debug("MQTT >> publish error :: mqtt not connected");
  }
}

void OHoCo::keepalive() {
//  this->debug("OHoCo::keepalive()");

  if (((millis() - this->_connection_timer) > 12000UL) && (WiFi.status() != WL_CONNECTED)) {
    this->_connection_timer = millis();
    this->wifi_connect();
  }
  if (((millis() - this->_alive_ping_timer) > 3600000) && (WiFi.status() == WL_CONNECTED)) {
    this->_alive_ping_timer = millis();
    this->send_alive_ping();
  }
  if (this->_USE_MQTT) {
    if (((millis() - this->_connection_timer) > 1000UL) && (WiFi.status() == WL_CONNECTED) && (!MQTTClient.connected())) {
      this->mqtt_connect();
      this->register_device("");
    }
  
    if (MQTTClient.connected() == true)
      MQTTClient.loop();   
  }
  else {
    // if there's data available, read a packet
    int packetSize = UdpServer.parsePacket();
    if (packetSize) {
      char packetBuffer[255]; //buffer to hold incoming packet
      
      if (this->DEBUGMODE == SERIAL) {
        IPAddress remoteIp = UdpServer.remoteIP();
        char strRemote[128];
        sprintf(strRemote, "%d.%d.%d.%d : %d", remoteIp[0],remoteIp[1],remoteIp[2],remoteIp[3],UdpServer.remotePort());
        this->debug("UDP  >> MESSAGE RECEIVED FROM " + String(strRemote));
      }

      // read the packet into packetBufffer
      int len = UdpServer.read(packetBuffer, 255);
      if (len > 0) {
        packetBuffer[len] = 0;
      }
//      this->debug(packetBuffer);

      this->config_command(String(packetBuffer));
    }
  }
  
  delay(1);
}

void OHoCo::send_alive_ping() {
  this->debug("API  >> Send alive ping");

  long rssi_dbm = WiFi.RSSI();
  int rssi_percent;

  if (rssi_dbm <= -100)
    rssi_percent = 0;
  else if (rssi_dbm >= -50)
    rssi_percent = 100;
  else
    rssi_percent = 2 * (rssi_dbm + 100);

  float voltage = 0.00f;
  voltage = ESP.getVcc();
  
  String payload;
  payload = "ws:" + String(rssi_percent) +" %|vc:" + String(voltage/1024.00f) + " V";

  char topic[128] = "ohoco/ping/";
  strcat(topic, this->_CLIENT_ID.c_str());

  char pload[strlen(payload.c_str())+1];
  payload.toCharArray(pload, strlen(payload.c_str())+1);
  
  if (this->_USE_MQTT)
    this->mqtt_publish(topic, pload, true);
  else
    this->http_publish(topic, pload);
}

void OHoCo::send_log(char* msg) {
  this->debug("API  >> Send log: " + String(msg));

  char topic[128] = "ohoco/log/";
  strcat(topic, this->_CLIENT_ID.c_str());

  if (this->_USE_MQTT)
    this->mqtt_publish(topic, msg, false);
  else
    this->http_publish(topic, msg);
}

void OHoCo::register_device(const char* sketch_version) {
  this->debug("API  >> Register device");

  if (strlen(sketch_version) > 1)
    this->_SKETCH_VERSION = sketch_version;

  IPAddress ip = WiFi.localIP();
  char strIP[24];
  sprintf(strIP, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);

  long rssi_dbm = WiFi.RSSI();
  int rssi_percent;

  if (rssi_dbm <= -100)
    rssi_percent = 0;
  else if (rssi_dbm >= -50)
    rssi_percent = 100;
  else
    rssi_percent = 2 * (rssi_dbm + 100);

  float voltage = 0.00f;
  voltage = ESP.getVcc();
  
  String payload;
  payload = "REG:" + String(this->config.displayName) + "|ip:" + String(strIP) + "|wn:" + String(this->config.wifi_ssid) + "|ws:" + String(rssi_percent) +" %|vc:" + String(voltage/1024.00f) + " V|ov:" + String(OHOCO_VERSION) + "|sv:" + String(this->_SKETCH_VERSION);
  
  char topic[128] = "ohoco/device/";
  strcat(topic, this->_CLIENT_ID.c_str());
  
  char pload[strlen(payload.c_str())+1];
  payload.toCharArray(pload, strlen(payload.c_str())+1);
  
//  this->debug(topic);
//  this->debug(pload);

  if (this->_USE_MQTT)
    this->mqtt_publish(topic, pload, false);
  else
    this->http_publish(topic, pload);

  delay(100);
}

void OHoCo::register_sensor(char* sensor_name, char* sensor_type) {
  this->debug("API  >> Register sensor");

  char topic[128] = "ohoco/sensor/";
  strcat(topic, sensor_name);
  
//  this->debug(topic);
//  this->debug(sensor_type);

  if (this->_USE_MQTT)
    this->mqtt_publish(topic, sensor_type, true);
  else
    this->http_publish(topic, sensor_type);

  delay(100);
}

void OHoCo::register_switch(char* switch_name, char* switch_type) {
  this->debug("API  >> Register switch");

  char topic[128] = "ohoco/switch/";
  strcat(topic, switch_name);
  
  char pload[128] = {0};
  strcat(pload, this->_CLIENT_ID.c_str());
  strcat(pload, ";");
  strcat(pload, switch_type);
  
//   this->debug(topic);
//   this->debug(pload);

  if (this->_USE_MQTT)
    this->mqtt_publish(topic, pload, true);
  else
    this->http_publish(topic, pload);

  delay(250);
}

void OHoCo::set_sensor_value(char* sensor_name, char* sensor_value, char* sensor_unit) {
  char topic[128] = "ohoco/status/";
  strcat(topic, sensor_name);
  
  char pload[128] = {0};
  strcat(pload, sensor_value);
  strcat(pload, sensor_unit);

  this->debug("API  >> Updating sensor " + String(sensor_name) +  " -> " + String(pload));
  
//  this->debug(topic);
//  this->debug(pload);
  
  if (this->_USE_MQTT)
    this->mqtt_publish(topic, pload, true);
  else
    this->http_publish(topic, pload);
}

void OHoCo::trigger_activate(char* trigger_name) {
  this->debug("API  >> Trigger " + String(trigger_name) + " activated");
  
  char topic[128] = "ohoco/trigger/";
  strcat(topic, trigger_name);

  if (this->_USE_MQTT)
    this->mqtt_publish(topic, "fire", false);
  else
    this->http_publish(topic, "fire");
}

bool OHoCo::config_read() {
  this->debug("CFG  >> Reading configuration from EEEPROM");
  
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(4095);
  EEPROM.get(0, this->config);
  EEPROM.end();

  if (this->config.valid != 34) {
    // Configuration not set or not valid
    return false;
  }
  return true;
}

void OHoCo::config_write() {
  this->debug("CFG  >> Writing configuration to EEEPROM");
  
  // Mark record in EEPROM as valid data. EEPROM erased -> Valid=0 / old config between 1 and current value
  this->config.valid = 34;
  
  // Save configuration from RAM into EEPROM
  EEPROM.begin(4095);
  EEPROM.put(0, this->config);
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure

  this->send_log("WRITECFG OK");
}

void OHoCo::config_reset() {
  this->debug("CFG  >> Reset configuration in EEEPROM");
  
  // Reset EEPROM bytes to '0' for the length of the data structure
  EEPROM.begin(4095);
  for (int i = 0 ; i < sizeof(this->config) ; i++) {
    EEPROM.write(i, 0);
  }
  delay(200);
  EEPROM.commit();
  EEPROM.end();

  this->send_log("RESETCFG OK");
}

void OHoCo::config_set(String cmd) {
  int pos = cmd.indexOf(':');
  String kvp = cmd.substring(pos+1);
  pos = kvp.indexOf('=');
  String key = kvp.substring(0, pos);
  String val = kvp.substring(pos+1);

  this->debug("CFG  >> Set configuration [" + key + "] -> [" + val + "]");

  if (key == "checkInterval") {
    this->config.checkInterval = val.toInt();
    if (this->config.checkInterval < 100)
      this->config.checkInterval = 100;
  }
  else if (key == "minValue") {
    this->config.minValue = val.toInt();
    if (this->config.minValue < 1)
      this->config.minValue = 1;
  }
  else if (key == "maxValue") {
    this->config.maxValue = val.toInt();
    if (this->config.maxValue > 32768)
      this->config.maxValue = 32768;
  }
  else if (key == "displayName")
    strcpy(this->config.displayName, val.c_str());
  else if (key == "dataTopic")
    strcpy(this->config.dataTopic, val.c_str());
  else if (key == "inTrigger")
    strcpy(this->config.inTrigger, val.c_str());
  else if (key == "outTrigger")
    strcpy(this->config.outTrigger, val.c_str());
  else
    this->debug("CFG  >> Unknown configuration key [" + key + "]");

//  this->config_display();
}

void OHoCo::config_send() {
  this->debug("API  >> Send config to controller");

  String payload;
  payload = "checkInterval:" + String(this->config.checkInterval) + "|minValue:" + String(this->config.minValue) + "|maxValue:" + String(this->config.maxValue) + "|inTrigger:" + String(this->config.inTrigger) + "|outTrigger:" + String(this->config.outTrigger) + "|WiFi-SSID:" + String(this->config.wifi_ssid) + "|WiFi-PASS:" + String(this->config.wifi_pass) + "|Controller-IP:" + String(this->config.controller_ip) + "|Controller-Port:" + String(this->config.controller_port);

  char topic[128] = "ohoco/config/";
  strcat(topic, this->_CLIENT_ID.c_str());
  
  char pload[strlen(payload.c_str())+1];
  payload.toCharArray(pload, strlen(payload.c_str())+1);
  
  //this->debug(topic);
  //this->debug(pload);

  if (this->_USE_MQTT)
    this->mqtt_publish(topic, pload, false);
  else
    this->http_publish(topic, pload);
}

void OHoCo::config_display() {
  this->debug("CFG  >> Current configuration\n");
  if (this->config.valid == 34)
    this->debug("config source  : EEPROM");
  else
    this->debug("config source  : DEFAULTS");
  this->debug("displayName    : " + String(this->config.displayName));
  this->debug("wifi_ssid      : " + String(this->config.wifi_ssid));
  this->debug("wifi_pass      : " + String(this->config.wifi_pass));
  this->debug("controller_ip  : " + String(this->config.controller_ip));
  this->debug("controller_port: " + String(this->config.controller_port));
  this->debug("controller_user: " + String(this->config.controller_user));
  this->debug("controller_pass: " + String(this->config.controller_pass));
  this->debug("useMQTT        : " + String(this->config.useMQTT));
  this->debug("checkInterval  : " + String(this->config.checkInterval));
  this->debug("minValue       : " + String(this->config.minValue));
  this->debug("maxValue       : " + String(this->config.maxValue));
  this->debug("dataTopic      : " + String(this->config.dataTopic));
  this->debug("inTrigger      : " + String(this->config.inTrigger));
  this->debug("outTrigger     : " + String(this->config.outTrigger));
  this->debug("genericValue01 : " + String(this->config.genericValue01));
  this->debug("genericValue02 : " + String(this->config.genericValue02));
  this->debug("genericValue03 : " + String(this->config.genericValue03));
  this->debug("genericValue04 : " + String(this->config.genericValue04));
  this->debug("genericValue05 : " + String(this->config.genericValue05));
  this->debug("genericValue06 : " + String(this->config.genericValue06));
  this->debug("genericValue07 : " + String(this->config.genericValue07));
  this->debug("genericValue08 : " + String(this->config.genericValue08));
  this->debug("genericValue09 : " + String(this->config.genericValue09));
  this->debug("genericValue10 : " + String(this->config.genericValue10));
  this->debug("");
}

void OHoCo::config_command(String cmd) {
//  this->debug("CFG  >> DEVICE COMMAND (" + cmd + ")");

  if (cmd.substring(0, 3) == "REG") {
    this->debug("CFG  >> Was a register message");
  }
  else if (cmd == "REREG") {
    this->register_device("");
  }
  else if (cmd == "REBOOT") {
    this->reboot();
  }
  else if (cmd == "SENDCFG") {
    this->config_send();
  }
  else if (cmd == "WRITECFG") {
    this->config_write();
  }
  else if (cmd == "RESETCFG") {
    this->config_reset();
  }
  else if (cmd == "PING") {
    this->send_alive_ping();
  }
  else if (cmd.substring(0, 3) == "CFG") {
    this->config_set(cmd);
  }
  else {
    if (this->_CallbackFunction != NULL)
      this->_CallbackFunction(cmd);
    else
      this->debug("CFG  >> UNKNOWN COMMAND (" + cmd + ") and no onMessage-Callback-Function set");
  }
}

void OHoCo::on_message(void (*CallbackFunc)(String)) {
  this->_CallbackFunction = CallbackFunc;
}

void OHoCo::reboot() {
  this->debug("SYS  >> Rebooting...");

  this->send_log("REBOOT OK");
  
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while(1)
    wdt_reset();
}

