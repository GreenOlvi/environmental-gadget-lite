#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "secrets.h"
#include "Mqtt.h"
#include "SensorsModule.h"

MqttClient mqtt(HOSTNAME, MQTT_HOST, MQTT_PORT);
DataModule dataModule;
SensorsModule sensors(&dataModule);

void wifiConnectBlocking() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
  }
}

void setup() {
  sensors.setup();

  wifiConnectBlocking();

  mqtt.setup();

  MDNS.begin(HOSTNAME);
}

void publish(const char* type, float value) {
  char topic[30];
  sprintf(topic, "env/%s/%s", ROOM, type);
  mqtt.publish(topic, value);
}

unsigned long nextMqttPublish = 0;
void mqttPublishUpdate(const unsigned long t) {
  if (t >= nextMqttPublish) {
    publish("temp_in", dataModule.getTemp());
    publish("hum_in", dataModule.getHumidity());
    nextMqttPublish = t + 60 * 1000;
  }
}

void loop() {
  mqttPublishUpdate(millis());
  sensors.update(millis());
  dataModule.update(millis());
  mqtt.update(millis());
}