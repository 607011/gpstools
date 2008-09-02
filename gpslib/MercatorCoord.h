// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __MERCATORCOORD_H_
#define __MERCATORCOORD_H_

#include "Point.h"

namespace GPS {

    /// Die Klasse MercatorCoord speichert Mercator-Koordinaten.
    /// Mercator-Koordinaten sind winkel-, aber nicht fl�chentreue Abbildungen
    /// von geografischen Koordinaten auf x- und y-Koordinaten.
    /// Um die Erde wird ein Zylinder gelegt, der jene am �quator ber�hrt. Vom Projektionszentrum
    /// in der Erdmitte werden nun alle Punkte vom Globus auf den Zylinder �bertragen. Rollt man
    /// danach den Zylinder ab, so erh�lt man eine plane Karte der Erdoberfl�che.
    /// F�r die Winkeltreue wird die Karte in Nord-S�d-Richtung verzerrt; zu den Polen hin
    /// wird die Verzerrung immer st�rker.
    /// Bei der Mercator-Projektion wird die Erde nicht wie in anderen Projektionen
    /// als Ellipsoid, sondern (ungenauer) als Kugel angenommen.
    /// @see http://de.wikipedia.org/wiki/Mercator-Projektion
    class MercatorCoord : public Point {
    public:
        /// Konstruktor.
        /// @param x x-Wert der Mercator-Koordinate
        /// @param y y-Wert der Mercator-Koordinate
        MercatorCoord(double x = 0.0, double y = 0.0) : Point(x, y)
        { /* ... */ }
    };


};

#endif // __MERCATORCOORD_H_
