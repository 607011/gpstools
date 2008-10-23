// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <getopt.h>
#include <cassert>
#include <errno.h>

#include "globals.h"

using namespace GPS;

enum _long_options {
    SELECT_ELE,
    SELECT_HR,
    SELECT_HELP,
    SELECT_GNUPLOT,
    SELECT_XAXIS,
    SELECT_DEBUG,
    SELECT_QUIET,
    SELECT_DUMP,
    SELECT_CONFIG,
    SELECT_VERBOSE,
};


static struct option long_options[] = {
    { "ele",       required_argument, 0, SELECT_ELE },
    { "hr",        required_argument, 0, SELECT_HR },
    { "help",      no_argument,       0, SELECT_HELP },
    { "gnuplot",   required_argument, 0, SELECT_GNUPLOT },
    { "quiet",     no_argument,       0, SELECT_QUIET },
    { "debug",     no_argument,       0, SELECT_DEBUG },
    { "dump",      required_argument, 0, SELECT_DUMP },
    { "config",    required_argument, 0, SELECT_CONFIG },
    { "verbose",   no_argument,       0, SELECT_VERBOSE },
    { 0,           0,                 0, 0 }
};




int main(int argc, char* argv[])
{
    for (;;) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "h?vqc:", long_options, &option_index);
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
        case SELECT_ELE:
            if (optarg != NULL)
                eleFileCmdline = optarg;
            break;
        case SELECT_HR:
            if (optarg != NULL)
                hrFileCmdline = optarg;
            break;
        case SELECT_DUMP:
            if (optarg != NULL)
                dumpFileCmdline = optarg;
            break;
        case SELECT_GNUPLOT:
            if (optarg != NULL)
                gnuplotExeCmdline = optarg;
            break;
        case SELECT_CONFIG:
            /* fall-through */
        case 'c':
            if (optarg != NULL)
                configFile = optarg;
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

    if (!quiet)
        disclaimer();

    if (configFile == "")
    {
        usage();
        exit(EXIT_FAILURE);
    }

    loadConfiguration();

    if (eleFileCmdline != "")
        eleFile = eleFileCmdline;
    if (hrFileCmdline != "")
        hrFile = hrFileCmdline;
    if (gnuplotExeCmdline != "")
        gnuplotExe = gnuplotExeCmdline;
    if (dumpFileCmdline != "")
        dumpFile = dumpFileCmdline;

    if (smoothings.size() == 0)
        errmsg(_("In der Konfigurationsdatei fehlt der Abschnitt calculations/smoothing"));

    if (eleFile == "")
        errmsg(_("Angabe der Datei mit den Höheninformationen fehlt"));

    GPXFile gpxFile;
    int rc = gpxFile.load(eleFile);
    if (rc != 0)
        errmsg(_("Kann GPX-Datei nicht laden"), rc);

    if (gpxFile.tracks().size() == 0)
        errmsg(_("Die GPX-Datei enthält keine Tracks"));

    GPS::TrackList trkList = gpxFile.tracks();

    trk = gpxFile.tracks().front();
    if (gpxFile.tracks().size() > 1)
    {
        if (trackSelectBy == "name")
        {
            trk = gpxFile.trackByName(trackSelector);
            if (trk == NULL)
                errmsg(_("Track konnte unter dem angegebenen Namen nicht gefunden werden"));
        }
        else if (trackSelectBy == "number")
        {
            trk = gpxFile.trackByNumber(atoi(trackSelector.c_str()));
            if (trk == NULL)
                errmsg(_("Track konnte unter der angegebenen Nummer nicht gefunden werden"));
        }
        else
        {
            warnmsg(_("Die GPX-Datei enthält mehrere Tracks und die Konfigurationsdatei keinen Hinweis in <trk>/<select>, welcher verwendet werden soll. Es wird automatisch der erste ausgewählt."));
        }
    }

    if (!trk->hasTimestamps() && !trk->hasDistance())
        errmsg(_("Der Track enthält weder Zeitstempel noch Positionsangaben"));
    if (!trk->hasElevation())
        warnmsg(_("Der Track enthält keine Höhenangaben"));
    if (!trk->hasDistance())
        warnmsg(_("Der Track enthält keine Positionsangaben"));
    if (!trk->hasTimestamps())
        warnmsg(_("Der Track enthält keine Zeitstempel"));

    trk->shiftTimestamps(eleTimeOffset);

    if (hrFile != "")
    {
        if (verbose > 0 && !quiet)
            std::cout << _("Zusammenführen von ") << eleFile << " " << _("mit") << " " << hrFile << " .." << std::endl;
        SuuntoDatafile suuntoFile;
        if (suuntoFile.load(hrFile) == 0) {
            suuntoFile.track()->shiftTimestamps(hrTimeOffset);
            trk->merge(suuntoFile.track());
        }
    }

    if (gnuplotSpeedIntervalSeconds > 0)
        trk->calculateSpeeds(gnuplotSpeedIntervalSeconds);
    else if (gnuplotSpeedIntervalMeters > 0)
        trk->calculateSpeeds(gnuplotSpeedIntervalMeters);
    else
        errmsg("Fehlermops!");

    if (!quiet)
        trk->dump(std::cout, "Original");
    smoothedTrack["Original"] = trk;

    for (std::vector<SmoothingOptions*>::const_iterator i = smoothings.begin(); i != smoothings.end(); ++i)
    {
        assert((*i) != NULL);
        Track* smoothed = NULL;
        const std::string& algorithm = (*i)->algorithm;
        if (algorithm != "")
        {
            const std::string& id = (*i)->id;
            if (algorithm == "douglas_peucker")
            {
                smoothed = trk->smoothDouglasPeucker((*i)->param, true);
            }
            else if (algorithm == "threshold")
            {
                smoothed = trk->smoothThreshold((*i)->param);
            }
            else
            {
                errmsg(std::string(_("Nicht unterstützter Glättungsalgorithmus:")) + " " + algorithm);
            }
            if (smoothed != NULL)
            {
                smoothed->keepAll();
                smoothed->calculateAscentDescent();
                if (!quiet)
                    smoothed->dump(std::cout, id);
                smoothedTrack[id] = smoothed;
            }
        }
    }

    dumpData();

    // do work
    writeGnuplotData();
    writeKml();
    writeGoogleMaps();
    executeGnuplot();

    if (verbose > 0 && !quiet)
        std::cout << std::endl << _("Fertig.") << std::endl;

    return EXIT_SUCCESS;
}

