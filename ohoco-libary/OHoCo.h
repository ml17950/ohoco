#ifndef OHoCo_h
#define OHoCo_h

// Please define MQTT_MAX_PACKET_SIZE to 256 in PubSubClient.h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

#define NONE    0
#define LED     1
#define SERIAL  2

// Types 'byte' und 'word' doesn't work!
typedef struct {
  int valid;                        // 0 = no configuration, 34 (0x22) = valid configuration
  char displayName[31];
  char wifi_ssid[31];
  char wifi_pass[31];
  char controller_ip[16];
  int  controller_port;
  char controller_user[16];
  char controller_pass[16];
  int useMQTT;
  int checkInterval;
  int minValue;
  int maxValue;
  char dataTopic[51];
  char inTrigger[51];
  char outTrigger[51];
  char genericValue01[31];
  char genericValue02[31];
  char genericValue03[31];
  char genericValue04[31];
  char genericValue05[31];
  char genericValue06[31];
  char genericValue07[31];
  char genericValue08[31];
  char genericValue09[31];
  char genericValue10[31];
} configData_t;

class OHoCo {
  public:
    configData_t config;
    
    OHoCo();
    void initialize();
    void setup_ready();

    void debugmode(int dbgmode);
    void debugmode(int dbgmode, int ledpin);
    void println(String msg);
    void println(int msg);
    void println(long msg);
    void println(float msg);
    void print(String msg);
    void print(int msg);

    void led_on();
    void led_off();
    void led_flash(int cnt, int delayms);
    
    void wifi_connect();
    void wifi_disconnect();
    bool wifi_connected();

    void http_setup();
    void http_publish(char* topic, char* payload);

    void mqtt_setup();
    void mqtt_setup(const char* willTopic, const char* willMessage, uint8_t willQos, boolean willRetain);
    void mqtt_connect();
    void mqtt_disconnect();
    bool mqtt_connected();
    void mqtt_callback(char* rtopic, byte* rpayload, unsigned int length);
    void mqtt_subscribe(char* topic);
    void mqtt_publish(char* topic, char* payload, boolean retained);

    void keepalive();
    void send_alive_ping();
    void send_connect_msg();
    void send_log(char* msg);
    
    void register_device(const char* sketch_version);
    void register_device(const char* sketch_version, int timeout);
    void register_sensor(char* sensor_name, char* sensor_type, char* sensor_unit);
    void register_switch(char* switch_name, char* switch_type);
    void register_notify(char* notify_name);
    
    void sensor_update(char* sensor_name, char* sensor_value);
    void sensor_update(char* sensor_name, int sensor_value);
    void sensor_update(char* sensor_name, float sensor_value, int precision);
    
    void trigger_activate(char* trigger_name);
    void notify(char* notify_name, char* message);

    bool config_read();
    void config_write();
    void config_reset();
    void config_set(String cmd);
    void config_send();
    void config_display();
    void config_command(String topic, String command);

    void on_message(void (*CallbackFunc)(String, String));

    void reboot();
    void deepSleep_seconds(int seconds);
    void deepSleep_minutes(int minutes);

  private:
    int           DEBUGMODE;
    int           LED_PIN;
    unsigned long _connection_timer = 0;
    unsigned long _alive_ping_timer = 0;
    String        _CLIENT_ID;
    bool          _WIFI_DHCP_MODE;
    IPAddress     _WIFI_IP;
    IPAddress     _WIFI_DNS;
    IPAddress     _WIFI_GATEWAY;
    IPAddress     _WIFI_SUBNET;
    const char*   _API_HOST;
    int           _API_PORT;
    const char*   _API_USER;
    const char*   _API_PASS;
    const char*   _API_PATH;    const char*   _MQTT_WILL_TOPIC;
    const char*   _MQTT_WILL_PAYLOAD;
    uint8_t       _MQTT_WILL_QOS;
    bool          _MQTT_WILL_RETAIN;    String        _MQTT_DEVICE_TOPIC;
    bool          _USE_MQTT;
    int           _ALIVE_PING_INTERVAL;
    bool          _is_wifi_connected;
    bool          _is_mqtt_connected;
    void          (*_CallbackFunction)(String, String);
    const char*   _SKETCH_VERSION;
    
    String        macToStr(const uint8_t* mac);
    void          ota_update();
};

#endif
