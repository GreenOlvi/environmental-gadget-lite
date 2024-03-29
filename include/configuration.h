#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "log.h"

#define FILE_READ "r"

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
};

struct Config {
    WiFiConfig WiFi;
    MqttConfig Mqtt;
    OtaConfig Ota;
    String NtpServer;
};

bool loadConfiguration(Config& config);

#endif