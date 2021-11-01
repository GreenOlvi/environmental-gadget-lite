#ifndef SECRETS_H

#define FIRMWARE_VERSION "0.0.4"

#undef DEBUG

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

#error "Fill the values"

#endif