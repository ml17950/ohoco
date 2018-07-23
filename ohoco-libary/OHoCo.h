#ifndef OHoCo_h
#define OHoCo_h

#define MQTT_VERSION MQTT_VERSION_3_1_1

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
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
    void debug(String msg);
    void debug(int msg);
    void debug(long msg);
    void debug(float msg);
    void debuginline(String msg);
    void debuginline(int msg);

    void led_on();
    void led_off();
    void led_flash(int cnt, int delayms);
    
//    void wifi_config(const char* WIFI_NAME, const char* WIFI_SSID, const char* WIFI_PASS);
//    void wifi_config(const char* WIFI_NAME, const char* WIFI_SSID, const char* WIFI_PASS, const char* WIFI_IP, const char* WIFI_DNS, const char* WIFI_GATEWAY);
    void wifi_connect();
    void wifi_disconnect();
    bool wifi_connected();

    void http_setup();
    void http_publish(char* topic, char* payload);

    void mqtt_setup();
    void mqtt_connect();
    void mqtt_disconnect();
    bool mqtt_connected();
    void mqtt_callback(char* rtopic, byte* rpayload, unsigned int length);
    void mqtt_subscribe(char* topic);
    void mqtt_publish(char* topic, char* payload, boolean retained);

    void keepalive();
    void send_alive_ping();
    void send_log(char* msg);
    
    void register_device(const char* sketch_version);
    void register_sensor(char* sensor_name, char* sensor_type);
    void register_switch(char* switch_name, char* switch_type);
    
    void set_sensor_value(char* sensor_name, char* sensor_value, char* sensor_unit);
    
    void trigger_activate(char* trigger_name);

    bool config_read();
    void config_write();
    void config_reset();
    void config_set(String cmd);
    void config_send();
    void config_display();
    void config_command(String cmd);

    void on_message(void (*CallbackFunc)(String));

    void reboot();

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
    String        _MQTT_DEVICE_TOPIC;
    bool          _USE_MQTT;
    bool          _is_wifi_connected;
    bool          _is_mqtt_connected;
    void          (*_CallbackFunction)(String);
    const char*   _SKETCH_VERSION;
    
    String macToStr(const uint8_t* mac);
};

#endif
