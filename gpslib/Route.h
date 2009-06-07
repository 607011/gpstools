// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#ifndef __ROUTE_H_
#define __ROUTE_H_

#include <limits>
#include <string>
#include <vector>
#include <ostream>
#include <cassert>
#include <cmath>

#include "AbstractWaypointList.h"
#include "Waypoint.h"

namespace GPS {

    typedef std::vector<Waypoint*> WaypointList;

    /// Eine Route, das heißt eine gerichtete Wegpunktliste gemäß http://www.topografix.com/GPX/1/1
    class Route : public AbstractWaypointList<Waypoint> {
    public:
        /// Konstruktor.
        Route(void) { }

        /// Copy-Konstruktor.
        /// @param other Route, deren sämtliche Daten inklusive Kopien ihrer Wegpunkte in das
        /// neue Objekt übertragen werden sollen.
        Route(const Route& other);

        /// Entfernung in Metern zurückgeben.
        /// Achtung: Die Entfernung wird bei jedem Aufruf über die Bildung
        /// einer Summe über die Entfernung zwischen den einzelnen Wegpunktem neu ermittelt.
        /// Die Kosten für den Aufruf dieser Methode steigen proportional mit der
        /// Anzahl der in der Route enthaltenen Wegpunkte.
        /// @return Entfernung in Metern
        double distance(void) const;

    };


};
#endif // __ROUTE_H_
