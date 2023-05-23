#include "Rtc.h"

RtcModule::RtcModule() : _rtc(new RtcDS3231<TwoWire>(Wire)) {
}

RtcModule::~RtcModule() {
    delete _rtc;
}

void RtcModule::setupRtcClock(const char *ntpServer)
{
    _rtc->Begin();

    if (!_rtc->GetIsRunning())
    {
        log("RTC was not running, starting now");
        _rtc->SetIsRunning(true);
    }

    if (!_rtc->IsDateTimeValid())
    {
        auto error = _rtc->LastError();
        if (!error)
        {
            log("Rtc lost confidence in the datetime");
            setRtcTimeFromNtp(ntpServer);
        }
        else
        {
            log("RTC error: %d", error);
        }
    }
    else
    {
        log("No error");
        setClockFromRtc();
    }
}

void RtcModule::setRtcTimeFromNtp(const char *ntpServer) {
    setClockFromNtp(ntpServer);

    time_t now = time(nullptr);
    struct tm dt;
    gmtime_r(&now, &dt);

    RtcDateTime nowNtp = RtcDateTime(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
    _rtc->SetDateTime(nowNtp);
}

void RtcModule::setClockFromRtc(void) {
    log("Set time from RTC");
    setTZ(TIMEZONE);
    auto dt = _rtc->GetDateTime();
    struct tm t = {0};
    t.tm_year = dt.Year() - 1900;
    t.tm_mon = dt.Month() - 1;
    t.tm_mday = dt.Day();
    t.tm_hour = dt.Hour();
    t.tm_min = dt.Minute();
    t.tm_sec = dt.Second();
    time_t time = mktime(&t);
    timeval tv = { time, 0 };
    settimeofday(&tv, nullptr);
}

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
        countof(datestring),
        PSTR("%04u-%02u-%02u %02u:%02u:%02u"),
        dt.Year(),
        dt.Month(),
        dt.Day(),
        dt.Hour(),
        dt.Minute(),
        dt.Second() );

    log("%s", datestring);
}
