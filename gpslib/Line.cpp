// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "Line.h"

namespace GPS {

    Line::Line(const Point &p1, const Point &p2)
        : _P1(p1), _P2(p2)
    { /* ... */ }

    Line::Line(double x1, double y1, double x2, double y2)
        : _P1(x1, y1), _P2(x2, y2)
    { /* ... */ }

    Line::Line(double x1, double y1, double z1, double x2, double y2, double z2)
        : _P1(x1, y1, z1), _P2(x2, y2, z2)
    { /* ... */ }

};
