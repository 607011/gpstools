// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
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

enum _direction {
    DIR_PLUS = +1,
    DIR_MINUS = -1,
    DIR_ABSOLUTE = 0
};

int direction = DIR_ABSOLUTE;
bool quiet = false;
bool doDump = false;
int verbose = 0;

enum _long_options {
    SELECT_HELP,
    SELECT_DEBUG,
    SELECT_QUIET,
    SELECT_DUMP,
    SELECT_VERBOSE,
    SELECT_TO,
    SELECT_PLUS,
    SELECT_MINUS
};


static struct option long_options[] = {
    { "help",      no_argument,       0, SELECT_HELP },
    { "quiet",     no_argument,       0, SELECT_QUIET },
    { "debug",     no_argument,       0, SELECT_DEBUG },
    { "dump",      no_argument,       0, SELECT_DUMP },
    { "to",        required_argument, 0, SELECT_TO },
    { "plus",      required_argument, 0, SELECT_PLUS },
    { "minus",     required_argument, 0, SELECT_MINUS },
    { "verbose",   no_argument,       0, SELECT_VERBOSE },
    { 0,           0,                 0, 0 }
};


void disclaimer(void)
{
    std::cout << "gpxtimeshift - Die Zeitstempel der Trackpunkte aller Tracks in einer" << std::endl
         << "               GPX-Datei um ein angegebenes Mass in die Zukunft oder" << std::endl
         << "               die Vergangenheit verschieben." << std::endl
         << "Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>" << std::endl
         << "Alle Rechte vorbehalten." << std::endl
         << std::endl;
}


void usage(void)
{
    std::cout << "Aufruf: " << std::endl
         << "   gpxtimeshift eingabe.gpx ausgabe.gpx [ --plus | --minus | --to ] Datum/Zeit" << std::endl
         << std::endl
         << "Optionen:" << std::endl
         << "   --plus hh:mm:ss            um hh:mms:ss in die Zukunft verschieben" << std::endl
         << "   --minus hh:mm:ss           um hh:mms:ss in die Vergangenheit verschieben" << std::endl
         << "   --to YYYY-MM-DDThh:mmZ     Zeitstempel an Datum ausrichten" << std::endl
         << "   --verbose | -v" << std::endl
         << "   --quiet   | -q             nur Ergebnisse ausgeben" << std::endl
         << "   --help    | -? | -h        diese Hilfe ausgeben" << std::endl
        ;
}


int main(int argc, char* argv[])
{
    int option_index;
    std::string amount;
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
        case SELECT_PLUS:
            direction = DIR_PLUS;
            amount = optarg;
            break;
        case SELECT_MINUS:
            direction = DIR_MINUS;
            amount = optarg;
            break;
        case SELECT_TO:
            direction = DIR_ABSOLUTE;
            amount = optarg;
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

    if (argc - optind < 2)
    {
        usage();
        exit(EXIT_FAILURE);
    }

    std::string gpxInFile = argv[optind++];
    std::string gpxOutFile = argv[optind++];

    if (!quiet)
        disclaimer();

    if (!quiet)
        std::cout << "Lesen von " << gpxInFile << " .. " << std::endl;
    GPXFile gpx;
    errno_t rc = gpx.load(gpxInFile);
    if (rc != 0) {
        std::cerr << "fehlgeschlagen." << std::endl;
        exit(rc);
    }

    GPS::TrackList& trkList = gpx.tracks();
    for (GPS::TrackList::iterator i = trkList.begin(); i != trkList.end(); ++i)
    {
        switch (direction)
        {
        case DIR_ABSOLUTE:
            (*i)->shiftTimestamps(Timestamp(amount));
            break;
        case DIR_MINUS:
            amount = "-" + amount;
            (*i)->shiftTimestamps(amount);
            break;
        case DIR_PLUS:
            amount = "+" + amount;
            (*i)->shiftTimestamps(amount);
            break;
        default:
            std::cerr << "Oops!" << std::endl;
            break;
        }
    }

    gpx.write(gpxOutFile);
    
    return EXIT_SUCCESS;
}
