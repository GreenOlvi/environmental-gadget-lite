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
    _client.loop();
}

bool MqttClient::publish(const char *topic, const char *payload) {
    if (_client.connected()) {
        log("Sending to [%s]: '%s'\n", topic, payload);
        _client.beginPublish(topic, strlen(payload), false);
        _client.print(payload);
        return _client.endPublish();
    }

    log("Mqtt not connected");
    return false;
}

bool MqttClient::subscribe(const char *topic) {
    return _client.subscribe(topic);
}

void MqttClient::setCallback(MQTT_CALLBACK_SIGNATURE) {
    _client.setCallback(callback);
}