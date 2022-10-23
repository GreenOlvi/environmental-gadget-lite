#ifndef Ota_h
#define Ota_h

#include "ESP8266httpUpdate.h"
#include "LittleFS.h"
#include "log.h"

class Ota {
    public:
        Ota(const char *url, const char *version_tag);
        void setup(void);
        void update();
        void setX509Time(const long time);
    private:
        const String *_url;
        const char *_version_tag;
        BearSSL::CertStore *_certStore;
        WiFiClient *_wifi;
        ESP8266HTTPUpdate *_client;
        bool _setX509Time = false;
        long _time = 0;
};

#endif