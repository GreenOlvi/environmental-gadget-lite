#ifndef SensorData_h
#define SendorData_h

#include <ArduinoJson.h>
#include "common.h"

struct SensorData
{
    String deviceName;
    String version;
    float temperatureIn;
    float humidityIn;
    int connectionTime;
};

void serializeData(SensorData &data, String &output) {
    StaticJsonDocument<128> doc;

    doc["DeviceName"] = data.deviceName;
    doc["Version"] = data.version;
    doc["TempIn"] = data.temperatureIn;
    doc["HumIn"] = data.humidityIn;
    doc["ConnectTime"] = data.connectionTime;

    serializeJson(doc, output);
}

#endif