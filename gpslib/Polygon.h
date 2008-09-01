// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __POLYGON_H_
#define __POLYGON_H_

#include <vector>

#include "Point.h"
#include "Line.h"

namespace GPS {

    typedef std::vector<Line> LineList;

    /// Die Klasse Polygon beschreibt 2-dimensionale Polygone in einem kartesischen Koordinatensystem.
    class Polygon
    {
    private:
        /// Liste der Punkt für Punkt aneinanderhängenden Kanten, aus denen sich der Polygon zusammensetzt.
        LineList lines;

    private:
        static double triangleArea(const Point& p1, const Point& p2, const Point& p3);

    public:
        /// Konstruktor.
        Polygon(void);

        /// Destruktor.
        ~Polygon(void);

        /// Umfang des Polygons ermitteln.
        /// @return Umfang des Polygons
        double perimeter(void) const;

        /// Eine Kante ans Ende der Kantenliste anhängen.
        /// @param line neue Kante
        inline void append(const Line& line) { lines.push_back(line); }
    };


};


#endif // __POLYGON_H_
