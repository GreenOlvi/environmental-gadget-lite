#include "configuration.h"

static const char* ConfigFile = "/config.json";

void readWiFiConfig(const JsonObject& doc, WiFiConfig& config) {
    config.SSID = doc["SSID"].as<String>();
    log("WiFi.SSID=%s", config.SSID.c_str());

    config.Password = doc["Password"].as<String>();
    log("WiFi.SSID=%s", config.Password.c_str());
}

void readMqttConfig(const JsonObject& doc, MqttConfig& config) {
    config.Host = doc["Host"].as<String>();
    log("Mqtt.Host=%s", config.Host.c_str());

    config.Port = doc["Port"].isNull() ? 1883 : doc["Port"].as<int>();
    log("Mqtt.Port=%d", config.Port);
}

void readOtaConfig(const JsonObject& doc, OtaConfig& config) {
    config.Url = doc["Url"].as<String>();
    log("Ota.Url=%s", config.Url.c_str());
}

void readConfig(const JsonObject& doc, Config& config) {
    readWiFiConfig(doc["WiFi"].as<JsonObject>(), config.WiFi);
    readMqttConfig(doc["Mqtt"].as<JsonObject>(), config.Mqtt);
    readOtaConfig(doc["Ota"].as<JsonObject>(), config.Ota);

    config.NtpServer = doc["NtpServer"].as<String>();
    log("NtpServer=%s", config.NtpServer.c_str());
}

bool loadConfiguration(Config& config) {
    log("Loading config from '%s'", ConfigFile);
    auto file = LittleFS.open(ConfigFile, FILE_READ);
    if (!file) {
        log("Could not open config file");
        return false;
    }
    log("File opened");

    StaticJsonDocument<512> doc;

    auto deserializationError = deserializeJson(doc, file);
    if (deserializationError) {
        log("Failed to read file");
        return false;

    }
    log("File deserialized");

    readConfig(doc.as<JsonObject>(), config);
    log("Config read");

    file.close();
    log("File closed");

    return true;
}