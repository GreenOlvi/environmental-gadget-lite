#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "log.h"

#define FILE_READ "r"
#define FILE_WRITE "w"

struct WiFiConfig {
    String SSID;
    String Password;
};

struct MqttConfig {
    String Host;
    int Port;
};

struct OtaConfig {
    String Url;
    String CertFingerprint;
};

struct Config {
    WiFiConfig WiFi;
    MqttConfig Mqtt;
    OtaConfig Ota;
};

bool loadConfiguration(Config &config);
void writeConfiguration(const Config &config, JsonDocument &output);
bool saveConfiguration(const Config &config);

#endif