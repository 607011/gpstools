// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
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

    /// Die Klasse GeoCoord speichert geographische Koordinaten (L�ngen- und Breitengrad)
    /// sowie gegebenenfalls die H�he �NN an diesem Punkt.
    /// Als Bezugssystem voreingestellt ist der WGS84-Ellipsoid.
    /// @see switchEllipsoid()
    class GeoCoord : public Point {
    private:
        Ellipsoid ell;

        /// Cache f�r die Berechnung der UTM-Koordinaten aus den geografischen Koordinaten.
        Value<UTMCoord> UTMCoordCache;

    protected:
        /// L�ngengrad.
        double _Longitude;

        /// Breitengrad.
        double _Latitude;

        /// H�he in Metern.
        DoubleValue _Elevation;

    public:
        /// Platzhalter f�r L�ngen- und Breitengrad.
        /// Wird f�r die Funktion dec2dms() ben�tigt.
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

        /// Breitengrad zur�ckgeben.
        /// @return Breitengrad
        /// @see _Latitude
        inline double latitude(void) const { return _Latitude; }

        /// L�ngengrad zur�ckgeben.
        /// @return L�ngengrad
        /// @see _Longitude
        inline double longitude(void) const { return _Longitude; }

        /// H�he in Metern �NN zur�ckgeben
        /// @return H�he in Metern
        inline DoubleValue elevation(void) const { return _Elevation; }

        /// Breitengrad setzen.
        /// @param lat Breitengrad
        inline void setLatitude(double lat)
        {
            _Latitude = lat;
            UTMCoordCache.undef();
        }

        /// L�ngengrad setzen.
        /// @param lon L�ngengrad
        inline void setLongitude(double lon)
        {
            _Longitude = lon;
            UTMCoordCache.undef();
        }

        /// H�he in Metern setzen
        /// @param ele H�he in Metern
        inline void setElevation(DoubleValue ele) { _Elevation = ele; }

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
        /// Der String hat das Format (E|W) dd�dd'dd.dddd" (N|S) dd�dd'dd.dddd".
        inline operator std::string() { return toString(); }

        /// Die geografischen Koordinaten in einen String wandeln.
        /// Der String hat das Format (E|W) dd�dd'dd.dddd" (N|S) dd�dd'dd.dddd".
        std::string toString(void) const;

        /// L�ngen- oder Breitengrad in einen String wandeln.
        /// Der String hat das Format (N|S|E|W) dd�dd'dd.dddd", abh�ngig davon,
        /// ob ein L�ngen- oder Breitengrad umgewandelt werden soll.
        /// @param dec Geografische Koordinate
        /// @param ref Angabe, ob L�ngen- oder Breitengrad
        static std::string dec2dms(double dec, enum LonLat ref);

        /// Aus einem Breitengrad die UTM-Zone ermitteln.
        /// @param lat Breitengrad
        /// @return UTM-Zone
        static char UTMLetterDesignator(double lat);
    };

};

#endif // __GEOCOORD_H_
