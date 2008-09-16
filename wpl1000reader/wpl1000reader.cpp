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
    SELECT_VERBOSE,
    SELECT_MULTI,
    SELECT_VERSION
};


static struct option long_options[] = {
    { "help",      no_argument,       0, SELECT_HELP },
    { "quiet",     no_argument,       0, SELECT_QUIET },
    { "verbose",   no_argument,       0, SELECT_VERBOSE },
    { "version",   no_argument,       0, SELECT_VERSION },
    { "multi",     no_argument,       0, SELECT_MULTI },
    { 0,           0,                 0, 0 }
};


const std::string VERSION = "0.9.3";
bool quiet = false;
bool multi = false;
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
    cout << "Aufruf: wpl1000reader [Optionen] <nvpipe.dat> <ausgabedatei.gpx>" << endl
        << endl
        << "Optionen:" << endl
        << "  -v       Mehr Information ueber Verarbeitungsschritte ausgeben" << endl
        << "  --multi |" << endl
        << "  -m       Jeden Track in eine separate Datei schreiben" << endl
        << endl;
}


int main(int argc, char* argv[])
{
    for (;;) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "h?vqm", long_options, &option_index);
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
        case SELECT_MULTI:
            /* fall-through */
        case 'm':
            multi = true;
            break;
        case SELECT_VERSION:
            disclaimer();
            cout << "Version: " << VERSION;
            exit(EXIT_SUCCESS);
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
    gpxFile.setWaypoints(wpl1000File.waypoints());

    if (gpxFile.tracks().size() > 0)
    {
        if (multi)
        {
            int n = 0;
            for (GPS::TrackList::const_iterator i = gpxFile.tracks().begin(); i != gpxFile.tracks().end(); ++i) {
                std::string outFilename = gpxFilename;
                int npos = gpxFilename.find_last_of('.');
                outFilename.insert(npos, "-" + GPS::tos(n++));
                GPS::GPXFile outFile;
                outFile.addTrack(*i);
                if (!quiet)
                    cout << "Speichern von " << outFilename << " .." << endl;
                outFile.write(outFilename);
            }
        }
        else // !multi
        {   
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
    }
    else
    {
        cout << "Die Datei enthaelt keine Tracks!" << endl;
        return EXIT_FAILURE;
    }
    if (!quiet)
        cout << endl << endl;
    return EXIT_SUCCESS;
}
