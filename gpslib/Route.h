// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
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
#include "Vector.h"

namespace GPS {

    typedef std::vector<Waypoint*> WaypointList;

    class Route : public AbstractWaypointList {
    private:
        /// Sortierte Liste der Wegpunkte.
        WaypointList samples;

    public:
        /// Konstruktor.
        Route(void);

        /// Copy-Konstruktor.
        /// @param other Route, deren s�mtliche Daten inklusive Kopien ihrer Wegpunkte in das
        /// neue Objekt �bertragen werden sollen.
        Route(const Route& other);

        /// Wegpunkt-Liste zur�ckgeben.
        /// @return Wegpunkt-Liste
        /// @see samples
        inline WaypointList& points(void) { return samples; }

        /// Routen-Daten l�schen.
        inline void clear(void)
        {
            samples.clear();
        }

        /// Ermitteln, ob die Wegpunkt-Liste Wegpunkte enth�lt.
        /// @return true, wenn die Wegpunkt-Liste leer ist.
        inline bool isEmpty(void) const
        {
            return samples.size() == 0;
        }

        /// Einen Wegpunkt aus der Route entfernen.
        /// @param trkpt Zu entfernender Trackpunkt.
        inline void erase(WaypointList::iterator wpt)
        {
            samples.erase(wpt);
        }

        /// Einen Wegpunkt an die Route anh�ngen.
        /// @param wpt Anzuh�ngender Wegpunkt. NULL-Zeiger werden ignoriert.
        inline void append(Waypoint* wpt)
        {
            if (wpt != NULL)
                samples.push_back(wpt);
        }

        /// Letzten Punkt aus der Route entfernen.
        void pop(void);


        /// Entfernung in Metern zur�ckgeben.
        /// Achtung: Die Entfernung wird bei jedem Aufruf �ber die Bildung
        /// einer Summe �ber die Entfernung zwischen den einzelnen Wegpunktem neu ermittelt.
        /// Die Kosten f�r den Aufruf dieser Methode steigen proportional mit der
        /// Anzahl der in der Route enthaltenen Wegpunkte.
        /// @return Entfernung in Metern
        double distance(void) const;

        /// Durch die Punkte eingeschlossene Fl�che in Quadratmetern zur�ckgeben.
        /// @return Fl�che in Quadratmetern
        double area(void) const;

    };


};
#endif // __ROUTE_H_
