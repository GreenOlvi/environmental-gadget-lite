#include "Ota.h"

Ota::Ota(const char *url, const char *version_tag) : _url(new String(url)), _version_tag(version_tag), _client(new ESP8266HTTPUpdate()) {
    logln("Ota ctor()");
}

void Ota::setup(void) {
    logln("Ota setup()");
    auto wifi = new WiFiClientSecure();

    if (!wifi->setFingerprint(_fingerprint)) {
        logln("Could not set fingerprint");
        return;
    }

    _wifi = wifi;
}

void Ota::update(const unsigned long) {
    logln("Ota update()");
    auto result = _client->update(*this->_wifi, *this->_url, _version_tag);

    switch (result) {
        case HTTP_UPDATE_FAILED:
            logln("Update failed");
            break;
        case HTTP_UPDATE_NO_UPDATES:
            logln("No updates");
            break;
        case HTTP_UPDATE_OK:
            logln("Update OK");
            break;
    }
}