
#include "Mqtt.h"

MqttClient::MqttClient(const char *clientId, const char *hostname, unsigned short port)
    : _clientId(clientId), _hostname(hostname), _port(port), _wifiClient(),  _client(_wifiClient) {
    log("Mqtt client name:%s\nHostname=%s\nPort=%d", _clientId, _hostname, _port);
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

bool MqttClient::publish(const char *topic, const char *payload) {
    if (_client.connected()) {
        log("Sending to [%s]: '%s'", topic, payload);
        return _client.publish(topic, payload, false);
    }

    log("Mqtt not connected");
    return false;
}

bool MqttClient::publish(const char *type, float value) {
    if (!isnan(value)) {
        char payload[10];
        sprintf(payload, "%.1f", value);
        return publish(type, payload);
    }

    log("Value is NaN");
    return false;
}

void MqttClient::callback(char *topic, byte *payload, unsigned int length) {
}