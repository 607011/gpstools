// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.


#include "Route.h"
#include "Polygon.h"

namespace GPS {

    Route::Route(const Route& other)
    { 
        copyMembers(other);
        // do a deeeeep copy of waypoints
        for (WaypointList::const_iterator i = other.samples.begin(); i != other.samples.end(); ++i)
            samples.push_back(new Waypoint(*(*i)));
    }


    double Route::distance(void) const
    {
        if (samples.size() < 2)
            return -1;
        Waypoint* prev = samples.front();
        if (prev == NULL)
            return - 1;
        double dist = 0.0;
        for (WaypointList::const_iterator i = samples.begin(); i != samples.end(); ++i)
        {
            Waypoint* curr = (*i);
            if (curr != NULL)
            {
                dist += curr->rangeTo(prev);
                prev = curr;
            }
        }
        return dist;
    }

};
