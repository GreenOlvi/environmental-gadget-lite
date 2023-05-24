#ifndef Rtc_h
#define Rtc_h

#include <Wire.h>
#include <RtcDS3231.h>
#include "log.h"
#include "common.h"
#include "PersistentStorage.h"

#define RtcSyncIntervalInSeconds (60*60*24*7) // once a week

class RtcModule {
    public:
        RtcModule(void);
        ~RtcModule(void);
        bool trySetClockFromRtc(void);
        void setClockFromNtp(const char *ntpServer);
        void setRtcAndClockFromNtp(const char *ntpServer);

    private:
        void setClockFromRtc(void);
        bool isRtcDataValid(void);
        bool isSyncNeeded(void);
        RtcDS3231<TwoWire>* _rtc;
};

#endif