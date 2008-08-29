// $Id: TrackLine.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __TRACKLINE_H_
#define __TRACKLINE_H_

#include "Trackpoint.h"

namespace GPS {

    /// Die Klasse TrackLine repräsentiert eine Verbindungslinie zwischen zwei Wegpunkten
    class TrackLine {

    private:
        Trackpoint* _P1;
        Trackpoint* _P2;

    public:
        /// Konstruktor.
        TrackLine(void) : _P1(0), _P2(0) { /* ... */ }

        /// Konstruktor.
        TrackLine(Trackpoint* p1, Trackpoint* p2);

        Trackpoint* interpolated(void);

        inline void reset(void) { _P1 = 0; _P2 = 0; }

        /// Ersten Punkt zurückgeben.
        /// @return Erster Punkt
        inline const Trackpoint* p1(void) const { return _P1; }

        /// Zweiten Punkt zurückgeben.
        /// @return Zweiter Punkt
        inline const Trackpoint* p2(void) const { return _P2; }

        /// Ersten Punkt setzen.
        /// @param p1 Erster Punkt.
        inline void setP1(Trackpoint* p1) { _P1 = p1; }

        /// Zweiten Punkt setzen.
        /// @param p1 Zweiter Punkt.
        inline void setP2(Trackpoint* p2) { _P2 = p2; }
    };


};

#endif // __TRACKLINE_H_
