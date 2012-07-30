// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include <string>
#include <ctime>
#include <iostream>
#include <cstdio>

#include "Duration.h"
#include "Timestamp.h"

using namespace std;

namespace GPS {

    Duration::Duration(const char* str) : ms(0)
    {
        if (str != NULL)
        {
            int rc;
            int days = 0;
            int hours;
            int minutes;
            int seconds;
#if defined(_WIN32) && (_MSC_VER >= 1400)
            rc = sscanf_s(str, "%d:%02d:%02d", &hours, &minutes, &seconds);
#else
            rc = sscanf(str, "%d:%02d:%02d", &hours, &minutes, &seconds);
#endif
            while (hours > 24) {
                hours -= 24;
                ++days;
            }
            if (rc == 3)
                ms = toMs(days, hours, minutes, seconds);
        }
    }


    Duration::Duration(int days, int hours, int minutes, int seconds, int milliseconds)
    {
        ms = toMs(days, hours, minutes, seconds, milliseconds);
    }


    void Duration::start(void)
    {
#ifdef WIN32
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&t0);
#else
        t0 = Timestamp::current();
#endif
    }


    void Duration::stop(void)
    {
#ifdef WIN32
        LARGE_INTEGER t1;
        QueryPerformanceCounter(&t1);
        ms = 1000LL * (t1.QuadPart - t0.QuadPart) / freq.QuadPart;
#else
        ms = (Timestamp::current() - t0).milliseconds();
#endif
    }


    timestamp_t Duration::toMs(int days, int hours, int minutes, int seconds, int milliseconds)
    {
        timestamp_t ts = 1000ULL * ((timestamp_t) days * 86400ULL + (timestamp_t) hours * 3600ULL + (timestamp_t) minutes * 60ULL + (timestamp_t) seconds) + milliseconds;
        return ts;
    }


    string Duration::toString(const char* fmt) const
    {
        if (fmt == NULL)
            fmt = "%H:%M:%S";
        char buf[100];
        time_t secs = seconds();
        struct tm t;
#if defined(_WIN32) && (_MSC_VER >= 1400)
        errno_t rc = gmtime_s(&t, &secs);
        if (rc != 0)
            return string();
#else
        gmtime_r(&secs, &t);
#endif
        strftime(buf, sizeof(buf), fmt, &t);
        return string(buf);
    }


    Duration& Duration::operator+=(const Duration& o)
    {
        ms += o.ms;
        return *this;
    }


    Duration& Duration::operator-=(const Duration& o)
    {
        ms -= o.ms;
        return *this;
    }


    Duration& Duration::operator+=(timestamp_t msecs)
    {
        ms += msecs;
        return *this;
    }


    Duration& Duration::operator-=(timestamp_t msecs)
    {
        ms -= msecs;
        return *this;
    }


    bool operator==(const Duration& t1, const Duration& t2)
    {
        return t1.ms == t2.ms;
    }


    bool operator!=(const Duration& t1, const Duration& t2)
    {
        return t1.ms != t2.ms;
    }


    bool operator>(const Duration& t1, const Duration& t2)
    {
        return t1.ms > t2.ms;
    }


    bool operator<(const Duration& t1, const Duration& t2)
    {
        return t1.ms < t2.ms;
    }


    bool operator>=(const Duration& t1, const Duration& t2)
    {
        return t1.ms >= t2.ms;
    }


    bool operator<=(const Duration& t1, const Duration& t2)
    {
        return t1.ms <= t2.ms;
    }


    const Duration operator-(const Duration& t1, const Duration& t2)
    {
        if (t1 > t2)
            return Duration(t1.ms - t2.ms);
        else
            return Duration(t2.ms - t1.ms);
    }


    const Duration operator-(const Duration& t1, timestamp_t msecs)
    {
        if (t1.ms > msecs)
            return Duration(t1.ms - msecs);
        else
            return Duration(msecs - t1.ms);
    }


    const Duration operator+(const Duration& t1, const Duration& t2)
    {
        return Duration(t1.ms + t2.ms);
    }


    const Duration operator+(const Duration& t1, timestamp_t msecs)
    {
        return Duration(t1.ms + msecs);
    }


    std::ostream& operator<<(std::ostream& os, const Duration& t)
    {
        if (os.good())
            os << t.seconds();
        return os;
    }

};
