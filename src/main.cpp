#define VERSION_PREFIX "1.1.3"

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

#ifndef BUILD_TIME
#define BUILD_TIME 0
#endif

#if DEBUG
#define SLEEPTIME 10e6
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
#include "Mqtt.h"
#include "SensorsModule.h"
#include "Ota.h"
#include "SensorData.h"

#if USE_RTC
  #include "Rtc.h"
#endif

const char *version_tag = VERSION_PREFIX VERSION_SUFFIX;

struct RtcData {
  uint32_t crc32;
  uint8_t channel;
  uint8_t ap_mac[6];
  uint32_t ip;
  uint32_t gateway;
  uint32_t subnet;
  uint32_t dns;
  uint8_t padding;
};

RtcData rtcData;

IPAddress ip;
IPAddress gateway;
IPAddress subnet;
IPAddress dns;

Config config;

char devName[10];
MqttClient *mqtt;

void getDeviceName(char* name) {
  auto mac = WiFi.macAddress();
  strcpy(name, "ls-");
  int nIndex = 3;
  for (unsigned int i = 9; i < 17; i++)  {
    char c = mac[i];
    if (c != ':') {
      name[nIndex++] = tolower(c);
    }
  }
  name[nIndex] = 0;
}

void wifiSetup() {
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
  log("Wifi off");
}

bool tryReadWifiSettings() {
  bool rtcValid = false;
  if (ESP.rtcUserMemoryRead(0, (uint32_t*)&rtcData, sizeof(rtcData))) {
    uint32_t crc = calculateCRC32(((uint8_t*)&rtcData) + 4, sizeof(rtcData) - 4);
    rtcValid = crc == rtcData.crc32;
    if (rtcValid) {
      ip = IPAddress(rtcData.ip);
      gateway = IPAddress(rtcData.gateway);
      subnet = IPAddress(rtcData.subnet);
      dns = IPAddress(dns);
    }
  }
  log("RTC data valid: %s", rtcValid ? "true" : "false");
  return rtcValid;
}

void saveWifiSettings() {
  rtcData.channel = WiFi.channel();
  rtcData.ip = ip;
  rtcData.gateway = gateway;
  rtcData.subnet = subnet;
  rtcData.dns = dns;
  memcpy(rtcData.ap_mac, WiFi.BSSID(), 6);
  rtcData.crc32 = calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
  ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(rtcData));
}

void wifiConnectBlocking() {
  WiFi.forceSleepWake();
  delay(1);
  WiFi.persistent(false);

  log("Start wifi");

  WiFi.mode(WIFI_STA);
  if (ip) {
    WiFi.config(ip, gateway, subnet, dns);
  }

  if (tryReadWifiSettings()) {
    WiFi.begin(config.WiFi.SSID, config.WiFi.Password, rtcData.channel, rtcData.ap_mac, true);
  } else {
    WiFi.begin(config.WiFi.SSID, config.WiFi.Password);
  }

  int retries = 0;
  int wifiStatus = WiFi.status();
  while (wifiStatus != WL_CONNECTED) {
    retries++;
    if (retries == 100) {
      WiFi.disconnect();
      delay(10);
      WiFi.forceSleepWake();
      delay(10);
      WiFi.forceSleepWake();
      delay(10);
      WiFi.begin(config.WiFi.SSID, config.WiFi.Password);
    }

    if (retries == 600) {
      WiFi.disconnect(true);
      delay(1);
      WiFi.mode(WIFI_OFF);
      ESP.deepSleep(SLEEPTIME, WAKE_RF_DISABLED);
      return;
    }

    delay(50);
    wifiStatus = WiFi.status();
  }

  MDNS.begin(devName);

  log("Wifi connected");
  log("IP address: %s", WiFi.localIP().toString().c_str());

  saveWifiSettings();
}

void publishAllData(const DataModule &dataModule) {
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

void setup() {
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

  wifiConnectBlocking();

#if USE_NTP
#if DEBUG
  auto beforeTime = millis();
#endif

#if USE_RTC
  auto rtc = new RtcModule();
  rtc->setupRtcClock(config.NtpServer.c_str());
  delete rtc;
#else
  setClockFromNtp(config.NtpServer.c_str());
#endif

#if DEBUG
  auto setClockTime = millis() - beforeTime;
  log("SetClock took %lu millis", setClockTime);
#endif

#endif

#if DEBUG
  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  log("Current UTC time: %s", asctime(&timeinfo));
#endif

  mqtt = new MqttClient(devName, config.Mqtt.Host.c_str(), config.Mqtt.Port);
  mqtt->setup();
  if (mqtt->connect()) {
    log("Sending data");
    publishAllData(dataModule);
    mqtt->disconnect();
  } else {
    log("Mqtt failed, rc=%d\n", mqtt->state());
  }

#if OTA_ENABLED
  Ota ota(config.Ota.Url.c_str(), version_tag);
  ota.setup();

#if !USE_NTP
  ota.setX509Time(BUILD_TIME);
#endif

  ota.update();
#endif

  log("Millis = %lu\n", millis());
  log("Go back to sleep");

  WiFi.disconnect(true);
  delay(1);
  ESP.deepSleep(SLEEPTIME, WAKE_RF_DISABLED);
}

void loop() {
}