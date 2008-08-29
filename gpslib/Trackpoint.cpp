// $Id: Trackpoint.cpp a5c498e66508 2008/08/28 06:31:06 Oliver Lau <oliver.lau@gmail.com> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iomanip>
#include <cmath>
#include <cassert>

#include "Trackpoint.h"
#include "Point.h"
#include "Vector.h"

using namespace std;


namespace GPS {

    Trackpoint::Trackpoint(double lon, double lat, DoubleValue ele, Timestamp timestamp)
        : Waypoint(lon, lat, ele, timestamp),
        _Distance(0.0),
        _Slope(0.0),
        _Speed(0.0),
        _Pace(0.0),
        _Heartrate(),
        _Temperature(),
        _Duration(0),
        _Keep(true)
    { /* ... */ }


    Trackpoint::Trackpoint(const Trackpoint& p)
        : Waypoint(p.longitude(), p.latitude(), p.elevation(), p.timestamp()),
        _Distance(p.distance()),
        _Slope(p.slope()),
        _Speed(p.speed()),
        _Pace(p.pace()),
        _Heartrate(p.heartrate()),
        _Temperature(p.temperature()),
        _Duration(p.duration()),
        _Keep(true)
    { /* ... */ }


    double Trackpoint::perpendicularDistanceElevation(const Trackpoint* p1, const Trackpoint* p2)
    {
        assert(p1 != NULL);
        assert(p2 != NULL);
        const Point R(_Distance, _Elevation.value());
        const Point A(p1->distance(), p1->elevation().value());
        const Point B(p2->distance(), p2->elevation().value());
        return R.perpendicularDistanceToSegment(A, B);
    }


    double Trackpoint::perpendicularDistanceElevation(const Trackpoint& p1, const Trackpoint& p2)
    {
        return perpendicularDistanceElevation(&p1, &p2);
    }


    // TODO: Trackpoint::perpendicularDistance2D() und Trackpoint::perpendicularDistance3D() dahingehend erweitern, dass alle in die Berechnung einbezogenen Punkte nicht mehr in der gleichen UTM-Zone liegen m�ssen.
    double Trackpoint::perpendicularDistance2D(Trackpoint* p1, Trackpoint* p2)
    {
        assert(p1 != NULL);
        assert(p2 != NULL);
        const UTMCoord R = toUTM();
        const UTMCoord A = p1->toUTM();
        const UTMCoord B = p2->toUTM();
        assert(A.zoneNumber() == B.zoneNumber());
        assert(A.zoneNumber() == R.zoneNumber());
        double dist = R.perpendicularDistanceToSegment(A, B);
        return dist;
    }


    double Trackpoint::perpendicularDistance2D(Trackpoint& p1, Trackpoint& p2)
    {
        return perpendicularDistance2D(&p1, &p2);
    }


    double Trackpoint::perpendicularDistance3D(Trackpoint* p1, Trackpoint* p2)
    {
        assert(p1 != NULL);
        assert(p2 != NULL);
        UTMCoord R = toUTM();
        R.setZ(_Elevation.defined()? _Elevation.value() : 0);
        UTMCoord A = p1->toUTM();
        A.setZ(p1->elevation().defined()? p1->elevation().value() : 0);
        UTMCoord B = p2->toUTM();
        B.setZ(p2->elevation().defined()? p2->elevation().value() : 0);
        assert(A.zoneNumber() == B.zoneNumber());
        assert(A.zoneNumber() == R.zoneNumber());
        double dist = R.perpendicularDistanceToSegment(A, B);
        return dist;
    }


    double Trackpoint::perpendicularDistance3D(Trackpoint& p1, Trackpoint& p2)
    {
        return perpendicularDistance3D(&p1, &p2);
    }


    void Trackpoint::dump(ostream& os)
    {
        os  << "  timestamp:  " << _Timestamp.toString() << endl
            << setprecision(12) << noshowpoint
            << "    lon:      " << _Longitude << endl
            << "    lat:      " << _Latitude << endl;
        if (_Elevation.defined())
            os  << setprecision(4) << "    ele:      " << _Elevation << endl;
        os  << setprecision(7)
            << "    distance: " << _Distance << endl
            << setprecision(3)
            << "    slope:    " << _Slope << endl
            << setprecision(3)
            << "    speed:    " << _Speed << endl;
        if (_Heartrate.defined())
            os << "    heartrate:" << _Heartrate << endl;
        if (_Temperature.defined())
            os << "    temp.:    " << _Temperature << endl;
    }


    std::string Trackpoint::paceString(void) const
    {
        int rc;
        char buf[17];
        double p = _Pace;
        int min = (int) (_Pace);
        if (min >= 100) { min = 99; p = 99; }
        int sec = (int) (60 * (p - (double) min));
#if defined(_WIN32) && (_MSC_VER >= 1400)
        rc = sprintf_s(buf, 16, "%d:%02d", min, sec);
#else
        rc = sprintf(buf, "%d:%02d", min, sec);
#endif
        return buf;
    }

};
