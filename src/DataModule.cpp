#include "DataModule.h"

DataModule::DataModule() {
}

void DataModule::setup() {
}

void DataModule::update(const unsigned long t) {
}

void DataModule::setTemp(float temp) {
    _temp = temp;
}

float DataModule::getTemp() const {
    return _temp;
}

void DataModule::setHumidity(float hum) {
    _humidity = hum;
}

float DataModule::getHumidity() const {
    return _humidity;
}