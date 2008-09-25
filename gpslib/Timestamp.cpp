// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <errno.h>
#include <ctime>
#include "Timestamp.h"

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

using namespace std;


#ifdef _MSC_VER
    static inline time_t timegm(struct tm* tm)
    {
        time_t ret = mktime(tm);
        long _Timezone;
        _get_timezone(&_Timezone);
        // rückgängig machen, was mktime() verbockt hat
        return ret - _Timezone;
    }
#endif


namespace GPS {

    Timestamp::Timestamp(void) : ms(0)
    { /* ... */ }


    Timestamp::Timestamp(timestamp_t msSinceEpoch) : ms(msSinceEpoch)
    { /* ... */ }


    Timestamp::Timestamp(const std::string& str)
    {
        *this = str.c_str();
    }


    Timestamp::Timestamp(const char* str) : ms(0)
    {
        if (str != NULL)
        {
            int rc;
            int year;
            int month;
            int day;
            int hours;
            int minutes;
            int seconds = 0;
            char offsetsign = '+';
            int offsethours = 0;
            int offsetminutes = 0;
#if defined(_WIN32) && (_MSC_VER >= 1400)
            rc = sscanf_s(str, "%04d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
                &year, &month, &day, &hours, &minutes, &seconds,
                &offsetsign, sizeof(char), &offsethours, &offsetminutes);
#else
            rc = sscanf(str, "%04d-%02d-%02dT%02d:%02d:%02d%[+-]%02d:%02d",
                &year, &month, &day, &hours, &minutes, &seconds,
                &offsetsign, &offsethours, &offsetminutes);
#endif
            if (rc >= 5 && rc <= 10) {
                if (offsetsign == '-') {
                    offsethours = -offsethours;
                    offsetminutes = -offsetminutes;
                }
                ms = toMs(year, month, day, hours, minutes, seconds, offsethours, offsetminutes);
                return;
            }

#if defined(_WIN32) && (_MSC_VER >= 1400)
            rc = sscanf_s(str, "%04d-%02d-%02dT%02d:%02d:%02d",
                &year, &month, &day, &hours, &minutes, &seconds);
#else
            rc = sscanf(str, "%04d-%02d-%02dT%02d:%02d:%02d",
                &year, &month, &day, &hours, &minutes, &seconds);
#endif
            if (rc >= 5 && rc <= 7) {
                ms = toMs(year, month, day, hours, minutes, seconds);
                return;
            }
        }
    }


    Timestamp::Timestamp(int year, int month, int day, int hours, int minutes, int seconds, int offsethours, int offsetminutes)
    {
        ms = toMs(year, month, day, hours, minutes, seconds, offsethours, offsetminutes);
    }


    timestamp_t Timestamp::toMs(int year, int month, int day,
        int hours, int minutes, int seconds,
        int offsethours, int offsetminutes)
    {
        struct tm t;
        t.tm_sec = seconds;
        t.tm_min = minutes + offsetminutes;
        t.tm_hour = hours + offsethours;
        t.tm_mday = day;
        t.tm_mon = month - 1;
        t.tm_year = year - 1900;
        t.tm_wday = 0;
        t.tm_yday = 0;
        t.tm_isdst = 0;
        timestamp_t ts = 1000ULL * (timestamp_t) timegm(&t);
        return ts;
    }


    string Timestamp::toString(const char* fmt) const
    {
        if (fmt == NULL)
            fmt = "%Y-%m-%dT%H:%M:%SZ";
        char buf[100];
        time_t seconds = (time_t) (ms / 1000);
        struct tm t;
#if defined(_WIN32) && (_MSC_VER >= 1400)
        errno_t rc = gmtime_s(&t, &seconds);
        if (rc != 0)
            return string();
#else
        gmtime_r(&seconds, &t);
#endif
        strftime(buf, sizeof(buf), fmt, &t);
        return string(buf);
    }


    Timestamp& Timestamp::operator+=(const Timestamp& o)
    {
        ms += o.ms;
        return *this;
    }


    Timestamp& Timestamp::operator-=(const Timestamp& o)
    {
        ms -= o.ms;
        return *this;
    }


    Timestamp& Timestamp::operator+=(timestamp_t msecs)
    {
        ms += msecs;
        return *this;
    }


    Timestamp& Timestamp::operator-=(timestamp_t msecs)
    {
        ms -= msecs;
        return *this;
    }


    bool operator==(const Timestamp& t1, const Timestamp& t2)
    {
        return t1.ms == t2.ms;
    }


    bool operator!=(const Timestamp& t1, const Timestamp& t2)
    {
        return t1.ms != t2.ms;
    }


    bool operator>(const Timestamp& t1, const Timestamp& t2)
    {
        return t1.ms > t2.ms;
    }


    bool operator<(const Timestamp& t1, const Timestamp& t2)
    {
        return t1.ms < t2.ms;
    }


    bool operator>=(const Timestamp& t1, const Timestamp& t2)
    {
        return t1.ms >= t2.ms;
    }


    bool operator<=(const Timestamp& t1, const Timestamp& t2)
    {
        return t1.ms <= t2.ms;
    }


    const Timestamp operator-(const Timestamp& t1, const Timestamp& t2)
    {
        if (t1 > t2)
            return Timestamp(t1.ms - t2.ms);
        else
            return Timestamp(t2.ms - t1.ms);
    }


    const Timestamp operator-(const Timestamp& t1, timestamp_t msecs)
    {
        if (t1.ms > msecs)
            return Timestamp(t1.ms - msecs);
        else
            return Timestamp(msecs - t1.ms);
    }


    const Timestamp operator+(const Timestamp& t1, const Timestamp& t2)
    {
        return Timestamp(t1.ms + t2.ms);
    }


    const Timestamp operator+(const Timestamp& t1, timestamp_t msecs)
    {
        return Timestamp(t1.ms + msecs);
    }


    Timestamp Timestamp::current(void)
    {
        return Timestamp(1000ULL * (timestamp_t) time(0));
    }


    time_t Timestamp::seconds(void) const
    {
        return ms / 1000;
    }


    timestamp_t Timestamp::milliseconds(void) const
    {
        return ms;
    }


    time_t Timestamp::offsetToSeconds(std::string offset)
    {
        char sign;
        int hours;
        int minutes;
        int seconds = 0;
        int rc;
#if defined(_WIN32) && (_MSC_VER >= 1400)
        rc = sscanf_s(offset.c_str(), "%c%2d:%2d:%2d", &sign, sizeof(char), &hours, &minutes, &seconds);
#else
        rc = sscanf(offset.c_str(), "%c%2d:%2d:%2d", &sign, &hours, &minutes, &seconds);
#endif
        if (rc == 4 || rc == 3)
        {
            time_t res = hours * 3600 + minutes * 60 + seconds;
            return (sign == '-')? -res : res;
        }
        return 0;
    }


    std::ostream& operator<<(std::ostream& os, const Timestamp& t)
    {
        os << t.seconds();
        return os;
    }


};
