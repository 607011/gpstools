// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __GEOCOORD_H_
#define __GEOCOORD_H_

#include <string>
#include <cmath>

#include "Value.h"
#include "Point.h"
#include "UTMCoord.h"
#include "MercatorCoord.h"
#include "Ellipsoid.h"

namespace GPS {

    /// Die Klasse GeoCoord speichert geographische Koordinaten (Längen- und Breitengrad)
    /// sowie gegebenenfalls die Höhe üNN an diesem Punkt.
    /// Als Bezugssystem voreingestellt ist der WGS84-Ellipsoid.
    /// @see switchEllipsoid()
    class GeoCoord : public Point {
    private:
        Ellipsoid ell;

        /// Cache für die Berechnung der UTM-Koordinaten aus den geografischen Koordinaten.
        Value<UTMCoord> UTMCoordCache;

    protected:
        /// Längengrad.
        double _Longitude;

        /// Breitengrad.
        double _Latitude;

        /// Höhe in Metern.
        DoubleValue _Elevation;

    public:
        /// Platzhalter für Längen- und Breitengrad.
        /// Wird für die Funktion dec2dms() benötigt.
        enum LonLat
        {
            LONGITUDE,
            LATITUDE
        };

    public:
        /// Konstruktor.
        GeoCoord(double lon = 0.0, double lat = 0.0, DoubleValue ele = DoubleValue());

        /// Entfernung in Metern zu einer anderen geografischen Koordinate berechnen.
        /// @return Entfernung in Metern
        double rangeTo(const GeoCoord*) const;

        /// Entfernung in Metern zu einer anderen geografischen Koordinate berechnen.
        /// @return Entfernung in Metern
        double rangeTo(const GeoCoord&) const;

        /// Richtung in Grad zu einer anderen geografischen Koordinate berechnen.
        /// @return Richtung in Grad
        double bearing(const GeoCoord*) const;

        /// Richtung in Grad zu einer anderen geografischen Koordinate berechnen.
        /// @return Richtung in Grad
        double bearing(const GeoCoord&) const;

        /// Breitengrad zurückgeben.
        /// @return Breitengrad
        /// @see _Latitude
        inline double latitude(void) const { return _Latitude; }

        /// Längengrad zurückgeben.
        /// @return Längengrad
        /// @see _Longitude
        inline double longitude(void) const { return _Longitude; }

        /// Höhe in Metern üNN zurückgeben
        /// @return Höhe in Metern
        inline DoubleValue elevation(void) const { return _Elevation; }

        /// Breitengrad setzen.
        /// @param lat Breitengrad
        inline void setLatitude(double lat)
        {
            _Latitude = lat;
            UTMCoordCache.undef();
        }

        /// Längengrad setzen.
        /// @param lon Längengrad
        inline void setLongitude(double lon)
        {
            _Longitude = lon;
            UTMCoordCache.undef();
        }

        /// Höhe in Metern setzen
        /// @param ele Höhe in Metern
        inline void setElevation(double ele) { _Elevation = ele; }

        /// Geografische Koordinaten in UTM-Koordinaten wandeln.
        /// @return UTM-Koordinaten
        UTMCoord toUTM(void);

        /// Geografische Koordinaten in UTM-Koordinaten wandeln.
        /// @return UTM-Koordinaten
        inline operator UTMCoord() { return toUTM(); }

        /// Geografische Koordinaten in Mercator-Koordinaten wandeln.
        /// @return Mercator-Koordinaten
        MercatorCoord toMercator(void) const;

        /// Geografische Koordinaten in Mercator-Koordinaten wandeln.
        /// @return Mercator-Koordinaten
        inline operator MercatorCoord() { return toMercator(); }

        /// Die geografischen Koordinaten in einen String wandeln.
        /// Der String hat das Format (E|W) dd°dd'dd.dddd" (N|S) dd°dd'dd.dddd".
        inline operator std::string() { return toString(); }

        /// Die geografischen Koordinaten in einen String wandeln.
        /// Der String hat das Format (E|W) dd°dd'dd.dddd" (N|S) dd°dd'dd.dddd".
        std::string toString(void) const;

        /// Längen- oder Breitengrad in einen String wandeln.
        /// Der String hat das Format (N|S|E|W) dd°dd'dd.dddd", abhängig davon,
        /// ob ein Längen- oder Breitengrad umgewandelt werden soll.
        /// @param dec Geografische Koordinate
        /// @param ref Angabe, ob Längen- oder Breitengrad
        static std::string dec2dms(double dec, enum LonLat ref);

        /// Aus einem Breitengrad die UTM-Zone ermitteln.
        /// @param lat Breitengrad
        /// @return UTM-Zone
        static char UTMLetterDesignator(double lat);
    };

};

#endif // __GEOCOORD_H_
