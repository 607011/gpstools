// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include <iomanip>
#include <cmath>
#include <cassert>

#include "Trackpoint.h"
#include "Point.h"
#include "Vector.h"

using namespace std;


namespace GPS {

    Trackpoint::Trackpoint(void)
        : Waypoint(),
        _Distance(0.0),
        _Slope(0.0),
        _Heartrate(),
        _Temperature(),
        _Duration(0),
        _Keep(true)
    { /* ... */ }

    Trackpoint::Trackpoint(double lon, double lat, DoubleValue ele, Timestamp timestamp)
        : Waypoint(lon, lat, ele, timestamp),
        _Distance(0.0),
        _Slope(0.0),
        _Heartrate(),
        _Temperature(),
        _Duration(0),
        _Keep(true)
    { /* ... */ }


    Trackpoint::Trackpoint(const Trackpoint& p)
        : Waypoint(p.longitude(), p.latitude(), p.elevation(), p.timestamp()),
        _Distance(p.distance()),
        _Slope(p.slope()),
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


    // TODO: Trackpoint::perpendicularDistance2D() und Trackpoint::perpendicularDistance3D() dahingehend erweitern, dass alle in die Berechnung einbezogenen Punkte nicht mehr in der gleichen UTM-Zone liegen müssen.
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


};
