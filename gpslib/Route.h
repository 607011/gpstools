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

    /// Eine Route, das hei�t eine gerichtete Wegpunktliste gem�� http://www.topografix.com/GPX/1/1
    class Route : public AbstractWaypointList<Waypoint> {
    public:
        /// Konstruktor.
        Route(void) { }

        /// Copy-Konstruktor.
        /// @param other Route, deren s�mtliche Daten inklusive Kopien ihrer Wegpunkte in das
        /// neue Objekt �bertragen werden sollen.
        Route(const Route& other);

        /// Entfernung in Metern zur�ckgeben.
        /// Achtung: Die Entfernung wird bei jedem Aufruf �ber die Bildung
        /// einer Summe �ber die Entfernung zwischen den einzelnen Wegpunktem neu ermittelt.
        /// Die Kosten f�r den Aufruf dieser Methode steigen proportional mit der
        /// Anzahl der in der Route enthaltenen Wegpunkte.
        /// @return Entfernung in Metern
        double distance(void) const;

    };


};
#endif // __ROUTE_H_
