#define VERSION_PREFIX "1.1.4"

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#define VERSION_SUFFIX "-debug"
#define OTA_ENABLED 1
#define MOCK_SENSOR 1
#else
#define VERSION_SUFFIX
#define OTA_ENABLED 1
#define MOCK_SENSOR 0
#endif

#ifndef USE_NTP
#define USE_NTP 0
#endif

#ifndef USE_RTC
#define USE_RTC 0
#endif

#if USE_NTP || USE_RTC
#define USE_REAL_TIME 1
#else
#define USE_REAL_TIME 0
#endif

#ifndef BUILD_TIME
#define BUILD_TIME 0
#endif

#if DEBUG
#define SLEEPTIME 20e6
#else
#define SLEEPTIME 60e6
#endif

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266httpUpdate.h>

#include "configuration.h"
#include "log.h"
#include "common.h"
#include "PersistentStorage.h"
#include "Mqtt.h"
#include "SensorsModule.h"
#include "Ota.h"
#include "SensorData.h"

#if USE_REAL_TIME
#include "Rtc.h"
#endif

const char *version_tag = VERSION_PREFIX VERSION_SUFFIX;

Config config;

char devName[10];
MqttClient *mqtt;

void wifiSetup()
{
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);
    log("Wifi off");
}

void saveWifiSettings()
{
    Storage.Channel(WiFi.channel());
    Storage.Ip(WiFi.localIP());
    Storage.Gateway(WiFi.gatewayIP());
    Storage.Subnet(WiFi.subnetMask());
    Storage.Dns(WiFi.dnsIP());
    Storage.ApMac(WiFi.BSSID());
}

bool wifiConnectBlocking()
{
    WiFi.forceSleepWake();
    delay(1);
    WiFi.persistent(false);

    log("Start wifi");

    WiFi.mode(WIFI_STA);
    if (Storage.IsValid())
    {
        WiFi.config(Storage.Ip(), Storage.Gateway(), Storage.Subnet(), Storage.Dns());
        WiFi.begin(config.WiFi.SSID, config.WiFi.Password, Storage.Channel(), Storage.ApMac(), true);
    }
    else
    {
        WiFi.begin(config.WiFi.SSID, config.WiFi.Password);
    }

    int retries = 0;
    int wifiStatus = WiFi.status();
    while (wifiStatus != WL_CONNECTED)
    {
        retries++;
        if (retries == 100)
        {
            WiFi.disconnect();
            delay(10);
            WiFi.forceSleepWake();
            delay(10);
            WiFi.forceSleepWake();
            delay(10);
            WiFi.begin(config.WiFi.SSID, config.WiFi.Password);
        }

        if (retries == 600)
        {
            WiFi.disconnect(true);
            delay(1);
            WiFi.mode(WIFI_OFF);
            return false;
        }

        delay(50);
        wifiStatus = WiFi.status();
    }

    MDNS.begin(devName);

    log("Wifi connected after %d loops", retries);

    saveWifiSettings();
    return true;
}

void publishAllData(const DataModule &dataModule)
{
    auto data = SensorData();
    data.deviceName = devName;
    data.version = version_tag;
    data.ip = WiFi.localIP();
    data.mac = WiFi.macAddress();
    data.connectionTime = millis();
    data.rssi = WiFi.RSSI();

    data.temperatureIn = dataModule.getTemp();
    data.humidityIn = dataModule.getHumidity();

    String serialized;
    serializeData(data, serialized);

    char topic[50];
    sprintf(topic, "env/%s/data", devName);

    mqtt->publish(topic, serialized.c_str());
}

void setup()
{
    getDeviceName(devName);

    Serial.begin(76800);
    Serial.printf("Device '%s', version '%s'\n", devName, version_tag);
    delay(100);
#if !DEBUG
    Serial.end();
#endif

    if (!LittleFS.begin())
    {
        log("Failed to initialize LittleFS");
    }

    if (!loadConfiguration(config))
    {
        log("Could not load config");
    }

    wifiSetup();

    DataModule dataModule;
    dataModule.setup();

#if MOCK_SENSOR
    dataModule.setTemp(23.45);
    dataModule.setHumidity(54.32);
#else
    SensorsModule sensors(&dataModule);
    sensors.setup();
    sensors.takeMeasurements();
#endif

#if USE_REAL_TIME
    auto rtc = new RtcModule();
#if USE_RTC
    bool clockSet = false;
    if (rtc->trySetClockFromRtc())
    {
        clockSet = true;
    }
#endif
#endif

    if (!wifiConnectBlocking())
    {
        // No wifi - go to sleep
        ESP.deepSleep(SLEEPTIME, WAKE_RF_DISABLED);
    }

#if USE_REAL_TIME
#if DEBUG
    auto beforeTime = millis();
#endif

#if USE_RTC
    if (!clockSet)
    {
        rtc->setRtcAndClockFromNtp(config.NtpServer.c_str());
    }
#else
    rtc->setClockFromNtp(config.NtpServer.c_str());
#endif
    delete rtc;

#if DEBUG
    auto setClockTime = millis() - beforeTime;
    log("SetClock took %lu millis", setClockTime);
#endif

#endif
    Storage.Save();

#if DEBUG
    time_t now = time(nullptr);
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    log("Current UTC time: %s", asctime(&timeinfo));
#endif

    mqtt = new MqttClient(devName, config.Mqtt.Host.c_str(), config.Mqtt.Port);
    mqtt->setup();
    if (mqtt->connect())
    {
        log("Sending data");
        publishAllData(dataModule);
        mqtt->disconnect();
    }
    else
    {
        log("Mqtt failed, rc=%d\n", mqtt->state());
    }

#if OTA_ENABLED
    Ota ota(config.Ota.Url.c_str(), version_tag);
    ota.setup();

#if !USE_REAL_TIME
    ota.setX509Time(BUILD_TIME);
#endif

    ota.update();
#endif

    log("Millis = %lu\n", millis());
    log("Go back to sleep");

    WiFi.disconnect(true);
    delay(1);

    ESP.deepSleep(calculateSleepTime(SLEEPTIME, MillisToMicros(millis())), WAKE_RF_DISABLED);
}

void loop()
{
}