// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <fstream>

#include "globals.h"

using namespace std;

void dumpData(void)
{
    fstream dump;
    if (dumpFile != "")
        dump.open(dumpFile.c_str(), fstream::out);

    if (dump.good())
    {
        bool circular = trk->points().front()->rangeTo(trk->points().back()) < MAX_RANGE_BETWEEN_START_AND_FINISH;
        dump << "Startzeit:        " << trk->startTimestamp().toString() << endl
             << "Zielzeit:         " << trk->finishTimestamp().toString() << endl
             << setprecision(5) << noshowpoint
             << "Entfernung:       " << 1e-3*trk->distance() << "km" << endl
             << "Höhe min/max:     " << trk->minElevation().value() << "m / " << trk->maxElevation().value() << "m" << endl
             << setprecision(3)
             << "mittl. Geschw.:   " << trk->avgSpeed() << " km/h" << endl;
        if (trk->avgHeartrate().defined())
            dump << "mittl. Herzfreq.: " << (int) trk->avgHeartrate().value() << " /min" << endl;
        if (trk->avgTemperature().defined())
            dump << "mittl. Temp.:     " << trk->avgTemperature().value() << " °C" << endl;
        dump << "An-/Abstiege ";
        if (circular)
            dump << "(mittl. Anstieg in %)";
        else
            dump << "(mittl. An-/Abstieg in %)";
        dump << endl;
        for (smoothedTrack_t::const_iterator i = smoothedTrack.begin(); i != smoothedTrack.end(); ++i) {
            GPS::Track* t = (*i).second;
            dump << "  " << setw(25) << left << setfill(' ')
                 << (*i).first << ": "
                 << setprecision(4)
                 << setw(5) << right
                 << t->ascent() << "m / "
                 << setw(5) << right
                 << t->descent() << "m ("
                 << setprecision(3)
                 << setw(4) << right;
            if (circular)
                dump << (t->avgUphillSlope() + t->avgDownhillSlope()).value() / 2;
            else
                dump << t->avgUphillSlope() << "% / " << t->avgDownhillSlope();
            dump << "%)" << endl;
        }
        dump.close();
    }
}
