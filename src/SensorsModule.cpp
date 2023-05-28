#include "SensorsModule.h"

SensorsModule::SensorsModule(DataModule* dataModule)
    : _data(dataModule), _dht() {
}

void SensorsModule::setup() {
    _dht.setup(ONE_WIRE_BUS, DHTesp::AM2302);
    _dhtSamplingPeriod = _dht.getMinimumSamplingPeriod();
}

void SensorsModule::update(const unsigned long t) {
    if (t - _lastDhtUpdate >= _dhtSamplingPeriod) {
        takeMeasurements();
        _lastDhtUpdate = t;
    }
}

void SensorsModule::takeMeasurements() {
    TempAndHumidity th = _dht.getTempAndHumidity();
    _data->setTemp(roundTwoDigits(th.temperature));
    _data->setHumidity(roundTwoDigits(th.humidity));
}
