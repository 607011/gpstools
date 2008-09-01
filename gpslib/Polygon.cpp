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


    double Polygon::triangleArea(const Point& p1, const Point& p2, const Point& p3)
    {
        double s1 = Vector(p1, p2).length();
        double s2 = Vector(p2, p3).length();
        double s3 = Vector(p3, p1).length();
        double S = (s1 + s2 + s3) / 2;
        double area = sqrt(S * (S - s1) * (S - s2) * (S - s3));
        return area;
    }


    double Polygon::perimeter(void) const
    {
        double sum = 0.0;
        for (LineList::const_iterator i = lines.begin(); i != lines.end(); ++i)
            sum += Vector((*i).p1(), (*i).p2()).length();
        return sum;
    }

};
