// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include "Waypoint.h"
#include "Timestamp.h"

#include <cmath>
#include <cstdio>

namespace GPS {

    Waypoint::Waypoint(void)
        : GeoCoord(),
        _Speed(0.0),
        _Pace(0.0)

    { /* ... */ }

    Waypoint::Waypoint(double lon, double lat)
        : GeoCoord(lon, lat),
        _Speed(0.0),
        _Pace(0.0)
    { /* ... */ }

    Waypoint::Waypoint(double lon, double lat, DoubleValue ele, Timestamp timestamp)
        : GeoCoord(lon, lat, ele),
          _Timestamp(timestamp),
          _Speed(0.0),
          _Pace(0.0)
    { /* ... */ }

    std::string Waypoint::paceString(void) const
    {
        double p = _Pace;
        unsigned int min = (unsigned int) (_Pace);
        if (min >= 100) { min = 99; p = 99; }
        unsigned int sec = (unsigned int) (60 * (p - (double) min));
#if defined(_WIN32) && (_MSC_VER >= 1400)
        char buf[17];
        sprintf_s(buf, 16, "%2u:%02u", min, sec);
#else
        char buf[256];
        sprintf(buf, "%2u:%02u", min, sec);
#endif
        return buf;
    }

};
