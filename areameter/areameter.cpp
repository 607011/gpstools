// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <errno.h>
#include <getopt.h>

#include "gpslib/Track.h"
#include "gpslib/GPXFile.h"
#include "gpslib/helper.h"

// gettext-Dummy
#define _(s) s

using namespace std;
using namespace GPS;

bool quiet = false;
int verbose = 1;

enum _long_options {
    SELECT_HELP,
    SELECT_DEBUG,
    SELECT_QUIET,
    SELECT_VERBOSE,
    SELECT_VERSION
};


static const string VERSION = "0.9.0-BETA";

static struct option long_options[] = {
    { "help",      no_argument,       0, SELECT_HELP },
    { "quiet",     no_argument,       0, SELECT_QUIET },
    { "verbose",   no_argument,       0, SELECT_VERBOSE },
    { "version",   no_argument,       0, SELECT_VERSION },
    { 0,           0,                 0, 0 }
};


void disclaimer(void)
{
    std::cout << "areameter - Von einem Track eingeschlossene Flaeche berechnen." << endl
        << "Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>" << endl
        << "Alle Rechte vorbehalten." << endl << endl;
}


void usage(void)
{
    cout << _("Aufruf: areameter track.gpx\n") << endl
	 << endl
	 << _("Optionen:\n"
	     "  -v            Mehr Information über Verarbeitungsschritte ausgeben\n"
	     "  -q            Saemtliche Ausgaben unterdruecken\n"
	     "  --version     Versionsinformationen ausgeben")
	 << endl << endl;
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
        int c = getopt_long(argc, argv, "h?vq", long_options, &option_index);
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
        case SELECT_VERSION:
            disclaimer();
            cout << "Version: " << VERSION << endl << endl;
            exit(EXIT_SUCCESS);
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

    string gpxFilename = argv[optind];
    if (gpxFilename == "") {
        usage();
        exit(EXIT_FAILURE);
    }

    cout << "Laden von " << gpxFilename << " .." << endl;
    GPXFile gpxFile;
    int rc = gpxFile.load(gpxFilename);
    if (rc != 0)
        errmsg("Kann GPX-Datei nicht laden", rc);

    TrackList allTracks;

    for (TrackList::const_iterator i = gpxFile.tracks().begin(); i != gpxFile.tracks().end(); ++i)
    {
        Track* trk = (*i);
        if (trk != NULL)
        {
            cout << "Track: " << trk->name() << endl;

            if (trk->isEmpty())
                errmsg("Der Track enthaelt keine Trackpunkte");
            if (!trk->hasDistance())
                errmsg("Der Track enthaelt keine Positionsangaben");

            cout << "  Flaecheninhalt: ";
            double area = trk->area();
            if (area > 1e6)
                cout << 1e-6 * area << " qkm";
            else
                cout << area << " qm";
            if (area > 1e4)
                cout << " (" << 1e-5 * area << " ha)";
            cout << endl << endl;
        }
    }

    return EXIT_SUCCESS;
}
