#ifndef SECRETS_H
#define SECRETS_H

#define VERSION_PREFIX "1.0.0"

#ifdef DEBUG
#define VERSION_SUFFIX "-debug"
#else
#define VERSION_SUFFIX
#endif

#define STASSID "..."
#define STAPSK "..."
#define MQTT_HOST "..."
#define MQTT_PORT 1883

#ifdef DEBUG
#undef OTA_ENABLED
#else
#define OTA_ENABLED
#endif

#define OTA_URL "..."
#define OTA_CERT_FINGERPRINT "..."

#endif