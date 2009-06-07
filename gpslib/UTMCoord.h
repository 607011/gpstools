// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
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
        /// Kennzeichnung f�r eine ung�ltige UTM-Zone
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

        /// Rechtswert zur�ckgeben.
        /// @return Rechtswert
        inline double easting(void) const { return _X; }

        /// Hochwert zur�ckgeben.
        /// @return Hochwert
        inline double northing(void) const { return _Y; }

        /// UTM-Zone zur�ckgeben
        /// @return UTM-Zone
        inline char zone(void) const { return _Zone; }

        /// UTM-Zonennummer zur�ckgeben
        /// @return UTM-Zonennummer
        inline int zoneNumber(void) const { return _ZoneNumber; }
    };

};

#endif // __UTMCOORD_H_
