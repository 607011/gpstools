// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "Polygon.h"
#include "Vector.h"

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
            for (PointList::const_iterator i = points.begin(); i != points.end()-1; ++i)
                sum += Vector(*i, (*(i+1))).length();
        }
        return sum;
    }


    double Polygon::area(void) const
    {
        double sum = 0.0;
        if (points.size() > 3) {
            for (PointList::const_iterator i = points.begin(); i != points.end()-1; ++i)
                sum += ((*i).x() - ((*i)+1).x()) * ((*i).y() + ((*i)+1).y());
        }
        return -0.5 * sum;
    }


    void Polygon::close(void) 
    {
        append(points.front());
    }
};
