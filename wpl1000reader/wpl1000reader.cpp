// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <cstdlib>

#include <getopt.h>

#include "gpslib/WPL1000File.h"
#include "gpslib/GPXFile.h"

// gettext-Dummy
#define _(s) s

using namespace std;
using namespace GPS;


#ifdef WIN32
const char PathDelimiter = '\\';
#else
const char PathDelimiter = '/';
#endif


enum _long_options {
    SELECT_HELP,
    SELECT_DEBUG,
    SELECT_QUIET,
    SELECT_VERBOSE,
    SELECT_MULTI,
    SELECT_VERSION,
    SELECT_SIMULATE
};


static struct option long_options[] = {
    { "help",      no_argument,       0, SELECT_HELP },
    { "quiet",     no_argument,       0, SELECT_QUIET },
    { "verbose",   no_argument,       0, SELECT_VERBOSE },
    { "version",   no_argument,       0, SELECT_VERSION },
    { "simulate",  no_argument,       0, SELECT_SIMULATE },
    { "multi",     no_argument,       0, SELECT_MULTI },
    { 0,           0,                 0, 0 }
};


const string VERSION = "0.9.6";
bool quiet = false;
bool multi = false;
bool simulate = false;
int verbose = 0;


void disclaimer(void)
{
    if (!quiet)
    {
        cout << _("wpl1000reader - Tracks aus einem GPS-Logger vom Typ 'Wintec WPL-1000\n"
            << "                (Easy Showily)' oder 'Navilock NL-456DL Easy Logger'\n"
            << "                auslesen und als GPX-Datei speichern.\n")
            << _("Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>") << endl
            << _("Alle Rechte vorbehalten.") << endl
            << endl;
    }
}


void usage(void)
{
    cout << _("Aufruf: wpl1000reader [Optionen] <nvpipe.dat> <ausgabedatei.gpx>") << endl
	 << endl
	 << _("Optionen:\n"
	      "  -v            Mehr Information über Verarbeitungsschritte ausgeben\n"
	      "  -m  --multi   Jeden Track in eine separate Datei schreiben\n"
	      "  -q            Saemtliche Ausgaben unterdruecken\n"
	      "  --simulate    Nichts schreiben, sondern nur so tun als ob\n"
	      "                (funktioniert nur zusammen mit --multi)\n"
	      "  --version     Versionsinformationen ausgeben")
	 << endl << endl;
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
        case SELECT_SIMULATE:
            simulate = true;
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

    disclaimer();

    if ((argc - optind) < 2) {
        usage();
        return EXIT_FAILURE;
    }

    string wpl1000Filename = argv[optind];
    string gpxFilename = argv[++optind];

    if (wpl1000Filename == "" || gpxFilename == "") {
        usage();
        return EXIT_FAILURE;
    }

    if (!quiet)
        cout << _("Laden von ") << wpl1000Filename << " .." << endl;
    WPL1000File wpl1000File;
    errno_t rc = wpl1000File.load(wpl1000Filename);
    if (rc != 0) {
        cout << _("FEHLER: Laden von ") << wpl1000Filename << _(" fehlgeschlagen.") << endl;
        return rc;
    }
    if (wpl1000File.tracks().size() > 0)
    {
        if (multi)
        {
            for (TrackList::const_iterator i = wpl1000File.tracks().begin(); i != wpl1000File.tracks().end(); ++i)
            {
                GPXFile trkFile;
                trkFile.addTrack(*i);
                int spos = gpxFilename.find_last_of(PathDelimiter);
                string trkFilename = gpxFilename;
                trkFilename.insert(spos+1, (*i)->startTimestamp().toString("%Y%m%d-%H%M") + "-");
                if (!quiet)
                    cout << _("Speichern von ") << trkFilename << " .." << endl;
                if (!simulate)
                {
                    rc = trkFile.write(trkFilename);
                    if (rc != 0) {
                        cout << _("FEHLER: Speichern von ") << gpxFilename << _(" fehlgeschlagen.") << endl;
                        return rc;
                    }
                }
            }
            if (!simulate)
            {
                GPXFile wptFile;
                wptFile.setWaypoints(wpl1000File.waypoints());
                string wptFilename = gpxFilename;
                int ppos = gpxFilename.find_last_of('.');
                wptFilename.insert(ppos, "-waypoints");
                wptFile.write(wptFilename);
            }
        }
        else // !multi
        {   
            GPXFile gpxFile;
            gpxFile.setTracks(wpl1000File.tracks());
            gpxFile.setWaypoints(wpl1000File.waypoints());
            if (!quiet && verbose > 0) {
                cout << "Track(s) @ ";
                for (TrackList::const_iterator i = gpxFile.tracks().begin(); i != gpxFile.tracks().end(); ++i)
                    cout << (*i)->name() << " ";
            }
            if (!quiet)
                cout << _("Speichern unter ") << gpxFilename << " .." << endl;
            rc = gpxFile.write(gpxFilename);
            if (rc != 0) {
                cout << _("FEHLER: Speichern von ") << gpxFilename << _(" fehlgeschlagen.") << endl;
                return rc;
            }
        }
    }
    else
    {
        cout << _("Die Datei enthaelt keine Tracks!") << endl;
        return EXIT_FAILURE;
    }
    if (!quiet)
        cout << endl << endl;
    return EXIT_SUCCESS;
}
