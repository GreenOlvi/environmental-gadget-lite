#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "secrets.h"
#include "common.h"
#include "Mqtt.h"
#include "SensorsModule.h"

#ifdef DEBUG
#define SLEEPTIME 10e6
#else
#define SLEEPTIME 60e6
#endif

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

MqttClient mqtt(HOSTNAME, MQTT_HOST, MQTT_PORT);
DataModule dataModule;
SensorsModule sensors(&dataModule);

void wifiSetup() {
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
#ifdef DEBUG
  Serial.println("Wifi off");
#endif
}

bool tryReadWifiSettings() {
  bool rtcValid = false;
  if (ESP.rtcUserMemoryRead(0, (uint32_t*)&rtcData, sizeof(rtcData))) {
    uint32_t crc = calculateCRC32(((uint8_t*)&rtcData) + 4, sizeof(rtcData) - 4);
    if ( crc == rtcData.crc32 ) {
      ip = IPAddress(rtcData.ip);
      gateway = IPAddress(rtcData.gateway);
      subnet = IPAddress(rtcData.subnet);
      dns = IPAddress(dns);
      rtcValid = true;
    }
  }
#ifdef DEBUG
  Serial.printf("RTC data valid: %s\n", rtcValid ? "true" : "false");
#endif
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

#ifdef DEBUG
  Serial.println("Start wifi");
#endif

  WiFi.mode(WIFI_STA);
  if (ip) {
    WiFi.config(ip, dns, subnet, dns);
  }

  if (tryReadWifiSettings()) {
    WiFi.begin(STASSID, STAPSK, rtcData.channel, rtcData.ap_mac, true);
  } else {
    WiFi.begin(STASSID, STAPSK);
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
      WiFi.begin(STASSID, STAPSK);
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

  MDNS.begin(HOSTNAME);

#ifdef DEBUG
  Serial.println("Wifi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif

  saveWifiSettings();
}

void publish(const char* type, float value) {
  char topic[30];
  sprintf(topic, "env/%s/%s", ROOM, type);
  mqtt.publish(topic, value);
}

void publishSensorData() {
  publish("temp_in", dataModule.getTemp());
  publish("hum_in", dataModule.getHumidity());
}

void publishMetrics() {
  char topic[30];
  snprintf(topic, 30, "tele/%s/awakeTime", ROOM);
  char value[10];
  snprintf(value, 10, "%lu", millis());
  mqtt.publish(topic, value);
}

void setup() {
#ifdef DEBUG
  Serial.begin(76800);
#endif
  wifiSetup();

  dataModule.setup();

  sensors.setup();
  sensors.takeMeasurements();

  wifiConnectBlocking();

  mqtt.setup();
  if (mqtt.connect()) {
#ifdef DEBUG
    Serial.println("Sending data");
#endif
    publishSensorData();
    publishMetrics();
    mqtt.disconnect();
#ifdef DEBUG
  } else {
    Serial.printf("Mqtt failed, rc=%d\n", mqtt.state());
#endif
  }

#ifdef DEBUG
  Serial.printf("Millis = %lu\n", millis());
  Serial.println("Go back to sleep");
#endif
  WiFi.disconnect(true);
  delay(1);
  ESP.deepSleep(SLEEPTIME, WAKE_RF_DISABLED);
}

void loop() {
}