// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <map>
#include <string>

#include "Point.h"
#include "Ellipsoid.h"
#include "GaussKruegerCoord.h"

namespace GPS {

    GaussKruegerCoord::GaussKruegerCoord(double easting, double northing)
        : Point(easting, northing)
    {
        ell.choose("WGS84");
    }


    /// Grad in Radians wandeln.
    /// @param _Deg Gradangabe
    /// @return Radians
    static inline double deg2rad(double _Deg)
    {
        return _Deg * PI / 180.0;
    }

};
