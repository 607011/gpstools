// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "Ellipsoid.h"

namespace GPS {

    std::map<std::string, Ellipsoid> ellipsoid;

    /// Mittels dieser Hilfsklasse wird die Tabelle ellipsoid gefüllt.
    /// @see ellipsoid
    struct LoadMap {
        LoadMap()
        {
            ellipsoid["WGS84"]              = Ellipsoid(6378137.0,   6356752.315 );
            ellipsoid["GRS80"]              = Ellipsoid(6378137.0,   6356752.3141);
            ellipsoid["HAYFORD"]            = Ellipsoid(6378388.0,   6356911.946 );
            ellipsoid["KRASSOVSKY-1938"]    = Ellipsoid(6378245.0,   6356755.4   );
            ellipsoid["BESSEL-1841"]        = Ellipsoid(6377397.155, 6356078.965 );
            ellipsoid["AIRY"]               = Ellipsoid(6377563.396, 6356256.91  );
            ellipsoid["AIRY-MODIFIED"]      = Ellipsoid(6377340.189, 6356034.447 );
            ellipsoid["NAD27"]              = Ellipsoid(6378206.4,   6356583.8   );
            ellipsoid["SOVIET-1985"]        = Ellipsoid(6378136.0,   6356751.3016);
            ellipsoid["WGS72"]              = Ellipsoid(6378135.0,   6356750.52  );     
            ellipsoid["EVEREST-1830"]       = Ellipsoid(6377276.345, 6356075.4131);
            ellipsoid["EVEREST-MODIFIED"]   = Ellipsoid(6377304.063, 6356103.039 );
            ellipsoid["AUSTRALIAN"]         = Ellipsoid(6378160.0,   6356774.7192);
            ellipsoid["CLARKE-1880"]        = Ellipsoid(6378249.145, 6356514.8646);
            ellipsoid["FISHER-1960"]        = Ellipsoid(6378166.0,   6356784.2836);
            ellipsoid["FISHER-1968"]        = Ellipsoid(6378150.0,   6356768.3372);
            ellipsoid["HOUGH-1956"]         = Ellipsoid(6378270.0,   6356794.34  );
            ellipsoid["IAU76"]              = Ellipsoid(6378140.0,   6356755.288 );
            ellipsoid["NWL-9D"]             = Ellipsoid(6378145.0,   6356759.7695);
            ellipsoid["SOUTHAMERICAN-1969"] = Ellipsoid(6378160.0,   6356774.7192);
        }
    };

    /// Objekt zur Initialisierung von ellipsoid.
    /// @see ellipsoid
    /// @see Ellipsoid
    static const LoadMap initializer;


    Ellipsoid::Ellipsoid(double a, double b)
    { 
        assert(a > 0);
        assert(b > 0);
        _Radius = a;
        _Radius2 = b;
        _Eccentricity = (a - b) / a ;
        _EccentricitySquared = (a * a - b * b) / (a * a);
        _EccentricitySquared2 = (a * a - b * b) / (b * b);
    }


    Ellipsoid::Ellipsoid(const Ellipsoid& e)
    {
        _Radius = e.a();
        _Radius2 = e.b();
        _Eccentricity = e.eccentricity();
        _EccentricitySquared = e.eccentricitySquared();
        _EccentricitySquared2 = e.eccentricitySquared2();
    }


    Ellipsoid::Ellipsoid(const std::string& name)
    {
        choose(name);
    }


    void Ellipsoid::choose(const std::string& name)
    {
        *this = ellipsoid[name];
        assert(_Radius > 0);
        assert(_Radius2 > 0);
        assert(_Eccentricity > 0);
        assert(_EccentricitySquared > 0);
        assert(_EccentricitySquared2 > 0);
    }
};
