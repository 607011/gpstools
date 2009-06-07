// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include "Vector.h"
#include "Point.h"

#include <cmath>


namespace GPS {

    Vector::Vector(const Point& p1, const Point& p2)
    {
        _U = p2.x() - p1.x();
        _V = p2.y() - p1.y();
        _W = p2.z() - p1.z();
    }


    double operator*(const Vector& v1, const Vector& v2)
    {
        return v1.u() * v2.u() + v1.v() * v2.v() + v1.w() * v2.w();
    }


    Vector operator-(const Vector& v1, const Vector& v2)
    {
        return Vector(v1.u() - v2.u(), v1.v() - v2.v(), v1.w() - v2.w());
    }


    Vector operator+(const Vector& v1, const Vector& v2)
    {
        return Vector(v1.u() + v2.u(), v1.v() + v2.v(), v1.w() + v2.w());
    }


    Vector operator/(const Vector& vec, double s)
    {
        if (s == 0)
            return Vector();
        return Vector(vec.u()/s, vec.v()/s, vec.w()/s);
    }


    Vector operator*(const Vector& vec, double s)
    {
        return Vector(vec.u()*s, vec.v()*s, vec.w()*s);
    }


    Vector operator*(double s, const Vector& vec)
    {
        return Vector(vec.u()*s, vec.v()*s, vec.w()*s);
    }


    double Vector::length(void) const
    {
        return sqrt(_U*_U + _V*_V + _W*_W);
    }


    Vector Vector::normalized(void) const
    {
        const double l = length();
        if (l == 0)
            return Vector();
        return *this / l;
    }

};
