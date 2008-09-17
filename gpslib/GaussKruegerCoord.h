// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __GAUSSKRUEGERCOORD_H_
#define __GAUSSKRUEGERCOORD_H_

#include "Ellipsoid.h"
#include "Point.h"
#include "GeoCoord.h"

namespace GPS {

    /// Die Klasse GaussKruegerCoord speichert Gauß-Krüger-Koordinaten.
    class GaussKruegerCoord : public Point {
    private:
        Ellipsoid ell;

    public:
        /// Konstruktor.
        /// @param easting Rechtswert der Gauß-Krüger-Koordinate
        /// @param northing Hochwert der Gauß-Krüger-Koordinate
        GaussKruegerCoord(double easting = 0.0, double northing = 0.0);

        /// Umwandeln der GK-Koordinaten in geografische Koordinaten (Längen- und Breitengrad)
        /// @return geografische Koordinaten
        GeoCoord toGeoCoord(void);
    };


};

#endif // __GAUSSKRUEGERCOORD_H_
