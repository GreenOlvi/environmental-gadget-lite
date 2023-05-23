#ifndef PersistentStorage_h
#define PersistentStorage_h

#include <Arduino.h>
#include "common.h"
#include "log.h"

struct RtcData
{
    uint32_t crc32;
    uint8_t channel;
    uint8_t ap_mac[6];
    uint32_t ip;
    uint32_t gateway;
    uint32_t subnet;
    uint32_t dns;
    uint8_t padding;
};

class PersistentStorage
{
    public:
        PersistentStorage(void);

        bool Load(void);
        void Save(void);

        bool IsValid(void);

        uint8_t Channel(void);
        void Channel(uint8_t value);

        uint32_t Ip(void);
        void Ip(uint32_t value);

        uint32_t Gateway(void);
        void Gateway(uint32_t value);

        uint32_t Subnet(void);
        void Subnet(uint32_t value);

        uint32_t Dns(void);
        void Dns(uint32_t value);

        const uint8_t* ApMac(void);
        void ApMac(uint8_t* value);

        void PrintDebug(void);

    private:
        RtcData _data;
        bool _isLoaded = false;
        bool _isValid = false;
        bool _changed = false;
};

extern PersistentStorage Storage;

#endif