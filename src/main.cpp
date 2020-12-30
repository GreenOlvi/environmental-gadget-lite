#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "secrets.h"
#include "Mqtt.h"
#include "SensorsModule.h"

MqttClient mqtt("envGadgetLite01", MQTT_HOST, MQTT_PORT);
DataModule dataModule;
SensorsModule sensors(&dataModule);

void setup() {
  sensors.setup();

  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  mqtt.setup();

  MDNS.begin(HOSTNAME);
}

void publish(const char* type, float value) {
  char topic[30];
  sprintf(topic, "env/bedroom/%s", type);
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