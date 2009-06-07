// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
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
