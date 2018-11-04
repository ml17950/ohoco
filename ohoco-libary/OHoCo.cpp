#include "OHoCo.h"

const char*  OHOCO_VERSION     = "18.10.06";

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
  this->_MQTT_WILL_QOS = -1;
  this->_ALIVE_PING_INTERVAL = 3600000;
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
  this->println("SYS  >> CLIENT_ID is " + clientName);
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

void OHoCo::println(String msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.println(msg);
}

void OHoCo::println(int msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.println(msg);
}

void OHoCo::println(long msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.println(msg);
}

void OHoCo::println(float msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.println(msg);
}

void OHoCo::print(String msg) {
  if (this->DEBUGMODE == SERIAL)
    Serial.print(msg);
}

void OHoCo::print(int msg) {
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
  
  this->print("SYS  >> Initialize WiFi - trying to connect to " + String(this->config.wifi_ssid) + " ");
  WiFi.begin(this->config.wifi_ssid, this->config.wifi_pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    this->print(".");
    connect_timeout_sec--;
    if (connect_timeout_sec <= 0) {
      this->println(" WiFi connection failed");
      this->_is_wifi_connected = false;
      return;
    }
  }
  this->println(" WiFi connected");

//  if (this->_WIFI_DHCP_MODE == false) {
//    WiFi.config(this->_WIFI_IP, this->_WIFI_GATEWAY, this->_WIFI_SUBNET);
//    //WiFi.setDNS(this->_WIFI_DNS);
//  }
  
  IPAddress ip = WiFi.localIP();
  char strIP[24];
  sprintf(strIP, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
  this->println("IP      " + String(strIP));

//  if (this->_WIFI_DHCP_MODE == false) {
//    sprintf(strIP, "%d.%d.%d.%d", this->_WIFI_SUBNET[0],this->_WIFI_SUBNET[1],this->_WIFI_SUBNET[2],this->_WIFI_SUBNET[3]);
//    this->println("SUBNET " + String(strIP));
//    sprintf(strIP, "%d.%d.%d.%d", this->_WIFI_GATEWAY[0],this->_WIFI_GATEWAY[1],this->_WIFI_GATEWAY[2],this->_WIFI_GATEWAY[3]);
//    this->println("GATWAY " + String(strIP));
//    //sprintf(strIP, "%d.%d.%d.%d", this->_WIFI_DNS[0],this->_WIFI_DNS[1],this->_WIFI_DNS[2],this->_WIFI_DNS[3]);
//    //this->println("DNS     " + String(strIP));
//  }

  this->_is_wifi_connected = true;
}

void OHoCo::wifi_disconnect() {
  this->println("Disable WiFi");
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
  this->_API_PATH = "ohoco";

  this->println("UDP  >> Starting UDP Server on port 18266");
  UdpServer.begin(18266);
}

void OHoCo::http_publish(char* topic, char* payload) {
  //String url = "http://" + String(this->_API_HOST) + "/rift3/api/" + String(topic);
  String url = "http://" + String(this->_API_HOST) + "/" + String(this->_API_PATH) + "/rpc/" + String(topic);
  
  String strPayload = String(payload);
  strPayload.replace("+", "%2B");
  
  //this->println(url);
  //this->println(payload);
  //this->println(strPayload);

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("payload=" + strPayload);
  String response = http.getString();

  if (response != "ACK") {
    this->println(url);
    this->println(strPayload);
    
    this->print("RESP >> ");
    this->println(response);
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
    this->println("mqtt_setup: invalid ip adress");
}

void OHoCo::mqtt_setup(const char* willTopic, const char* willMessage, uint8_t willQos, boolean willRetain) {
  this->_USE_MQTT = true;
  this->_API_HOST = this->config.controller_ip;
  this->_API_PORT = this->config.controller_port;
  this->_API_USER = this->config.controller_user;
  this->_API_PASS = this->config.controller_pass;

  this->_MQTT_WILL_TOPIC    = willTopic;
  this->_MQTT_WILL_PAYLOAD  = willMessage;
  this->_MQTT_WILL_QOS      = willQos;
  this->_MQTT_WILL_RETAIN   = willRetain;

  IPAddress addr;
  if (addr.fromString(this->_API_HOST)) {
    // it was a valid address, do something with it 
    MQTTClient.setServer(addr, this->_API_PORT);
    MQTTClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->mqtt_callback(topic, payload, length); });
  }
  else
    this->println("mqtt_setup: invalid ip adress");
}

void OHoCo::mqtt_connect() {
  this->print("SYS  >> Connecting to MQTT Broker ");
  this->print(this->_API_HOST);
  this->print(":");
  this->print(this->_API_PORT);
  this->print(" ");

  int connect_timeout_sec = 10;
  
  while (!MQTTClient.connected()) {
    //MQTTClient.connect(this->_CLIENT_ID.c_str(), this->_API_USER, this->_API_PASS); //, "/debug/lwt", 0, 0, "off");
    if (this->_MQTT_WILL_QOS > -1) {
      this->print(" with LWT ");
      MQTTClient.connect(this->_CLIENT_ID.c_str(), this->_MQTT_WILL_TOPIC, this->_MQTT_WILL_QOS, this->_MQTT_WILL_RETAIN, this->_MQTT_WILL_PAYLOAD);
    }
    else
      MQTTClient.connect(this->_CLIENT_ID.c_str());

    if (MQTTClient.connected())
      break;
    
    delay(1000);
    this->print(".");
    connect_timeout_sec--;
    if (connect_timeout_sec <= 0) {
      this->print(" connection failed, responsecode = ");
      this->println(MQTTClient.state());
      this->_is_mqtt_connected = false;
      return;
    }
  }
  this->println(" MQTT client connected");
  
  if (MQTTClient.connected()) {
    this->_MQTT_DEVICE_TOPIC = "ohoco/device/" + this->_CLIENT_ID + "/callback";
    MQTTClient.subscribe(this->_MQTT_DEVICE_TOPIC.c_str());
    this->println("MQTT >> topic subscribed " + this->_MQTT_DEVICE_TOPIC);

//    MQTTClient.subscribe("ntp/OUT");
//    Publish("debug", String(CLIENT_ID) + " [LOGGED IN]");
//    Publish("ntp/IN", JsonString(String(MQTT_SUBSCRIBE_TOPIC), ""));
  }

  this->_is_mqtt_connected = MQTTClient.connected();
}

void OHoCo::mqtt_disconnect() {
  this->println("MQTT >> Disconnectiong");
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

  this->println("MQTT << receive [" + topic + "] -> " + payload);

  if (topic == this->_MQTT_DEVICE_TOPIC) {
    this->println("DEVICE COMMAND");
    this->config_command(topic, payload);
  }
  else {
    if (this->_CallbackFunction != NULL)
      this->_CallbackFunction(topic, payload);
    else
      this->println("CFG  >> no onMessage-Callback-Function set");
  }
}

void OHoCo::mqtt_subscribe(char* topic) {
  if (MQTTClient.connected()) {
    if (MQTTClient.subscribe(topic))
      this->println("MQTT >> topic subscribed " + String(topic));
    else
      this->println("MQTT >> subscribe error :: return == false");
  }
  else {
    this->println("MQTT >> subscribe error :: mqtt not connected");
  }
}

void OHoCo::mqtt_publish(char* topic, char* payload, boolean retained) {
//  this->print("MQTT_MAX_PACKET_SIZE :: ");
//  this->println(MQTT_MAX_PACKET_SIZE);
  
  if (MQTTClient.connected()) {
    int lop = strlen(payload);
    if (lop < (MQTT_MAX_PACKET_SIZE - 36)) {
      //if (MQTTClient.publish(topic, payload, retained)) {
      if (MQTTClient.publish(topic, (uint8_t*)payload, lop, retained)) {
        if (retained)
          this->println("MQTT >> publish [" + String(topic) + "] -> " + String(payload) + " (RETAINED)");
        else
          this->println("MQTT >> publish [" + String(topic) + "] -> " + String(payload));
      }
      else {
        this->print("MQTT >> publish error :: return == false (");
        this->print(lop);
        this->println(") Bytes");
      }
    }
    else {
      this->print("MQTT >> publish error :: payload greater (MQTT_MAX_PACKET_SIZE - 36) Bytes (");
      this->print(lop);
      this->println(") Bytes");
    }
  }
  else {
    this->println("MQTT >> publish error :: mqtt not connected");
  }
}

void OHoCo::keepalive() {
//  this->println("OHoCo::keepalive()");

  if (((millis() - this->_connection_timer) > 12000UL) && (WiFi.status() != WL_CONNECTED)) {
    this->_connection_timer = millis();
    this->wifi_connect();
    this->send_connect_msg();
  }
  if (((millis() - this->_alive_ping_timer) > this->_ALIVE_PING_INTERVAL) && (WiFi.status() == WL_CONNECTED)) {
    this->_alive_ping_timer = millis();
    this->send_alive_ping();
  }
  if (this->_USE_MQTT) {
    if (((millis() - this->_connection_timer) > 1000UL) && (WiFi.status() == WL_CONNECTED) && (!MQTTClient.connected())) {
      this->mqtt_connect();
      this->send_connect_msg();
    }
  
    if (MQTTClient.connected() == true)
      MQTTClient.loop();
    else
      this->mqtt_connect();
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
        this->println("UDP  >> MESSAGE RECEIVED FROM " + String(strRemote));
      }

      // read the packet into packetBufffer
      int len = UdpServer.read(packetBuffer, 255);
      if (len > 0) {
        packetBuffer[len] = 0;
      }
//      this->println(packetBuffer);

      this->config_command("UDP", String(packetBuffer));
    }
  }
  
  delay(1);
}

void OHoCo::send_alive_ping() {
  this->println("API  >> Send alive ping");

  char topic[128] = {0};
  char pload[128] = {0};

  // ---------------------------------------------------------------------
  
  long rssi_dbm = WiFi.RSSI();
  int rssi_percent;

  if (rssi_dbm <= -100)
    rssi_percent = 0;
  else if (rssi_dbm >= -50)
    rssi_percent = 100;
  else
    rssi_percent = 2 * (rssi_dbm + 100);
  
  itoa(rssi_percent, pload, 10);
  strcat(pload, "%");

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/signal", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/signal/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }

  // ---------------------------------------------------------------------

  float voltage = 0.00f;
  voltage = ESP.getVcc();
  
  dtostrf((voltage/1024.00f), 1, 2, pload);
  strcat(pload, " V");

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/voltage", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/voltage/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }
  
  if (this->_USE_MQTT)
    this->mqtt_publish(topic, pload, false);
  else
    this->http_publish(topic, pload);

  // ---------------------------------------------------------------------

  long now = millis() / 1000;
  int days =  now / 86400UL;
  int hours = (now % 86400UL) / 3600UL;
  int minutes = (now / 60UL) % 60UL;
  int seconds = now % 60UL;
  
  if (days > 0)
    sprintf(pload, "%dd %02d:%02d:%02d", days, hours, minutes, seconds);
  else
    sprintf(pload, "%02d:%02d:%02d", hours, minutes, seconds);

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/uptime", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/uptime/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }
}

void OHoCo::send_connect_msg() {
  this->println("API  >> Send connect message");

  char topic[128] = {0};

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/connect/%s", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, "1", false);
  }
  else {
    sprintf(topic, "device/connect/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, "1");
  }
}

void OHoCo::send_log(char* msg) {
  this->println("API  >> Send log: " + String(msg));

  char topic[128] = {0};

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/log/%s", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, msg, false);
  }
  else {
    sprintf(topic, "log/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, msg);
  }
}

void OHoCo::register_device(const char* sketch_version) {
  this->register_device(sketch_version, 3600);
}

void OHoCo::register_device(const char* sketch_version, int timeout) {
  this->println("API  >> Register device " + String(this->config.displayName));

  char topic[128] = {0};
  char pload[128] = {0};
  
  sprintf(pload, "%s", this->config.displayName);
  
  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/name", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/name/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }

  // ---------------------------------------------------------------------

  IPAddress ip = WiFi.localIP();

  sprintf(pload, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
  
  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/ip", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/ip/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }

  // ---------------------------------------------------------------------

  this->_ALIVE_PING_INTERVAL = timeout * 1000;
  
  sprintf(pload, "%d", timeout);
  
  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/timeout", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/timeout/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }
  
  // ---------------------------------------------------------------------

  sprintf(pload, "%s", this->config.wifi_ssid);
  
  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/ssid", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/ssid/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }
  
  // ---------------------------------------------------------------------

  sprintf(pload, "%s", OHOCO_VERSION);
  
  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/ohoco", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/ohoco/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }
  
  // ---------------------------------------------------------------------

  if (strlen(sketch_version) > 1)
    this->_SKETCH_VERSION = sketch_version;

  sprintf(pload, "%s", this->_SKETCH_VERSION);
  
  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/sketch", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/sketch/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }
  
  // ---------------------------------------------------------------------

  this->send_connect_msg();
  this->send_alive_ping();
  
  // ---------------------------------------------------------------------

  delay(100);
}

void OHoCo::register_sensor(char* sensor_name, char* sensor_type, char* sensor_unit) {
  this->println("API  >> Register sensor " + String(sensor_name));

  char topic[128] = {0};

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/sensor/%s/type", sensor_name);
    this->mqtt_publish(topic, sensor_type, false);
  }
  else {
    sprintf(topic, "sensor/type/?id=%s", sensor_name);
    this->http_publish(topic, sensor_type);
  }

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/sensor/%s/unit", sensor_name);
    this->mqtt_publish(topic, sensor_unit, false);
  }
  else {
    sprintf(topic, "sensor/unit/?id=%s", sensor_name);
    this->http_publish(topic, sensor_unit);
  }
}

void OHoCo::register_switch(char* switch_name, char* switch_type) {
  this->println("API  >> Register switch " + String(switch_name));

  char topic[128] = {0};

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/sensor/%s/type", switch_name);
    this->mqtt_publish(topic, switch_type, false);
    
    sprintf(topic, "ohoco/switch/%s", switch_name);
    this->mqtt_publish(topic, "off", false);

    this->mqtt_subscribe(topic);
  }
  else {
    char pload[128] = {0};
    sprintf(pload, "%s:%s", switch_type, this->_CLIENT_ID.c_str());
    sprintf(topic, "switch/register/?id=%s", switch_name);
    this->http_publish(topic, pload);
  }
}

void OHoCo::register_notify(char* notify_name) {
  this->println("API  >> Register notify " + String(notify_name));

  char topic[128] = {0};
  char pload[128] = {0};
  
  strcat(pload, this->_CLIENT_ID.c_str());

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/notify/%s", notify_name);
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "notify/register/?id=%s", notify_name);
    this->http_publish(topic, pload);
  }
}

void OHoCo::sensor_update(char* sensor_name, char* sensor_value) {
  this->println("API  >> Updating sensor value " + String(sensor_name) +  " -> " + String(sensor_value));
  
  char topic[128] = {0};
  char pload[128] = {0};
  
  strcat(pload, sensor_value);

  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/sensor/%s/value", sensor_name);
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "sensor/value/?id=%s", sensor_name);
    this->http_publish(topic, pload);
  }
}

void OHoCo::sensor_update(char* sensor_name, int sensor_value) {
  char CString[10];
  itoa(sensor_value, CString, 10);
  this->sensor_update(sensor_name, CString);
}

void OHoCo::sensor_update(char* sensor_name, float sensor_value, int precision) {
  char CString[10];
  if (sensor_value < 10)
    dtostrf(sensor_value, 1, precision, CString);
  else if (sensor_value < 100)
    dtostrf(sensor_value, 2, precision, CString);
  else if (sensor_value < 1000)
    dtostrf(sensor_value, 3, precision, CString);
  else
    dtostrf(sensor_value, 4, precision, CString);

  // convert . to ,
  for (int i=0; i<strlen(CString); i++) {
    if (CString[i] == '.')
      CString[i] = ',';
  }
  
  this->sensor_update(sensor_name, CString);
}

void OHoCo::trigger_activate(char* trigger_name) {
  this->println("API  >> Activate trigger " + String(trigger_name));
  
  char topic[128] = {0};
  char pload[128] = {0};
  
  strcat(pload, "fire");
  
  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/trigger/%s", trigger_name);
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "trigger/activate/?id=%s", trigger_name);
    this->http_publish(topic, pload);
  }
}

void OHoCo::notify(char* notify_name, char* message) {
  this->println("API  >> Send notification " + String(notify_name));

  char topic[128] = {0};
  char pload[128] = {0};
  
  strcat(pload, message);
  
  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/notify/%s", notify_name);
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "notify/activate/?id=%s", notify_name);
    this->http_publish(topic, pload);
  }
}

bool OHoCo::config_read() {
  this->println("CFG  >> Reading configuration from EEEPROM");
  
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(4095);
  EEPROM.get(0, this->config);
  EEPROM.end();

  if (this->config.valid == 0) {
    this->config_reset();
    return false;
  }
  if (this->config.valid != 34) {
    // Configuration not set or not valid
    return false;
  }
  return true;
}

void OHoCo::config_write() {
  this->println("CFG  >> Writing configuration to EEEPROM");
  
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
  this->println("CFG  >> Reset configuration in EEEPROM");
  
  // Reset EEPROM bytes to '0' for the length of the data structure
  EEPROM.begin(4095);
  for (int i = 0 ; i < sizeof(this->config) ; i++) {
    if (i < 2)
      EEPROM.write(i, 1);
    else
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

  this->println("CFG  >> Set configuration [" + key + "] -> [" + val + "]");

  if (key == "chk") {
    this->config.checkInterval = val.toInt();
    if (this->config.checkInterval < 100)
      this->config.checkInterval = 100;
  }
  else if (key == "min") {
    this->config.minValue = val.toInt();
    if (this->config.minValue < 0)
      this->config.minValue = 0;
  }
  else if (key == "max") {
    this->config.maxValue = val.toInt();
  }
  else if (key == "din")
    strcpy(this->config.displayName, val.c_str());
  else if (key == "dataTopic")
    strcpy(this->config.dataTopic, val.c_str());
  else if (key == "itr")
    strcpy(this->config.inTrigger, val.c_str());
  else if (key == "otr")
    strcpy(this->config.outTrigger, val.c_str());
  else if (key == "wid")
    strcpy(this->config.wifi_ssid, val.c_str());
  else if (key == "wpw")
    strcpy(this->config.wifi_pass, val.c_str());
  else if (key == "cip")
    strcpy(this->config.controller_ip, val.c_str());
  else if (key == "cpo")
    this->config.controller_port = val.toInt();
  else if (key == "cus")
    strcpy(this->config.controller_user, val.c_str());
  else if (key == "cpw")
    strcpy(this->config.controller_pass, val.c_str());
  else if (key == "mqt") {
    this->config.useMQTT = val.toInt();
    if (this->config.useMQTT < 0)
      this->config.useMQTT = 0;
    if (this->config.useMQTT > 1)
      this->config.useMQTT = 1;
  }
  else
    this->println("CFG  >> Unknown configuration key [" + key + "]");

//  this->config_display();
}

void OHoCo::config_send() {
  this->println("API  >> Send config to controller");

  String payload;
  // fill payload with data as json array
  payload  = "{";
  payload += "\"din\":\"" + String(this->config.displayName) + "\",";
  payload += "\"wid\":\"" + String(this->config.wifi_ssid) + "\",";
  payload += "\"wpw\":\"" + String(this->config.wifi_pass) + "\",";
  payload += "\"cip\":\"" + String(this->config.controller_ip) + "\",";
  payload += "\"cpo\":\"" + String(this->config.controller_port) + "\",";
  payload += "\"cus\":\"" + String(this->config.controller_user) + "\",";
  payload += "\"cpw\":\"" + String(this->config.controller_pass) + "\",";
  payload += "\"mqt\":\"" + String(this->config.useMQTT) + "\",";
  payload += "\"chk\":\"" + String(this->config.checkInterval) + "\",";
  payload += "\"min\":\"" + String(this->config.minValue) + "\",";
  payload += "\"max\":\"" + String(this->config.maxValue) + "\",";
  payload += "\"itr\":\"" + String(this->config.inTrigger) + "\",";
  payload += "\"otr\":\"" + String(this->config.outTrigger) + "\"";
  payload += "}";
  
  char topic[128] = "ohoco/config/";
  strcat(topic, this->_CLIENT_ID.c_str());
  
  char pload[strlen(payload.c_str())+1];
  payload.toCharArray(pload, strlen(payload.c_str())+1);
  
  if (this->_USE_MQTT) {
    sprintf(topic, "ohoco/device/%s/config", this->_CLIENT_ID.c_str());
    this->mqtt_publish(topic, pload, false);
  }
  else {
    sprintf(topic, "device/config/?id=%s", this->_CLIENT_ID.c_str());
    this->http_publish(topic, pload);
  }
}

void OHoCo::config_display() {
  this->println("CFG  >> Current configuration\n");
  if (this->config.valid == 34) {
    this->print("config source  : EEPROM   // ");
    this->println(this->config.valid);
  }
  else {
    this->print("config source  : DEFAULTS // ");
    this->println(this->config.valid);
  }
  this->println("displayName    : " + String(this->config.displayName));
  this->println("wifi_ssid      : " + String(this->config.wifi_ssid));
  this->println("wifi_pass      : " + String(this->config.wifi_pass));
  this->println("controller_ip  : " + String(this->config.controller_ip));
  this->println("controller_port: " + String(this->config.controller_port));
  this->println("controller_user: " + String(this->config.controller_user));
  this->println("controller_pass: " + String(this->config.controller_pass));
  this->println("useMQTT        : " + String(this->config.useMQTT));
  this->println("checkInterval  : " + String(this->config.checkInterval));
  this->println("minValue       : " + String(this->config.minValue));
  this->println("maxValue       : " + String(this->config.maxValue));
  this->println("dataTopic      : " + String(this->config.dataTopic));
  this->println("inTrigger      : " + String(this->config.inTrigger));
  this->println("outTrigger     : " + String(this->config.outTrigger));
//  this->println("genericValue01 : " + String(this->config.genericValue01));
//  this->println("genericValue02 : " + String(this->config.genericValue02));
//  this->println("genericValue03 : " + String(this->config.genericValue03));
//  this->println("genericValue04 : " + String(this->config.genericValue04));
//  this->println("genericValue05 : " + String(this->config.genericValue05));
//  this->println("genericValue06 : " + String(this->config.genericValue06));
//  this->println("genericValue07 : " + String(this->config.genericValue07));
//  this->println("genericValue08 : " + String(this->config.genericValue08));
//  this->println("genericValue09 : " + String(this->config.genericValue09));
//  this->println("genericValue10 : " + String(this->config.genericValue10));
  this->println("");
}

void OHoCo::config_command(String topic, String command) {
  this->println("CFG  >> DEVICE COMMAND (" + command + ") on (" + topic + ")");

  if (command.substring(0, 3) == "REG") {
    this->println("CFG  >> Was a register message");
  }
  else if (command == "REREG") {
    this->register_device("");
  }
  else if (command == "REBOOT") {
    this->reboot();
  }
  else if (command == "SENDCFG") {
    this->config_send();
  }
  else if (command == "WRITECFG") {
    this->config_write();
  }
  else if (command == "RESETCFG") {
    this->config_reset();
  }
  else if (command == "PING") {
    this->send_alive_ping();
  }
  else if (command == "OTAUPD") {
    this->ota_update();
  }
  else if (command.substring(0, 3) == "CFG") {
    this->config_set(command);
  }
  else {
    if (this->_CallbackFunction != NULL)
      this->_CallbackFunction(topic, command);
    else
      this->println("CFG  >> UNKNOWN COMMAND (" + command + ") and no onMessage-Callback-Function set");
  }
}

void OHoCo::on_message(void (*CallbackFunc)(String, String)) {
  this->_CallbackFunction = CallbackFunc;
}

void OHoCo::reboot() {
  this->println("SYS  >> Rebooting...");

  this->send_log("REBOOT OK");
  
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while(1)
    wdt_reset();
}

void OHoCo::deepSleep_seconds(int seconds) {
  unsigned int microseconds = seconds * 1000000;
  this->print("SYS  >> Going into deepSleep for ");
  this->print(seconds);
  this->println(" seconds");
  ESP.deepSleep(microseconds);
}

void OHoCo::deepSleep_minutes(int minutes) {
  if (minutes > 70)
    minutes = 70;
  unsigned int microseconds = minutes * 60000000;
  this->print("SYS  >> Going into deepSleep for ");
  this->print(minutes);
  this->println(" minutes");
  ESP.deepSleep(microseconds);
}

void OHoCo::ota_update() {
  String ota_url = "http://" + String(this->_API_HOST) + "/" + String(this->_API_PATH) + "/rpc/device/ota/?id=" + this->_CLIENT_ID + "&currver=" + String(this->_SKETCH_VERSION);
  
  this->println("OTA  >> starting OTA update via " + ota_url);

  this->send_log("OTA Start");
  
  t_httpUpdate_return ret = ESPhttpUpdate.update(ota_url);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      this->print("OTA  >> HTTP_UPDATE_FAILD : ");
      this->print(ESPhttpUpdate.getLastError());
      this->print(" / ");
      this->println(ESPhttpUpdate.getLastErrorString());
      this->send_log("OTA Fail");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      this->println("OTA  >> HTTP_UPDATE_NO_UPDATES");
      this->send_log("OTA NoUpd");
      break;
    case HTTP_UPDATE_OK:
      this->println("OTA  >> HTTP_UPDATE_OK");
      this->send_log("OTA Ok");
      break;
    default:
      this->print("OTA  >> HTTP_UPDATE_UNKNOWN ");
      this->println(ret);
  }
}

