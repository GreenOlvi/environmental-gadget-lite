#ifndef Mqtt_h
#define Mqtt_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "log.h"
#include "Module.h"

#define RECONNECT_DELAY 10000

class MqttClient : public Module {
    public:
        MqttClient(const char* clientId, const char* hostname, unsigned short port = 1883);
        bool isConnected(void);
        void setup(void) override;
        void update(const unsigned long) override;
        bool connect(void);
        void disconnect(void);
        int state(void);
        bool publish(const char* topic, const char* payload);
        bool publish(const char* type, float value);
    private:
        const char* _clientId;
        const char* _hostname;
        const unsigned short _port;
        WiFiClient _wifiClient;
        PubSubClient _client;
        unsigned long _lastUpdate = 0;
        void callback(char* topic, byte* payload, unsigned int length);
};

#endif