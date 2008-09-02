// $Id: Line.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __LINE_H_
#define __LINE_H_

#include "Point.h"

namespace GPS {

    /// Die Klasse Line repräsentiert eine Verbindungslinie zwischen zwei Punkten
    /// in einem zwei- oder dreidimensionalen Koordinatensystem.
    class Line {
    private:
        Point _P1;
        Point _P2;
    public:
        /// Konstruktor.
        Line(void) { /* ... */ }

        /// Konstruktor.
        /// @param p1 Erster Punkt.
        /// @param p2 Zweiter Punkt.
        Line(const Point& p1, const Point& p2);

        /// Konstruktor.
        Line(double x1, double y1, double x2, double y2);

        /// Konstruktor.
        Line(double x1, double y1, double z1, double x2, double y2, double z2);

        /// Ersten Punkt zurückgeben.
        /// @return Erster Punkt
        inline const Point& p1(void) const { return _P1; }

        /// Zweiten Punkt zurückgeben.
        /// @return Zweiter Punkt
        inline const Point& p2(void) const { return _P2; }

        /// Ersten Punkt setzen.
        /// @param p1 Erster Punkt.
        inline void setP1(const Point& p1) { _P1 = p1; }

        /// Zweiten Punkt setzen.
        /// @param p1 Zweiter Punkt.
        inline void setP2(const Point& p2) { _P2 = p2; }
    };


};

#endif // __LINE_H_
