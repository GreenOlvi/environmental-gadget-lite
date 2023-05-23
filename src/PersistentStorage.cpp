#include "PersistentStorage.h"

PersistentStorage Storage = PersistentStorage();

PersistentStorage::PersistentStorage()
{
}

bool PersistentStorage::Load()
{
    _isValid = false;
    if (ESP.rtcUserMemoryRead(0, (uint32_t *)&_data, sizeof(_data)))
    {
        uint32_t crc = calculateCRC32(((uint8_t *)&_data) + 4, sizeof(_data) - 4);
        _isValid = crc == _data.crc32;
    }
    _isLoaded = true;
    log("RTC data valid: %s", _isValid ? "true" : "false");
    return _isValid;
}

void PersistentStorage::Save()
{
  _data.crc32 = calculateCRC32(((uint8_t *)&_data) + 4, sizeof(_data) - 4);
  ESP.rtcUserMemoryWrite(0, (uint32_t *)&_data, sizeof(_data));
}

bool PersistentStorage::IsValid()
{
    if (!_isLoaded)
    {
        Load();
    }
    return _isValid;
}

uint8_t PersistentStorage::Channel()
{
    return _data.channel;
}

void PersistentStorage::Channel(uint8_t value)
{
    if (value != _data.channel)
    {
        _data.channel = value;
        _changed = true;
    }
}

uint32_t PersistentStorage::Ip()
{
    return _data.ip;
}

void PersistentStorage::Ip(uint32_t value)
{
    if (value != _data.ip)
    {
        _data.ip = value;
        _changed = true;
    }
}

uint32_t PersistentStorage::Gateway()
{
    return _data.gateway;
}

void PersistentStorage::Gateway(uint32_t value)
{
    if (value != _data.gateway)
    {
        _data.gateway = value;
        _changed = true;
    }
}

uint32_t PersistentStorage::Subnet()
{
    return _data.subnet;
}

void PersistentStorage::Subnet(uint32_t value)
{
    if (value != _data.subnet)
    {
        _data.subnet = value;
        _changed = true;
    }
}

uint32_t PersistentStorage::Dns()
{
    return _data.dns;
}

void PersistentStorage::Dns(uint32_t value)
{
    if (value != _data.dns)
    {
        _data.dns = value;
        _changed = true;
    }
}

const uint8_t* PersistentStorage::ApMac()
{
    return _data.ap_mac;
}

void PersistentStorage::ApMac(uint8_t* value)
{
    for (int i = 0; i < 6; i++)
    {
        if (_data.ap_mac[i] != value[i])
        {
            _changed = true;
            _data.ap_mac[i] = value[i];
        }
    }
}

void PersistentStorage::PrintDebug()
{
    log("persistent storage (%zu bytes)", sizeof(RtcData));
    log("CRC32=%04X", _data.crc32);
    log("Channel=%d", _data.channel);
    log("AP MAC=%02X:%02X:%02X:%02X:%02X:%02X", _data.ap_mac[0], _data.ap_mac[1], _data.ap_mac[2], _data.ap_mac[3], _data.ap_mac[4], _data.ap_mac[5]);
    log("IP=%s", IPAddress(_data.ip).toString().c_str());
    log("Gateway=%s", IPAddress(_data.gateway).toString().c_str());
    log("Subnet=%s", IPAddress(_data.subnet).toString().c_str());
    log("DNS=%s", IPAddress(_data.dns).toString().c_str());
}