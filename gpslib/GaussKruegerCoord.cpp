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

};
