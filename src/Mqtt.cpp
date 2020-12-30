#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "Mqtt.h"

MqttClient::MqttClient(const char* clientId, const char* hostname, unsigned short port)
    : _clientId(clientId), _hostname(hostname), _port(port), _wifiClient(),  _client(_wifiClient) {
}

void MqttClient::setup() {
    _client.setServer(_hostname, _port);
}

bool MqttClient::isConnected() {
    return _client.connected();
}

void MqttClient::connect() {
    _client.connect(_clientId);
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

void MqttClient::publish(const char* topic, const char* payload) {
    if (_client.connected()) {
        _client.publish(topic, payload);
    }
}

void MqttClient::publish(const char* type, float value) {
    if (_client.connected() && value != NAN) {
        char payload[10];
        sprintf(payload, "%.1f", value);
        publish(type, payload);
    }
}

void MqttClient::callback(char* topic, byte* payload, unsigned int length) {
}