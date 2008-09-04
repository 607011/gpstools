// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <cstdlib>

#include <getopt.h>

#include "gpslib/WPL1000File.h"
#include "gpslib/GPXFile.h"


using namespace std;
using namespace GPS;


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


bool quiet = false;
int verbose = 0;


void disclaimer(void)
{
    if (!quiet)
    {
        cout << "wpl1000reader - Tracks aus einem GPS-Logger vom Typ 'Wintec WPL-1000" << endl
            << "                (Easy Showily)' oder 'Navilock NL-456DL Easy Logger'" << endl
            << "                auslesen und als GPX-Datei speichern." << endl
            << "Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>" << endl
            << "Alle Rechte vorbehalten." << endl
            << endl;
    }
}


void usage(void)
{
    cout << "Aufruf: wpl1000reader [-v] <nvpipe.dat> <ausgabedatei.gpx>" << endl
        << endl
        << "Optionen:" << endl
        << "  -v       Mehr Information ueber Verarbeitungsschritte ausgeben" << endl
        << endl;
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
        default:
            usage();
            exit(EXIT_FAILURE);
            break;
        }
    }

    disclaimer();

    if ((argc - optind) < 2) {
        usage();
        return EXIT_FAILURE;
    }

    string wpl1000Filename = argv[optind++];
    if (!quiet)
        cout << "Laden von " << wpl1000Filename << " .." << endl;
    WPL1000File wpl1000File;
    errno_t rc = wpl1000File.load(wpl1000Filename);
    if (rc != 0) {
        cout << "FEHLER: Laden von " << wpl1000Filename << " fehlgeschlagen." << endl;
        return rc;
    }
    string gpxFilename = argv[optind++];
    GPXFile gpxFile;
    gpxFile.setTracks(wpl1000File.tracks());
    if (gpxFile.tracks().size() > 0) {
        if (!quiet && verbose > 0) {
            cout << "Track(s) @ ";
            for (GPS::TrackList::const_iterator i = gpxFile.tracks().begin(); i != gpxFile.tracks().end(); ++i)
                cout << (*i)->name() << " ";
        }
        if (!quiet)
            cout << "Speichern unter " << gpxFilename << " .." << endl;
        rc = gpxFile.write(gpxFilename);
        if (rc != 0) {
            cout << "FEHLER: Speichern von " << gpxFilename << " fehlgeschlagen." << endl;
            return rc;
        }
    }
    else {
        cout << "Die Datei enthaelt keine Tracks!";
    }
    cout << endl;
    if (!quiet)
        cout << endl;
    return EXIT_SUCCESS;
}
