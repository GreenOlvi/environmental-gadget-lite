#ifndef Mqtt_h
#define Mqtt_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "log.h"
#include "Module.h"

class MqttClient : public Module {
    public:
        MqttClient(const char *clientId, const char *hostname, unsigned short port = 1883);
        bool isConnected(void);
        void setup(void) override;
        void update(const unsigned long) override;
        bool connect(void);
        void disconnect(void);
        int state(void);
        bool publish(const char *topic, const char *payload);
        bool subscribe(const char *topic);
        void setCallback(MQTT_CALLBACK_SIGNATURE);
    private:
        const char *_clientId;
        const char *_hostname;
        const unsigned short _port;
        WiFiClient _wifiClient;
        PubSubClient _client;
};

#endif