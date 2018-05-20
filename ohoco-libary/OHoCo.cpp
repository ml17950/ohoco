#include "OHoCo.h"

const char*  OHOCO_VERSION     = "2018-04-14";

WiFiClient WiFiClient;
PubSubClient MQTTClient(WiFiClient);
WiFiUDP UdpServer;

OHoCo::OHoCo() {
  this->_debugmode = SERIAL;
  this->_WIFI_DHCP_MODE = true;
  this->_use_mqtt = false;
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
  if (_debugmode == SERIAL) {
    Serial.begin(115200);
    Serial.setTimeout(2000);
    Serial.println();
    Serial.println();
    Serial.println("---------------------------------------------");
    Serial.println("SYS >> Serial output initialized");
    delay(10);
  }
  else if (_debugmode == LED) {
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW); // turn led on
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
  this->debug("SYS >> CLIENT_ID is " + clientName);

  this->config_read();
}

void OHoCo::setup_ready() {
  if (_debugmode == SERIAL) {
    Serial.println("SYS >> Setup ready");
    Serial.println("---------------------------------------------");
  }
  else if (_debugmode == LED) {
    this->led_flash(5, 100);
  }
}

void OHoCo::debugmode(int dbgmode) {
  _debugmode = dbgmode;
}

void OHoCo::debug(String msg) {
  if (_debugmode == SERIAL)
    Serial.println(msg);
}

void OHoCo::debug(int msg) {
  if (_debugmode == SERIAL)
    Serial.println(msg);
}

void OHoCo::debug(long msg) {
  if (_debugmode == SERIAL)
    Serial.println(msg);
}

void OHoCo::debug(float msg) {
  if (_debugmode == SERIAL)
    Serial.println(msg);
}

void OHoCo::debuginline(String msg) {
  if (_debugmode == SERIAL)
    Serial.print(msg);
}

void OHoCo::led_on() {
  if (_debugmode == LED)
    digitalWrite(BUILTIN_LED, LOW); // turn led on
  else if (_debugmode == SERIAL)
    Serial.println("LED ON");
}

void OHoCo::led_off() {
  if (_debugmode == LED)
    digitalWrite(BUILTIN_LED, HIGH); // turn led off
  else if (_debugmode == SERIAL)
    Serial.println("LED OFF");
}

void OHoCo::led_flash(int cnt, int delayms = 500) {
  if (_debugmode == LED) {
    for (int i=0; i<cnt; i++) {
      digitalWrite(BUILTIN_LED, LOW); // turn led on
      delay(delayms);
      digitalWrite(BUILTIN_LED, HIGH); // turn led off
      delay(delayms);
    }
  }
  else if (_debugmode == SERIAL)
    Serial.println("LED FLASHING");
}

void OHoCo::wifi_config(const char* WIFI_NAME, const char* WIFI_SSID, const char* WIFI_PASS) {
  this->_WIFI_DHCP_MODE = true;
  this->_WIFI_NAME = WIFI_NAME;
  this->_WIFI_SSID = WIFI_SSID;
  this->_WIFI_PASS = WIFI_PASS;
}

void OHoCo::wifi_config(const char* WIFI_NAME, const char* WIFI_SSID, const char* WIFI_PASS, const char* WIFI_IP, const char* WIFI_DNS, const char* WIFI_GATEWAY) {
  this->_WIFI_DHCP_MODE = false;
  this->_WIFI_NAME = WIFI_NAME;
  this->_WIFI_SSID = WIFI_SSID;
  this->_WIFI_PASS = WIFI_PASS;
  
  this->_WIFI_IP.fromString(WIFI_IP);
  this->_WIFI_DNS.fromString(WIFI_DNS);
  this->_WIFI_GATEWAY.fromString(WIFI_GATEWAY);
  this->_WIFI_SUBNET.fromString("255.255.255.0");
}

void OHoCo::wifi_connect() {
  int connect_timeout_sec = 20;

  WiFi.mode(WIFI_STA);
  WiFi.hostname(this->_WIFI_NAME);
  
  this->debuginline("SYS >> Initialize WiFi - trying to connect to " + String(this->_WIFI_SSID) + " ");
  WiFi.begin(this->_WIFI_SSID, this->_WIFI_PASS);
  
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

  if (this->_WIFI_DHCP_MODE == false) {
    WiFi.config(this->_WIFI_IP, this->_WIFI_GATEWAY, this->_WIFI_SUBNET);
    //WiFi.setDNS(this->_WIFI_DNS);
  }
  
  IPAddress ip = WiFi.localIP();
  char strIP[24];
  sprintf(strIP, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
  this->debug("IP     " + String(strIP));

  if (this->_WIFI_DHCP_MODE == false) {
    sprintf(strIP, "%d.%d.%d.%d", this->_WIFI_SUBNET[0],this->_WIFI_SUBNET[1],this->_WIFI_SUBNET[2],this->_WIFI_SUBNET[3]);
    this->debug("SUBNET " + String(strIP));
    sprintf(strIP, "%d.%d.%d.%d", this->_WIFI_GATEWAY[0],this->_WIFI_GATEWAY[1],this->_WIFI_GATEWAY[2],this->_WIFI_GATEWAY[3]);
    this->debug("GATWAY " + String(strIP));
    //sprintf(strIP, "%d.%d.%d.%d", this->_WIFI_DNS[0],this->_WIFI_DNS[1],this->_WIFI_DNS[2],this->_WIFI_DNS[3]);
    //this->debug("DNS     " + String(strIP));
  }

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

void OHoCo::http_config(const char* HTTP_HOST, const int HTTP_PORT, const char* HTTP_USER, const char* HTTP_PASS) {
  this->_use_mqtt = false;
  this->_API_HOST = HTTP_HOST;
  this->_API_PORT = HTTP_PORT;
  this->_API_USER = HTTP_USER;
  this->_API_PASS = HTTP_PASS;

  this->debug("UDP >> Starting UDP Server on port 18266");
  UdpServer.begin(18266);
}

void OHoCo::http_publish(char* topic, char* payload) {
  String url = "http://" + String(this->_API_HOST) + "/rift3/api/" + String(topic);

//  this->debug(url);
//  this->debug(payload);

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("payload=" + String(payload));
  String response = http.getString();

  if (response != "OK") {
    this->debug(response);
  }
  
  http.end();
}

void OHoCo::mqtt_config(const char* MQTT_BROKER, const int MQTT_PORT, const char* MQTT_USER, const char* MQTT_PASS) {
  this->_use_mqtt = true;
  this->_API_HOST = MQTT_BROKER;
  this->_API_PORT = MQTT_PORT;
  this->_API_USER = MQTT_USER;
  this->_API_PASS = MQTT_PASS;
  
  MQTTClient.setServer(this->_API_HOST, this->_API_PORT);
  MQTTClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->mqtt_callback(topic, payload, length); });
}

void OHoCo::mqtt_connect() {
  this->debug("Connecting to MQTT Broker");
  
  MQTTClient.connect(this->_CLIENT_ID.c_str(), this->_API_USER, this->_API_PASS);
  
  if (MQTTClient.connected()) {
    this->debug("MQTT client connected");
    
    this->_MQTT_DEVICE_TOPIC = "ohoco/device/" + this->_CLIENT_ID;
    MQTTClient.subscribe(this->_MQTT_DEVICE_TOPIC.c_str());
    this->debug("MQTT topic subscribed " + this->_MQTT_DEVICE_TOPIC);
    
//    MQTTClient.subscribe("test");
//    MQTTClient.publish("test", String(this->_CLIENT_ID).c_str());
    
//    MQTTClient.subscribe("ntp/OUT");
//    Publish("debug", String(CLIENT_ID) + " [LOGGED IN]");
//    Publish("ntp/IN", JsonString(String(MQTT_SUBSCRIBE_TOPIC), ""));
    
  }
  else {
    this->debuginline("Failed to connect to MQTT Broker, rc=");
    this->debug(MQTTClient.state());
    this->debug("");
  }
  
	this->_is_mqtt_connected = MQTTClient.connected();
}

void OHoCo::mqtt_disconnect() {
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

    if (payload.substring(0, 3) == "REG") {
      this->debug("Register Message");
    }  
    else if (payload == "REBOOT") {
      this->reboot();
    }
    else if (payload == "WRITECFG") {
      this->config_write();
    }
    else {
      this->config_set(payload);
    }
  }
}

void OHoCo::mqtt_subscribe(char* topic) {
  if (MQTTClient.connected()) {
    MQTTClient.subscribe(topic);
    this->debug("MQTT topic subscribed " + String(topic));
  }
  else {
    this->debug("mqtt_subscribe :: mqtt not connected");
  }
}

void OHoCo::mqtt_publish(char* topic, char* payload) {
  if (MQTTClient.connected()) {
    MQTTClient.publish(topic, payload, true);
    this->debug("MQTT publish [" + String(topic) + "] -> " + String(payload));
  }
  else {
    this->debug("mqtt_publish :: mqtt not connected");
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
  if (this->_use_mqtt) {
    if (((millis() - this->_connection_timer) > 1000UL) && (WiFi.status() == WL_CONNECTED) && (!MQTTClient.connected())) {
      this->mqtt_connect();
    }
  
    if (MQTTClient.connected() == true)
      MQTTClient.loop();   
  }
  else {
    // if there's data available, read a packet
    int packetSize = UdpServer.parsePacket();
    if (packetSize) {
      char packetBuffer[255]; //buffer to hold incoming packet
      
      if (_debugmode == SERIAL) {
        IPAddress remoteIp = UdpServer.remoteIP();
        char strRemote[128];
        sprintf(strRemote, "%d.%d.%d.%d : %d", remoteIp[0],remoteIp[1],remoteIp[2],remoteIp[3],UdpServer.remotePort());
        this->debug("UDP >> MESSAGE RECEIVED FROM " + String(strRemote));
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
  this->debug("API >> Send alive ping");

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
  payload = "signal:" + String(rssi_percent) +" %|vcc:" + String(voltage/1024.00f) + " V";

  char topic[128] = "ohoco/ping/";
  strcat(topic, this->_CLIENT_ID.c_str());

  char pload[strlen(payload.c_str())+1];
  payload.toCharArray(pload, strlen(payload.c_str())+1);
  
  if (this->_use_mqtt)
    this->mqtt_publish(topic, pload);
  else
    this->http_publish(topic, pload);
}

void OHoCo::send_log(char* msg) {
  this->debug("API >> Send log: " + String(msg));

  char topic[128] = "ohoco/log/";
  strcat(topic, this->_CLIENT_ID.c_str());

  if (this->_use_mqtt)
    this->mqtt_publish(topic, msg);
  else
    this->http_publish(topic, msg);
}

void OHoCo::register_device(const char* sketch_version) {
  this->debug("API >> Register device");

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
  payload = "REG:" + String(this->_WIFI_NAME) + "|ip:" + String(strIP) + "|ssid:" + String(this->_WIFI_SSID) + "|signal:" + String(rssi_percent) +" %|vcc:" + String(voltage/1024.00f) + " V|ohoco-version:" + String(OHOCO_VERSION) + "|sketch-version:" + String(sketch_version);
  
  char topic[128] = "ohoco/device/";
  strcat(topic, this->_CLIENT_ID.c_str());
  
  char pload[strlen(payload.c_str())+1];
  payload.toCharArray(pload, strlen(payload.c_str())+1);
  
//  this->debug(topic);
//  this->debug(pload);

  if (this->_use_mqtt)
    this->mqtt_publish(topic, pload);
  else
    this->http_publish(topic, pload);

  delay(100);
}

void OHoCo::register_sensor(char* sensor_name, char* sensor_type) {
  this->debug("API >> Register sensor");

  char topic[128] = "ohoco/sensor/";
  strcat(topic, sensor_name);
  
//  this->debug(topic);
//  this->debug(sensor_type);

  if (this->_use_mqtt)
    this->mqtt_publish(topic, sensor_type);
  else
    this->http_publish(topic, sensor_type);

  delay(100);
}

void OHoCo::register_switch(char* switch_name, char* switch_type) {
  this->debug("API >> Register switch");

  char topic[128] = "ohoco/switch/";
  strcat(topic, switch_name);
  
  char pload[128] = {0};
  strcat(pload, this->_CLIENT_ID.c_str());
  strcat(pload, ";");
  strcat(pload, switch_type);
  
//   this->debug(topic);
//   this->debug(pload);

  if (this->_use_mqtt)
    this->mqtt_publish(topic, pload);
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

  this->debug("API >> Updating sensor " + String(sensor_name) +  " -> " + String(pload));
  
//  this->debug(topic);
//  this->debug(pload);
  
  if (this->_use_mqtt)
    this->mqtt_publish(topic, pload);
  else
    this->http_publish(topic, pload);
}

void OHoCo::trigger_activate(char* trigger_name) {
  this->debug("API >> Trigger " + String(trigger_name) + " activated");
  
  char topic[128] = "ohoco/trigger/";
  strcat(topic, trigger_name);

  if (this->_use_mqtt)
    this->mqtt_publish(topic, "fire");
  else
    this->http_publish(topic, "fire");
}

void OHoCo::config_read() {
  this->debug("CFG >> Reading configuration from EEEPROM");
  
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(4095);
  EEPROM.get(0, this->_ohoco_cfg);
  EEPROM.end();

  if (this->_ohoco_cfg.valid != 1) {
    // Configuration not set or not valid -> load defaults
    this->debug("CFG >> Not valid, using default-configuration");
    this->_ohoco_cfg.checkInterval = 5000;
    this->_ohoco_cfg.minValue = 1;
    this->_ohoco_cfg.maxValue = 32768;
    strcpy(this->_ohoco_cfg.displayName, this->_CLIENT_ID.c_str());
    strcpy(this->_ohoco_cfg.dataTopic, "");
    strcpy(this->_ohoco_cfg.inTrigger, "");
    strcpy(this->_ohoco_cfg.outTrigger, "");
  }
}

void OHoCo::config_write() {
  this->debug("CFG >> Writing configuration to EEEPROM");
  
  // Mark record in EEPROM as valid data. EEPROM erased -> Valid=0 / old config between 1 and current value
  this->_ohoco_cfg.valid = 1;
  
  // Save configuration from RAM into EEPROM
  EEPROM.begin(4095);
  EEPROM.put(0, this->_ohoco_cfg);
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure

  this->send_log("WRITECFG OK");
}

void OHoCo::config_reset() {
  this->debug("CFG >> Reset configuration in EEEPROM");
  
  // Reset EEPROM bytes to '0' for the length of the data structure
  EEPROM.begin(4095);
  for (int i = 0 ; i < sizeof(this->_ohoco_cfg) ; i++) {
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

  this->debug("CFG >> Set configuration [" + key + "] -> [" + val + "]");

  if (key == "checkInterval") {
    this->_ohoco_cfg.checkInterval = val.toInt();
    if (this->_ohoco_cfg.checkInterval < 100)
      this->_ohoco_cfg.checkInterval = 100;
  }
  else if (key == "minValue") {
    this->_ohoco_cfg.minValue = val.toInt();
    if (this->_ohoco_cfg.minValue < 1)
      this->_ohoco_cfg.minValue = 1;
  }
  else if (key == "maxValue") {
    this->_ohoco_cfg.maxValue = val.toInt();
    if (this->_ohoco_cfg.maxValue > 32768)
      this->_ohoco_cfg.maxValue = 32768;
  }
  else if (key == "displayName")
    strcpy(this->_ohoco_cfg.displayName, val.c_str());
  else if (key == "dataTopic")
    strcpy(this->_ohoco_cfg.dataTopic, val.c_str());
  else if (key == "inTrigger")
    strcpy(this->_ohoco_cfg.inTrigger, val.c_str());
  else if (key == "outTrigger")
    strcpy(this->_ohoco_cfg.outTrigger, val.c_str());
  else
    this->debug("CFG >> Unknown configuration key [" + key + "]");

//  this->config_display();
}

void OHoCo::config_send() {
  this->debug("API >> Send config to controller");

  String payload;
  payload = "checkInterval:" + String(this->_ohoco_cfg.checkInterval) + "|minValue:" + String(this->_ohoco_cfg.minValue) + "|maxValue:" + String(this->_ohoco_cfg.maxValue) + "|inTrigger:" + String(this->_ohoco_cfg.inTrigger) + "|outTrigger:" + String(this->_ohoco_cfg.outTrigger);

  char topic[128] = "ohoco/config/";
  strcat(topic, this->_CLIENT_ID.c_str());
  
  char pload[strlen(payload.c_str())+1];
  payload.toCharArray(pload, strlen(payload.c_str())+1);
  
//  this->debug(topic);
//  this->debug(pload);

  if (this->_use_mqtt)
    this->mqtt_publish(topic, pload);
  else
    this->http_publish(topic, pload);
}

void OHoCo::config_display() {
  this->debug("CFG >> Current configuration\n");
  this->debug("displayName  : " + String(this->_ohoco_cfg.displayName));
  this->debug("checkInterval: " + String(this->_ohoco_cfg.checkInterval));
  this->debug("minValue     : " + String(this->_ohoco_cfg.minValue));
  this->debug("maxValue     : " + String(this->_ohoco_cfg.maxValue));
  this->debug("dataTopic    : " + String(this->_ohoco_cfg.dataTopic));
  this->debug("inTrigger    : " + String(this->_ohoco_cfg.inTrigger));
  this->debug("outTrigger   : " + String(this->_ohoco_cfg.outTrigger));
  this->debug("");
}

void OHoCo::config_command(String cmd) {
//  this->debug("CFG >> DEVICE COMMAND (" + cmd + ")");

  if (cmd.substring(0, 3) == "REG") {
    this->debug("CFG >> Was a register message");
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
      this->debug("CFG >> UNKNOWN COMMAND (" + cmd + ") and no onMessage-Callback-Function set");
  }
}

void OHoCo::on_message(void (*CallbackFunc)(String)) {
  this->_CallbackFunction = CallbackFunc;
}

void OHoCo::reboot() {
  this->debug("SYS >> Rebooting...");

  this->send_log("REBOOT OK");
  
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while(1)
    wdt_reset();
}

