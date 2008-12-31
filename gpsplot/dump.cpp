// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <fstream>

#include "globals.h"

void dumpData(void)
{
    std::fstream dump;
    if (dumpFile != "")
        dump.open(dumpFile.c_str(), std::fstream::out);

    if (dump.good())
    {
        bool circular = trk->points().front()->rangeTo(trk->points().back()) < MAX_RANGE_BETWEEN_START_AND_FINISH;
        dump << "Startzeit:        " << trk->startTimestamp().toString() << std::endl
             << "Zielzeit:         " << trk->finishTimestamp().toString() << std::endl
             << "Dauer:            " << (trk->finishTimestamp() - trk->startTimestamp()).toString() << std::endl
             << std::setprecision(5) << std::noshowpoint
             << "Entfernung:       " << 1e-3*trk->distance() << "km" << std::endl
             << "Höhe min/max:     " << trk->minElevation().value() << "m / " << trk->maxElevation().value() << "m" << std::endl
             << std::setprecision(3)
             << "mittl. Geschw.:   " << trk->avgSpeed() << " km/h" << std::endl;
        if (trk->avgHeartrate().defined())
            dump << "mittl. Herzfreq.: " << (int) trk->avgHeartrate().value() << " /min" << std::endl;
        if (trk->avgTemperature().defined())
            dump << "mittl. Temp.:     " << trk->avgTemperature().value() << "° C" << std::endl;
        dump << "An-/Abstiege ";
        if (circular)
            dump << "(mittl. Anstieg in %)";
        else
            dump << "(mittl. An-/Abstieg in %)";
        dump << std::endl;
        for (smoothedTrack_t::const_iterator i = smoothedTrack.begin(); i != smoothedTrack.end(); ++i) {
            GPS::Track* t = (*i).second;
            dump << "  " << std::setw(25) << std::left << std::setfill(' ')
                 << (*i).first << ": "
                 << std::setprecision(4)
                 << std::setw(5) << std::right
                 << t->ascent() << "m / "
                 << std::setw(5) << std::right
                 << t->descent() << "m ("
                 << std::setprecision(3)
                 << std::setw(4) << std::right;
            if (circular)
                dump << (t->avgUphillSlope() + t->avgDownhillSlope()).value();
            else
                dump << t->avgUphillSlope() << "% / " << t->avgDownhillSlope();
            dump << "%)" << std::endl;
        }
        dump.close();
    }
}
