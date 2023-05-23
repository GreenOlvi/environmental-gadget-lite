#ifndef SensorData_h
#define SendorData_h

#include <ArduinoJson.h>
#include "common.h"

struct SensorData
{
    float temperatureIn;
    float humidityIn;

    // Telemetrics
    String deviceName;
    String version;
    IPAddress ip;
    String mac;
    int connectionTime;
    int8_t rssi;
};

void serializeData(SensorData &data, String &output) {
    StaticJsonDocument<256> doc;

    doc["TempIn"] = data.temperatureIn;
    doc["HumIn"] = data.humidityIn;

    doc["DeviceName"] = data.deviceName;
    doc["Version"] = data.version;
    doc["Ip"] = data.ip.toString();
    doc["Mac"] = data.mac;
    doc["ConnectTime"] = data.connectionTime;
    doc["RSSI"] = data.rssi;
#if USE_REAL_TIME
    doc["Timestamp"] = (unsigned long)time(nullptr);
#else
    doc["Timestamp"] = nullptr;
#endif

    serializeJson(doc, output);
}

#endif