#ifndef Rtc_h
#define Rtc_h

#include <Wire.h>
#include <RtcDS3231.h>
#include "log.h"
#include "common.h"

class RtcModule {
    public:
        RtcModule();
        ~RtcModule();
        void setupRtcClock(const char *ntpServer);

    private:
        void setRtcTimeFromNtp(const char *ntpServer);
        void setClockFromRtc(void);
        RtcDS3231<TwoWire>* _rtc;
};

#endif