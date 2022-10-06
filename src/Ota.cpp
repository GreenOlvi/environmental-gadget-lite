#include "Ota.h"

Ota::Ota(const char *url, const char *fingerprint, const char *version_tag)
    : _url(new String(url)), _fingerprint(fingerprint), _version_tag(version_tag), _client(new ESP8266HTTPUpdate()) {
    log("Ota ctor()");
}

void Ota::setup(void) {
    log("Ota setup()");
    auto wifi = new WiFiClientSecure();

    if (!wifi->setFingerprint(_fingerprint)) {
        log("Could not set fingerprint");
        return;
    }

    _wifi = wifi;
}

void Ota::update(const unsigned long) {
    log("Ota update()");
    auto result = _client->update(*this->_wifi, *this->_url, _version_tag);

    switch (result) {
        case HTTP_UPDATE_FAILED:
            log("Update failed");
            break;
        case HTTP_UPDATE_NO_UPDATES:
            log("No updates");
            break;
        case HTTP_UPDATE_OK:
            log("Update OK");
            break;
    }
}