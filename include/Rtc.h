#ifndef Rtc_h
#define Rtc_h

#include <Wire.h>
#include <RtcDS3231.h>
#include "log.h"
#include "common.h"

class RtcModule {
    public:
        RtcModule(void);
        ~RtcModule(void);
        bool trySetClockFromRtc(void);
        void setClockFromNtp(const char *ntpServer);
        void setRtcAndClockFromNtp(const char *ntpServer);

    private:
        void setClockFromRtc(void);
        RtcDS3231<TwoWire>* _rtc;
};

#endif