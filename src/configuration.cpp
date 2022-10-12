#include "configuration.h"

static const char *ConfigFile = "/config.json";

void readWiFiConfig(const JsonObject &doc, WiFiConfig &config) {
    config.SSID = doc["SSID"].as<String>();
    log("WiFi.SSID=%s", config.SSID.c_str());

    config.Password = doc["Password"].as<String>();
    log("WiFi.SSID=%s", config.Password.c_str());
}

void readMqttConfig(const JsonObject &doc, MqttConfig &config) {
    config.Host = doc["Host"].as<String>();
    log("Mqtt.Host=%s", config.Host.c_str());

    config.Port = doc["Port"].isNull() ? 1883 : doc["Port"].as<int>();
    log("Mqtt.Port=%d", config.Port);
}

void readOtaConfig(const JsonObject &doc, OtaConfig &config) {
    config.Url = doc["Url"].as<String>();
    log("Ota.Url=%s", config.Url.c_str());

    config.CertFingerprint = doc["CertFingerprint"].as<String>();
    log("Ota.CertFingerprint=%s", config.CertFingerprint.c_str());
}

void readConfig(const JsonObject &doc, Config &config) {
    readWiFiConfig(doc["WiFi"].as<JsonObject>(), config.WiFi);
    readMqttConfig(doc["Mqtt"].as<JsonObject>(), config.Mqtt);
    readOtaConfig(doc["Ota"].as<JsonObject>(), config.Ota);
}

bool loadConfiguration(Config &config) {
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

void writeWiFiConfig(const WiFiConfig &config, JsonObject &output) {
    output["SSID"] = config.SSID;
    output["Password"] = config.Password;
}

void writeMqttConfig(const MqttConfig &config, JsonObject &output) {
    output["Host"] = config.Host;
    output["Port"] = config.Port;
}

void writeOtaConfig(const OtaConfig &config, JsonObject &output) {
    output["Url"] = config.Url;
    output["CertFingerprint"] = config.CertFingerprint;
}

void writeConfiguration(const Config &config, JsonDocument &output) {
    auto wifi = output.createNestedObject("WiFi");
    writeWiFiConfig(config.WiFi, wifi);
    auto mqtt = output.createNestedObject("Mqtt");
    writeMqttConfig(config.Mqtt, mqtt);
    auto ota = output.createNestedObject("Ota");
    writeOtaConfig(config.Ota, ota);
}

bool saveConfiguration(const Config &config) {
    log("Saving config to '%s'", ConfigFile);
    auto file = LittleFS.open(ConfigFile, FILE_WRITE);
    if (!file) {
        log("Could not open config file for writing");
        return false;
    }
    log("File opened");

    StaticJsonDocument<512> doc;

    writeConfiguration(config, doc);
    log("Config written");

    if (serializeJson(doc, file) == 0) {
        log("Failed to serialize config");
        return false;
    }
    log("Config serialized");

    file.close();
    log("File closed");

    return true;
}