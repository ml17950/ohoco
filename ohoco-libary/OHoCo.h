#ifndef OHoCo_h
#define OHoCo_h

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
  int valid;                        // 0=no configuration, 1=valid configuration
  int minValue;
  int maxValue;
  int checkInterval;
  char displayName[31];
  char dataTopic[51];
  char inTrigger[51];
  char outTrigger[51];
} configData_t;

class OHoCo {
	public:
    configData_t _ohoco_cfg;
    
		OHoCo();
		void initialize();
    void setup_ready();

    void debugmode(int dbgmode);
    void debug(String msg);
    void debug(int msg);
    void debug(long msg);
    void debug(float msg);
    void debuginline(String msg);

    void led_on();
    void led_off();
    void led_flash(int cnt, int delayms);
    
    void wifi_config(const char* WIFI_NAME, const char* WIFI_SSID, const char* WIFI_PASS);
    void wifi_config(const char* WIFI_NAME, const char* WIFI_SSID, const char* WIFI_PASS, const char* WIFI_IP, const char* WIFI_DNS, const char* WIFI_GATEWAY);
    void wifi_connect();
    void wifi_disconnect();
    bool wifi_connected();

    void http_config(const char* HTTP_HOST, const int HTTP_PORT, const char* HTTP_USER, const char* HTTP_PASS);
    void http_publish(char* topic, char* payload);
    
    void mqtt_config(const char* MQTT_BROKER, const int MQTT_PORT, const char* MQTT_USER, const char* MQTT_PASS);
    void mqtt_connect();
    void mqtt_disconnect();
    bool mqtt_connected();
    void mqtt_callback(char* rtopic, byte* rpayload, unsigned int length);
    void mqtt_subscribe(char* topic);
    void mqtt_publish(char* topic, char* payload);

    void keepalive();
    void send_alive_ping();
    void send_log(char* msg);
    
    void register_device(const char* sketch_version);
    void register_sensor(char* sensor_name, char* sensor_type);
    void register_switch(char* switch_name, char* switch_type);
    
    void set_sensor_value(char* sensor_name, char* sensor_value, char* sensor_unit);
    
    void trigger_activate(char* trigger_name);

    void config_read();
    void config_write();
    void config_reset();
    void config_set(String cmd);
    void config_send();
    void config_display();
    void config_command(String cmd);

    void on_message(void (*CallbackFunc)(String));

    void reboot();

	private:
		int _debugmode;
    unsigned long _connection_timer = 0;
    unsigned long _alive_ping_timer = 0;
    String _CLIENT_ID;
    bool _WIFI_DHCP_MODE;
    const char* _WIFI_NAME; 
    const char* _WIFI_SSID;
    const char* _WIFI_PASS;
    IPAddress _WIFI_IP;
    IPAddress _WIFI_DNS;
    IPAddress _WIFI_GATEWAY;
    IPAddress _WIFI_SUBNET;
    const char* _API_HOST;
    int   _API_PORT;
    const char* _API_USER;
    const char* _API_PASS;
    String _MQTT_DEVICE_TOPIC;
    bool _use_mqtt;
		bool _is_wifi_connected;
		bool _is_mqtt_connected;
    void (*_CallbackFunction)(String);
    
    String macToStr(const uint8_t* mac);
};

#endif
