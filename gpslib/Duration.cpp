// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <string>
#include <ctime>

#include "Duration.h"
#include "Timestamp.h"

using namespace std;

namespace GPS {

    Duration::Duration(void) : ms(0)
    { /* ... */ }


    Duration::Duration(timestamp_t _MS) : ms(_MS)
    { /* ... */ }


    Duration::Duration(const std::string& str)
    {
        *this = str.c_str();
    }


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
