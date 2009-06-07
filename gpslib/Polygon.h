// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#ifndef __POLYGON_H_
#define __POLYGON_H_

#include <vector>

#include "Point.h"
#include "Line.h"

namespace GPS {

    typedef std::vector<Point> PointList;

    /// Die Klasse Polygon beschreibt 2-dimensionale Polygone in einem kartesischen Koordinatensystem.
    class Polygon
    {
    private:
        /// Liste der Punkt für Punkt aneinanderhängenden Kanten, aus denen sich der Polygon zusammensetzt.
        PointList points;

    public:
        /// Konstruktor.
        Polygon(void);

        /// Destruktor.
        ~Polygon(void);

        /// Umfang des Polygons ermitteln.
        /// @return Umfang des Polygons
        double perimeter(void) const;

        /// Flächeninhalt des Polygons ermitteln.
        /// @return Flächeninhalt des Polygons
        double area(void) const;

        /// Einen Punkt ans Ende der Punktliste anhängen.
        /// @param line neuer Punkt
        inline void append(const Point& p) { points.push_back(p); }

        inline Polygon& operator<<(const Point& p) { points.push_back(p); return *this; }

        /// Polygon schließen.
        /// Diese Funktion *muss* aufgerufen werden, bevor man perimeter() oder area() aufruft.
        void close(void);
    };


};


#endif // __POLYGON_H_
