 #include "Ota.h"

Ota::Ota(const char *url, const char *version_tag)
    : _url(new String(url)), _version_tag(version_tag), _client(new ESP8266HTTPUpdate()) {
    log("Ota ctor()");
    _certStore = new BearSSL::CertStore();
}

void Ota::setup(void) {
    log("Ota setup()");
    int numCerts = _certStore->initCertStore(LittleFS, PSTR("/root-ca.idx"), PSTR("/root-ca.ar"));
    log("Initialized %d certs", numCerts);
}

void Ota::setX509Time(const long time) {
    _setX509Time = true;
    _time = time;
}

void Ota::update() {
    log("Ota update()");
    auto wifi = new WiFiClientSecure();
    wifi->setCertStore(_certStore);

    if (_setX509Time) {
        wifi->setX509Time(_time);
        log("Set X509 time to %lu", _time);
    }

    auto result = _client->update(*wifi, *this->_url, _version_tag);
    switch (result) {
        case HTTP_UPDATE_FAILED: {
                log("Update failed");
                char buf[100];
                auto errNum = wifi->getLastSSLError(buf, 100);
                log("Error[%d]: '%s'", errNum, buf);
            }
            break;
        case HTTP_UPDATE_NO_UPDATES:
            log("No updates");
            break;
        case HTTP_UPDATE_OK:
            log("Update OK");
            break;
        default:
            log("Sontething else");
    }
}