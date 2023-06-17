#include "common.h"

uint32_t calculateCRC32(const uint8_t* data, size_t length)
{
    uint32_t crc = 0xffffffff;
    while (length--)
    {
        uint8_t c = *data++;
        for (uint32_t i = 0x80; i > 0; i >>= 1)
        {
            bool bit = crc & 0x80000000;
            if (c & i)
            {
                bit = !bit;
            }

            crc <<= 1;
            if (bit)
            {
                crc ^= 0x04c11db7;
            }
        }
    }

    return crc;
}

float roundTwoDigits(float value)
{
    float val = (int)(value * 100 + .5);
    return (float)val / 100;
}

void getDeviceName(char *name)
{
    auto mac = WiFi.macAddress();
    strcpy(name, "ls-");
    int nIndex = 3;
    for (unsigned int i = 9; i < 17; i++)
    {
        char c = mac[i];
        if (c != ':')
        {
            name[nIndex++] = tolower(c);
        }
    }
    name[nIndex] = 0;
}

unsigned long calculateSleepTime(const unsigned long sleepInterval, const unsigned long runningTime)
{
    auto sleep = sleepInterval - (runningTime % sleepInterval);
#if DEBUG
    log("Will be sleeping for %lu", sleep);
#endif
    return sleep;
}
