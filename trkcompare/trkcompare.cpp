// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <getopt.h>

#include "gpslib/GeoCoord.h"
#include "gpslib/Trackpoint.h"
#include "gpslib/Point.h"
#include "gpslib/Vector.h"
#include "gpslib/Track.h"
#include "gpslib/GPXFile.h"
#include "gpslib/Stochastics.h"

using namespace std;
using namespace GPS;

bool quiet = false;
bool doDump = false;
int verbose = 0;

enum _long_options {
    SELECT_HELP,
    SELECT_DEBUG,
    SELECT_QUIET,
    SELECT_DUMP,
    SELECT_VERBOSE
};


static struct option long_options[] = {
    { "help",      no_argument,       0, SELECT_HELP },
    { "quiet",     no_argument,       0, SELECT_QUIET },
    { "debug",     no_argument,       0, SELECT_DEBUG },
    { "dump",      no_argument,       0, SELECT_DUMP },
    { "verbose",   no_argument,       0, SELECT_VERBOSE },
    { 0,           0,                 0, 0 }
};


void disclaimer(void)
{
    cout << "trkcompare - Ermitteln, wie gut GPS-Tracks mit einer Referenz übereinstimmen." << endl
        << "Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>" << endl
        << "Alle Rechte vorbehalten." << endl
        << endl;
}


void usage(void)
{
    cout << "Aufruf: " << endl
        << "   trkcompare [Optionen] Referenztrack <Liste mit Vergleichstracks>" << endl
        << endl
        << "Optionen:" << endl
        << "   --verbose | -v     " << endl
        << "   --quiet | -q       nur Ergebnisse ausgeben" << endl
        << "   --help | -? | -h   diese Hilfe ausgeben" << endl
        ;
}


int main(int argc, char* argv[])
{
    if (!quiet)
        disclaimer();
    int option_index;
    for (;;) {
        option_index = 0;
        int c = getopt_long(argc, argv, "h?vq", long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
        case 'h':
            // fall-through
        case '?':
            // fall-through
        case SELECT_HELP:
            usage();
            return EXIT_SUCCESS;
        case SELECT_VERBOSE:
            // fall-through
        case 'v':
            ++verbose;
            break;
        case SELECT_DUMP:
            doDump = true;
            break;
        case SELECT_QUIET:
            // fall-through
        case 'q':
            quiet = true;
            break;
        default:
            usage();
            return EXIT_FAILURE;
        }
    }

    if (argc - optind < 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    string refFilename = argv[optind++];
    if (!quiet)
        cout << "Lesen von " << refFilename << " .. " << endl;
    GPXFile ref;
    errno_t rc = ref.load(refFilename);
    if (rc != 0) {
        cerr << "fehlgeschlagen." << endl;
        exit(rc);
    }
    Track* refTrack = ref.track();
    if (doDump)
        refTrack->dump(cout, refFilename);    

    for (/* ... */; optind < argc; ++optind)
    {
        string cmpFilename = argv[optind];
        cout << "Lesen von " << cmpFilename << " .. " << endl;
        GPXFile cmp;
        rc = cmp.load(cmpFilename);
        if (rc != 0) {
            cerr << "fehlgeschlagen." << endl;
            exit(rc);
        }
        Stochastics* sres = cmp.track()->compareTo(refTrack);
        double lengthDiff = cmp.track()->distance() - ref.track()->distance();
        cout << "  mittlerer Fehler    = " << noshowpoint << left << setprecision(8) << sres->average() << " m" << endl
            << "  min./max. Fehler    = " << sres->min() << " / " << sres->max() << endl
            << "  Varianz             = " << sres->variance() << endl
            << "  Standardabweichung  = " << sres->standardDeviation() << endl
            << "  Streckendifferenz   = " << lengthDiff << " m" << endl
            ;
    }
    return EXIT_SUCCESS;
}
