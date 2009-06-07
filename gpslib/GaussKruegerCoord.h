// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#ifndef __GAUSSKRUEGERCOORD_H_
#define __GAUSSKRUEGERCOORD_H_

#include "Ellipsoid.h"
#include "Point.h"
#include "GeoCoord.h"

namespace GPS {

    /// Die Klasse GaussKruegerCoord speichert Gau�-Kr�ger-Koordinaten.
    class GaussKruegerCoord : public Point {
    private:
        Ellipsoid ell;

    public:
        /// Konstruktor.
        /// @param easting Rechtswert der Gau�-Kr�ger-Koordinate
        /// @param northing Hochwert der Gau�-Kr�ger-Koordinate
        GaussKruegerCoord(double easting = 0.0, double northing = 0.0);

        /// Umwandeln der GK-Koordinaten in geografische Koordinaten (L�ngen- und Breitengrad)
        /// @return geografische Koordinaten
        GeoCoord toGeoCoord(void);
    };


};

#endif // __GAUSSKRUEGERCOORD_H_
