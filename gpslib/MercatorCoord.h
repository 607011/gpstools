// $Id: MercatorCoord.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __MERCATORCOORD_H_
#define __MERCATORCOORD_H_

#include "Point.h"

namespace GPS {

    /// Die Klasse MercatorCoord speichert Mercator-Koordinaten.
    /// Mercator-Koordinaten sind winkel-, aber nicht flächentreue Abbildungen
    /// von geografischen Koordinaten auf x- und y-Koordinaten.
    /// Um die Erde wird ein Zylinder gelegt, der jene am Äquator berührt. Vom Projektionszentrum
    /// in der Erdmitte werden nun alle Punkte vom Globus auf den Zylinder übertragen. Rollt man
    /// danach den Zylinder ab, so erhält man eine plane Karte der Erdoberfläche.
    /// Für die Winkeltreue wird die Karte in Nord-Süd-Richtung verzerrt; zu den Polen hin
    /// wird die Verzerrung immer stärker.
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
