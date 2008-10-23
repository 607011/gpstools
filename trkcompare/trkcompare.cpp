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
    std::cout << "trkcompare - Ermitteln, wie gut GPS-Tracks mit einer Referenz übereinstimmen." << std::endl
        << "Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>" << std::endl
        << "Alle Rechte vorbehalten." << std::endl
        << std::endl;
}


void usage(void)
{
    std::cout << "Aufruf: " << std::endl
        << "   trkcompare [Optionen] Referenztrack <Liste mit Vergleichstracks>" << std::endl
        << std::endl
        << "Optionen:" << std::endl
        << "   --verbose | -v     " << std::endl
        << "   --quiet | -q       nur Ergebnisse ausgeben" << std::endl
        << "   --help | -? | -h   diese Hilfe ausgeben" << std::endl
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

    std::string refFilename = argv[optind++];
    if (!quiet)
        std::cout << "Lesen von " << refFilename << " .. " << std::endl;
    GPXFile ref;
    errno_t rc = ref.load(refFilename);
    if (rc != 0) {
        std::cerr << "fehlgeschlagen." << std::endl;
        exit(rc);
    }
    Track* refTrack = ref.tracks().front();
    if (doDump)
        refTrack->dump(std::cout, refFilename);    

    for (/* ... */; optind < argc; ++optind)
    {
        std::string cmpFilename = argv[optind];
        std::cout << "Lesen von " << cmpFilename << " .. " << std::endl;
        GPXFile cmp;
        rc = cmp.load(cmpFilename);
        if (rc != 0) {
            std::cerr << "fehlgeschlagen." << std::endl;
            exit(rc);
        }
        Stochastics* sres = cmp.tracks().front()->compareTo(refTrack);
        double lengthDiff = cmp.tracks().front()->distance() - ref.tracks().front()->distance();
        std::cout << "  mittlerer Fehler    = " << std::noshowpoint << std::left << std::setprecision(8) << sres->average() << " m" << std::endl
            << "  min./max. Fehler    = " << sres->minimum() << " / " << sres->maximum() << std::endl
            << "  Varianz             = " << sres->variance() << std::endl
            << "  Standardabweichung  = " << sres->standardDeviation() << std::endl
            << "  Streckendifferenz   = " << lengthDiff << " m" << std::endl
            ;
    }
    return EXIT_SUCCESS;
}
