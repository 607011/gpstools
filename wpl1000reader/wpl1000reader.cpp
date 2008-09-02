// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <cstdlib>

#include "gpslib/WPL1000File.h"
#include "gpslib/GPXFile.h"


using namespace std;
using namespace GPS;


bool quiet = false;


void disclaimer(void)
{
    if (!quiet)
    {
        cout << "wpl1000reader - Tracks aus einem GPS-Logger vom Typ 'Wintec WPL-1000" << endl
            << "                (Easy Showily)' oder 'Navilock NL-456DL Easy Logger'" << endl
            << "                auslesen und als GPX-Datei speichern." << endl
            << "Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>" << endl
            << "Alle Rechte vorbehalten." << endl << endl;
    }
}


void usage(void)
{
    cout << "Aufruf: wpl1000reader <nvpipe.dat> <ausgabedatei.gpx>" << endl;
}


int main(int argc, char* argv[])
{
    disclaimer();
    if (argc < 3) {
        usage();
        return EXIT_FAILURE;
    }
    string wpl1000Filename = argv[1];
    WPL1000File wpl1000File;
    errno_t rc = wpl1000File.load(wpl1000Filename);
    if (rc != 0) {
        cout << "FEHLER: Laden von " << wpl1000Filename << " fehlgeschlagen." << endl;
        return rc;
    }
    string gpxFilename = argv[2];
    GPXFile gpxFile;
    gpxFile.setTracks(wpl1000File.tracks());
    rc = gpxFile.write(gpxFilename);
    if (rc != 0) {
        cout << "FEHLER: Speichern von " << gpxFilename << " fehlgeschlagen." << endl;
        return rc;
    }
    return EXIT_SUCCESS;
}
