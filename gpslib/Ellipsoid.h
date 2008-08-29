// $Id: Ellipsoid.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __ELLIPSOID_H_
#define __ELLIPSOID_H_

#include <cassert>

#include <string>
#include <map>

#include "helper.h"


namespace GPS {

    class Ellipsoid;

    extern std::map<std::string, Ellipsoid> ellipsoid;

    /// Erdumfang in Metern
    const double RHO = 6378137.0;

    ///
    const double PHI = RHO * PI / 180.0;

    ///
    const double PI360th = PI / 360.0;


    /// Die Klasse Ellipsoid beschreibt einen Ellipsoid �ber seine Radien.
    /// @see http://de.wikipedia.org/wiki/Erdellipsoid
    class Ellipsoid {
    private:
        /// Gro�e Achse in Metern
        double _Radius;

        /// Kleine Achse in Metern
        double _Radius2;

        /// Exzentrizit�t (Kehrwert der Abflachung): a / (a - b)
        double _Eccentricity;

        /// (a^2 - b^2) / a^2
        double _EccentricitySquared;

        /// (a^2 - b^2) / b^2
        double _EccentricitySquared2;

    public:
        /// Konstruktor.
        explicit Ellipsoid()
        { /* ... */ };

        /// Copy-Konstruktor.
        Ellipsoid(const Ellipsoid& o);

        /// Konstruktor.
        /// @param a Gro�e Achse in Metern
        /// @param b Kleine Achse in Metern
        Ellipsoid(double a, double b);

        /// Konstruktor.
        /// @param name Name des Ellipsoid
        Ellipsoid(const std::string& name);

        /// Einen Ellipsoid �ber seinen Namen ausw�hlen.
        /// M�gliche Angaben sind:
        /// - "AIRY"
        /// - "AIRY-MODIFIED"
        /// - "AUSTRALIAN"
        /// - "BESSEL-1841"
        /// - "CLARKE-1880"
        /// - "EVEREST-1830"
        /// - "EVEREST-MODIFIED"
        /// - "FISHER-1960"
        /// - "FISHER-1968"
        /// - "GRS80"
        /// - "HOUGH-1956"
        /// - "HAYFORD"
        /// - "IAU76"
        /// - "KRASSOVSKY-1938"
        /// - "NAD27"
        /// - "NWL-9D"
        /// - "SOUTHAMERICAN-1969"
        /// - "SOVIET-1985"
        /// - "WGS72"
        /// - "WGS84"
        void choose(const std::string& name);

        inline double radius(void) const { return _Radius; }
        inline double a(void) const { return _Radius; }
        inline double b(void) const { return _Radius2; }
        inline double eccentricity(void) const { return _Eccentricity; }
        inline double eccentricitySquared(void) const { return _EccentricitySquared; }
        inline double eccentricitySquared2(void) const { return _EccentricitySquared2; }
    };

};

#endif // __ELLIPSOID_H_
