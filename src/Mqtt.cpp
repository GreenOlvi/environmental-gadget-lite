#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "secrets.h"
#include "Mqtt.h"

MqttClient::MqttClient(const char* clientId, const char* hostname, unsigned short port)
    : _clientId(clientId), _hostname(hostname), _port(port), _wifiClient(),  _client(_wifiClient) {
    logf1("Mqtt client name:%s\n", clientId);
}

void MqttClient::setup() {
    _client.setServer(_hostname, _port);
}

bool MqttClient::isConnected() {
    return _client.connected();
}

bool MqttClient::connect() {
    return _client.connect(_clientId);
}

void MqttClient::disconnect() {
    _client.disconnect();
}

int MqttClient::state() {
    return _client.state();
}

void MqttClient::update(unsigned long t) {
    if (t - _lastUpdate >= RECONNECT_DELAY) {
        if (WiFi.isConnected() && !_client.connected()) {
            connect();
        }
        _lastUpdate = t;
    }
    _client.loop();
}

bool MqttClient::publish(const char* topic, const char* payload) {
    if (_client.connected()) {
        logf2("Sending to [%s]: '%s'\n", topic, payload);
        return _client.publish(topic, payload, false);
    }

    logln("Mqtt not connected");
    return false;
}

bool MqttClient::publish(const char* type, float value) {
    if (!isnan(value)) {
        char payload[10];
        sprintf(payload, "%.1f", value);
        return publish(type, payload);
    }

    logln("Value is NaN");
    return false;
}

void MqttClient::callback(char* topic, byte* payload, unsigned int length) {
}