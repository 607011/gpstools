// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include <cmath>


#include "Line.h"
#include "Point.h"
#include "Vector.h"
#include "helper.h"

namespace GPS {

    std::string Point::toString(void) const
    {
        return tos(_X) + " / " + tos(_Y);
    }


    double Point::perpendicularDistanceToSegment(const Point& p1, const Point& p2) const
    {
        Vector vec(p1, p2);
        Vector ref(*this - p1);
        double hypotenuse = ref.length();
        double A = ref * vec;
        if (A < 0)
            return ref.length();
        double B = vec * vec;
        if (B < A)
            return (*this - p2).length();
        A /= vec.length();
        double d = sqrt(hypotenuse * hypotenuse - A * A);
        return d;
    }


    double Point::perpendicularDistanceToSegment(const Line& line) const
    {
        return perpendicularDistanceToSegment(line.p1(), line.p2());
    }


    double Point::perpendicularDistanceToLine(const Point& p1, const Point& p2) const
    {
        Vector vec(p1, p2);
        Vector ref(*this - p1);
        double hypotenuse = ref.length();
        Vector unified = vec.normalized();
        double A = ref * unified;
        double d = sqrt(hypotenuse * hypotenuse - A * A);
        return d;
    }


    double Point::perpendicularDistanceToLine(const Line& line) const
    {
        return perpendicularDistanceToLine(line.p1(), line.p2());
    }


    Point& Point::operator-=(const Vector& v)
    {
        _X -= v.u();
        _Y -= v.v();
        _Z -= v.w();
        return *this;
    }

    
    Point& Point::operator+=(const Vector& v)
    {
        _X += v.u();
        _Y += v.v();
        _Z += v.w();
        return *this;
    }


    Point& Point::operator*=(double c)
    {
        _X *= c;
        _Y *= c;
        _Z *= c;
        return *this;
    }


    Point& Point::operator/=(double c)
    {
        _X /= c;
        _Y /= c;
        _Z /= c;
        return *this;
    }


    Vector operator-(const Point& p1, const Point& p2)
    {
        return Vector(p1.x() - p2.x(), p1.y() - p2.y(), p1.z() - p2.z());
    }


    Point operator+(const Point& p, const Vector& v)
    {
        return Point(p.x() + v.u(), p.y() + v.v(), p.z() + v.w());
    }


    Point operator-(const Point& p, const Vector& v)
    {
        return Point(p.x() - v.u(), p.y() - v.v(), p.z() - v.w());
    }


    Point operator*(const Point& p, double c)
    {
        return Point(p.x() * c, p.y() * c, p.z() * c);
    }


    Point operator*(double c, const Point& p)
    {
        return Point(p.x() * c, p.y() * c, p.z() * c);
    }


    Point operator/(const Point& p, double c)
    {
        return Point(p.x() / c, p.y() / c, p.z() / c);
    }


    Point operator+(const Point& p1, const Point& p2)
    {
        return Point(p1.x() + p2.x(), p1.y() + p2.y(), p1.z() + p2.z());
    }


    bool operator==(const Point& p1, const Point& p2)
    {
        return p1.x() == p2.x() && p1.y() == p2.y();
    }


    bool operator!=(const Point& p1, const Point& p2)
    {
        return !(p1 == p2);
    }

};
