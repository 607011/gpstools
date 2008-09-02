// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <getopt.h>
#include <errno.h>

#include "gpslib/Track.h"
#include "gpslib/GPXFile.h"
#include "gpslib/helper.h"

using namespace std;
using namespace GPS;

const int DefaultPointCount = 500;

bool quiet = false;
int verbose = 1;
int pointCount = DefaultPointCount;

enum _long_options {
    SELECT_HELP,
    SELECT_DEBUG,
    SELECT_QUIET,
    SELECT_VERBOSE
};


static struct option long_options[] = {
    { "help",      no_argument,       0, SELECT_HELP },
    { "quiet",     no_argument,       0, SELECT_QUIET },
    { "verbose",   no_argument,       0, SELECT_VERBOSE },
    { 0,           0,                 0, 0 }
};


void disclaimer(void)
{
    std::cout << "splittrack - Track aus GPX-Datei in mehrere aufteilen." << endl
        << "Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>" << endl
        << "Alle Rechte vorbehalten." << endl << endl;
}


void usage(void)
{
    std::cout << "Aufruf: splittrack [-n x] track.gpx" << endl
        << "Optionen:" << endl
        << "  -n x" << endl
        << "     Teiltrack soll nicht mehr als x Punkte haben (Vorgabe: x = " << DefaultPointCount << ")" << endl
        << endl;
}


void errmsg(std::string str, int rc = 0, bool _usage = true)
{
    std::cerr << endl << "FEHLER (" << ((rc != 0)? rc : errno) << "): " << str << endl;
    if (_usage)
        usage();
    exit(EXIT_FAILURE);
}


void warnmsg(std::string str)
{
    std::cerr << endl << "WARNUNG: " << str << endl;
}


int main(int argc, char* argv[])
{
    for (;;) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "h?vqn:", long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
        case 'h':
            /* fall-through */
        case '?':
            /* fall-through */
        case SELECT_HELP:
            usage();
            return 0;
            break;
        case SELECT_VERBOSE:
            /* fall-through */
        case 'v':
            ++verbose;
            break;
        case SELECT_QUIET:
            /* fall-through */
        case 'q':
            quiet = true;
            break;
        case 'n':
            pointCount = atoi(optarg);
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (!quiet)
        disclaimer();

    if ((argc - optind) < 1) {
        usage();
        exit(EXIT_FAILURE);
    }

    string trkFile = argv[optind++];

    GPXFile gpxFile;
    int rc = gpxFile.load(trkFile);
    if (rc != 0)
        errmsg("Kann GPX-Datei nicht laden", rc);

    TrackList allTracks;

    for (TrackList::iterator i = gpxFile.tracks().begin(); i != gpxFile.tracks().end(); ++i)
    {
        cout << "  Name : " << (*i)->name() << endl;
        Track* trk = *i;

        if (trk->isEmpty())
            errmsg("Der Track enthält keine Trackpunkte");
        if (!trk->hasTimestamps() && !trk->hasDistance())
            errmsg("Der Track enthält weder Zeitstempel noch Positionsangaben");
        if (!trk->hasElevation())
            warnmsg("Der Track enthält keine Höhenangaben");
        if (!trk->hasDistance())
            warnmsg("Der Track enthält keine Positionsangaben");
        if (!trk->hasTimestamps())
            warnmsg("Der Track enthält keine Zeitstempel");

        if (verbose > 0 && !quiet)
            cout << "Track wird in Teil-Tracks mit nicht mehr als " << pointCount << " Punkten aufgeteilt .." << endl;

        TrackList* tl = trk->split(pointCount);
        for (TrackList::iterator j = tl->begin(); j != tl->end(); ++j)
            allTracks.push_back(*j);

    }

    GPXFile gpxOut;
    gpxOut.setTracks(allTracks);
    int npos = trkFile.find_last_of('.');
    string trkOutFile = trkFile;
    trkOutFile.insert(npos, "-split" + itos(pointCount));
    gpxOut.write(trkOutFile);

    if (verbose > 0 && !quiet)
        cout << allTracks.size() << " Track(s) gespeichert in " << trkOutFile << "." << endl;

    return EXIT_SUCCESS;
}
