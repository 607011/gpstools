// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "Polygon.h"
#include "Vector.h"

#ifdef _DEBUG
#include <iostream>
#endif

#include <cmath>

namespace GPS {

    Polygon::Polygon(void)
    {
        // ...
    }


    Polygon::~Polygon(void)
    {
        // ...
    }


    double Polygon::perimeter(void) const
    {
        double sum = 0.0;
        if (points.size() > 3) {
            for (PointList::const_iterator i = points.begin(); i != points.end()-1; ++i) {
                const Point& p1 = *i;
                const Point& p2 = *(i+1);
                sum += Vector(p1, p2).length();
            }
        }
        return sum;
    }


    double Polygon::area(void) const
    {
        double sum = 0.0;
        if (points.size() > 3) {
            for (PointList::const_iterator i = points.begin(); i != points.end()-1; ++i)
            {
                const Point& p1 = *i;
                const Point& p2 = *(i+1);
                sum += (p1.x() - p2.x()) * (p1.y() + p2.y());
            }
        }
        return fabs(0.5 * sum);
    }


    void Polygon::close(void) 
    {
        if (points.front() != points.back())
            append(points.front());
    }
};
