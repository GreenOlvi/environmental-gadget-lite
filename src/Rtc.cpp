#include "Rtc.h"

RtcModule::RtcModule() : _rtc(new RtcDS3231<TwoWire>(Wire))
{
}

RtcModule::~RtcModule()
{
    delete _rtc;
}

bool RtcModule::trySetClockFromRtc()
{
    _rtc->Begin();

    if (!_rtc->GetIsRunning())
    {
        log("RTC was not running, starting now");
        _rtc->SetIsRunning(true);
        return false;
    }

    if (isRtcDataValid())
    {
        return false;
    }

    setClockFromRtc();
    if (isSyncNeeded())
    {
        log("Scheduled Ntp sync");
        return false;
    }

    return true;
}

bool RtcModule::isRtcDataValid()
{
    if (!_rtc->IsDateTimeValid())
    {
#if DEBUG
        auto error = _rtc->LastError();
        if (error)
        {
            log("RTC error: %d", error);
        }
        else
        {
            log("Rtc lost confidence in the datetime");
        }
#endif
        return true;
    }
    return false;
}

bool RtcModule::isSyncNeeded()
{
    if (!Storage.IsValid())
    {
        return true;
    }

    auto now = time(nullptr);
    auto lastSync = Storage.LastRtcSync();
    if (now - lastSync > RtcSyncIntervalInSeconds)
    {
        return true;
    }

    return false;
}

void RtcModule::setClockFromNtp(const char* ntpServer)
{
    configTime(TIMEZONE, ntpServer);

    log("%s", "Waiting for NTP time sync");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2)
    {
        delay(250);
        now = time(nullptr);
    }
}

void RtcModule::setRtcAndClockFromNtp(const char* ntpServer)
{
    setClockFromNtp(ntpServer);

    time_t now = time(nullptr);
    struct tm dt;
    gmtime_r(&now, &dt);

    RtcDateTime nowNtp = RtcDateTime(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
    _rtc->SetDateTime(nowNtp);

    Storage.LastRtcSync((unsigned long)now);
}

void RtcModule::setClockFromRtc(void)
{
    log("Set time from RTC");
    setTZ(TZ_Etc_UTC);
    auto dt = _rtc->GetDateTime();
    struct tm t = { 0 };
    t.tm_year = dt.Year() - 1900;
    t.tm_mon = dt.Month() - 1;
    t.tm_mday = dt.Day();
    t.tm_hour = dt.Hour();
    t.tm_min = dt.Minute();
    t.tm_sec = dt.Second();
    time_t time = mktime(&t);
    timeval tv = { time, 0 };
    settimeofday(&tv, nullptr);
    setTZ(TIMEZONE);
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
        dt.Second());

    log("%s", datestring);
}
