// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "helper.h"
#include "TrackLine.h"

#include <cmath>

namespace GPS {

    TrackLine::TrackLine(Trackpoint* p1, Trackpoint* p2)
        : _P1(p1), _P2(p2)
    { /* ... */ }

    Trackpoint* TrackLine::interpolated(void) const
    {
        Trackpoint* t = NULL;
        if (_P1 != NULL && _P2 != NULL)
        {
            t = new Trackpoint(fabs(_P1->longitude() - _P2->longitude()) / 2 + GPS::min<double>(_P1->longitude(), _P2->longitude()), fabs(_P1->latitude() - _P2->latitude()) / 2 + GPS::min<double>(_P1->latitude(), _P2->latitude()));
            t->setTimestamp(abs((long)(_P1->timestamp().milliseconds() - _P2->timestamp().milliseconds())) / 2 + GPS::min<Timestamp>(_P1->timestamp(), _P2->timestamp()));
            t->setDistance(fabs((_P1->distance() - _P2->distance())) / 2 + GPS::min<double>(_P1->distance(), _P2->distance()));
            if (_P1->elevation().defined() && _P2->elevation().defined())
                t->setElevation(fabs(_P1->elevation().value() - _P2->elevation().value()) / 2 + GPS::min<double>(_P1->elevation().value(), _P2->elevation().value()));
        }
        return t;
    }

};
