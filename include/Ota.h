#ifndef Ota_h
#define Ota_h

#include "ESP8266httpUpdate.h"

#include "secrets.h"
#include "log.h"
#include "Module.h"

class Ota : public Module {
    public:
        Ota(const char *url);
        void setup(void) override;
        void update(const unsigned long) override;
    private:
        const String *_url;
        WiFiClient *_wifi;
        ESP8266HTTPUpdate *_client;
        const char *_fingerprint = OTA_CERT_FINGERPRINT;
};

#endif