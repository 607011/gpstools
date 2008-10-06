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

enum _direction {
    PLUS = +1,
    MINUS = -1,
    ABSOLUTE = 0
};

int direction = ABSOLUTE;
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
    cout << "gpxtimeshift - Die Zeitstempel aller Trackpunkte in einer GPX-Datei um" << endl
         << "               ein angegebenes Mass in die Zukunft oder die Vergangenheit" << endl
         << "               verschieben." << endl
         << "Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>" << endl
         << "Alle Rechte vorbehalten." << endl
         << endl;
}


void usage(void)
{
    cout << "Aufruf: " << endl
         << "   gpxtimeshift eingabe.gpx ausgabe.gpx [ --plus | --minus | --to ] Datum/Zeit" << endl
         << endl
         << "Optionen:" << endl
         << "   --plus hh:mm:ss            um hh:mms:ss in die Zukunft verschieben" << endl
         << "   --minus hh:mm:ss           um hh:mms:ss in die Vergangenheit verschieben" << endl
         << "   --to YYYY-MM-DDThh:mmZ     Zeitstempel an Datum ausrichten" << endl
         << "   --verbose | -v" << endl
         << "   --quiet   | -q             nur Ergebnisse ausgeben" << endl
         << "   --help    | -? | -h        diese Hilfe ausgeben" << endl
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
            direction = PLUS;
            amount = optarg;
            break;
        case SELECT_MINUS:
            direction = MINUS;
            amount = optarg;
            break;
        case SELECT_TO:
            direction = ABSOLUTE;
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
        cout << "Lesen von " << gpxInFile << " .. " << endl;
    GPXFile gpx;
    errno_t rc = gpx.load(gpxInFile);
    if (rc != 0) {
        cerr << "fehlgeschlagen." << endl;
        exit(rc);
    }

    switch (direction)
    {
    case ABSOLUTE:
        gpx.track()->shiftTimestamps(Timestamp(amount));
        break;
    case MINUS:
        amount = "-" + amount;
        gpx.track()->shiftTimestamps(amount);
        break;
    case PLUS:
        amount = "+" + amount;
        gpx.track()->shiftTimestamps(amount);
        break;
    default:
        cerr << "Oops!" << endl;
        break;
    }

    gpx.write(gpxOutFile);
    
    return EXIT_SUCCESS;
}
