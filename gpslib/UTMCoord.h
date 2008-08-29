// $Id: UTMCoord.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __UTMCOORD_H_
#define __UTMCOORD_H_

#include "Point.h"

namespace GPS {

    /// Die Klasse UTMCoord speichert Koordinaten aus dem UTM-Koordinatensystem.
    /// @see http://de.wikipedia.org/wiki/UTM-Koordinatensystem
    class UTMCoord : public Point {
    protected:
        /// UTM-Zone (A..Y)
        char _Zone;

        /// UTM-Zonennummer
        int _ZoneNumber;

    public:
        /// Kennzeichnung für eine ungültige UTM-Zone
        static const char INVALID_ZONE = 'Z';

    public:
        /// Konstruktor.
        UTMCoord(void) : _Zone(INVALID_ZONE)
        { /* ... */ }

        /// Konstruktor.
        /// @param easting Rechtswert
        /// @param northing Hochwert
        /// @param zone UTM-Zone
        /// @param zoneNumber UTM-Zonennummer
        UTMCoord(double easting, double northing, char zone, int zoneNumber)
            : Point(easting, northing), _Zone(zone), _ZoneNumber(zoneNumber)
        { /* ... */ }

        /// Rechtswert zurückgeben.
        /// @return Rechtswert
        inline double easting(void) const { return _X; }

        /// Hochwert zurückgeben.
        /// @return Hochwert
        inline double northing(void) const { return _Y; }

        /// UTM-Zone zurückgeben
        /// @return UTM-Zone
        inline char zone(void) const { return _Zone; }

        /// UTM-Zonennummer zurückgeben
        /// @return UTM-Zonennummer
        inline int zoneNumber(void) const { return _ZoneNumber; }
    };

};

#endif // __UTMCOORD_H_
