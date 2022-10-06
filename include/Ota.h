#ifndef Ota_h
#define Ota_h

#include "ESP8266httpUpdate.h"

#include "log.h"
#include "Module.h"

class Ota : public Module {
    public:
        Ota(const char *url, const char *fingerprint, const char *version_tag);
        void setup(void) override;
        void update(const unsigned long) override;
    private:
        const String *_url;
        const char *_fingerprint;
        const char *_version_tag;
        WiFiClient *_wifi;
        ESP8266HTTPUpdate *_client;
};

#endif