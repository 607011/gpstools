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

using namespace std;
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

    if (configFile == "") {
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
        errmsg("In der Konfigurationsdatei fehlt der Abschnitt calculations/smoothing");

    if (eleFile == "")
        errmsg("Angabe der Datei mit den Höheninformationen fehlt");

    GPXFile gpxFile;
    int rc = gpxFile.load(eleFile);
    if (rc != 0)
        errmsg("Kann GPX-Datei nicht laden", rc);

    if (hrFile != "")
    {
        if (verbose > 0 && !quiet)
            cout << "Zusammenführen von " << eleFile << " mit " << hrFile << " .." << endl;
        SuuntoDatafile suuntoFile;
        if (suuntoFile.load(hrFile) == 0) {
            suuntoFile.track()->shiftTimestamps(hrTimeOffset);
            gpxFile.track()->merge(suuntoFile.track());
        }
    }

    Track* trk = gpxFile.track();

    if (trk->isEmpty())
        errmsg("Der Track enthält keine Trackpunkte");
    if (!trk->hasTimestamps() && !trk->hasDistance())
        errmsg("Der Track enthält weder Zeitstempel noch Positionsangaben");
    if (!trk->hasElevation())
        warnmsg("Der Track enthält keine Höhenangaben");
    if (!trk->hasDistance())
        warnmsg("Der Track enthält keine Positionsangaben");
    if (!trk->hasTimestamps())
        warnmsg("Der Track enthält keine Zeitstempel");

    if (gnuplotSpeedIntervalSeconds > 0)
        trk->calculateSpeeds(gnuplotSpeedIntervalSeconds);
    else if (gnuplotSpeedIntervalMeters > 0)
        trk->calculateSpeeds(gnuplotSpeedIntervalMeters);
    else
        errmsg("Ups!");

    trk->shiftTimestamps(eleTimeOffset);

    if (!quiet)
        trk->dump(cout, "Original");
    smoothedTrack["Original"] = trk;

    for (vector<SmoothingOptions*>::const_iterator i = smoothings.begin(); i != smoothings.end(); ++i)
    {
        assert((*i) != NULL);
        Track* smoothed = NULL;
        const string& algorithm = (*i)->algorithm;
        if (algorithm != "")
        {
            const string& id = (*i)->id;
            if (algorithm == "douglas_peucker") {
                smoothed = trk->smoothDouglasPeucker((*i)->param, true);
            }
            else if (algorithm == "threshold") {
                smoothed = trk->smoothThreshold((*i)->param);
            }
            else {
                errmsg(string("Nicht unterstützter Glättungsalgorithmus:") + algorithm);
            }
            if (smoothed != NULL) {
                smoothed->keepAll();
                smoothed->calculateAscentDescent();
                if (!quiet)
                    smoothed->dump(cout, id);
                smoothedTrack[id] = smoothed;
            }
        }
    }

    fstream dump;
    if (dumpFile != "")
        dump.open(dumpFile.c_str(), fstream::out);

    if (dump.good())
    {
        bool circular = trk->points().front()->rangeTo(trk->points().back()) < MAX_RANGE_BETWEEN_START_AND_FINISH;
        dump << "Startzeit:        " << trk->startTimestamp().toString() << endl
             << "Zielzeit:         " << trk->finishTimestamp().toString() << endl
             << setprecision(5) << noshowpoint
             << "Entfernung:       " << 1e-3*trk->distance() << "km" << endl
             << "Höhe min/max:     " << trk->minElevation().value() << "m / " << trk->maxElevation().value() << "m" << endl
             << setprecision(3)
             << "mittl. Geschw.:   " << trk->avgSpeed() << " km/h" << endl;
        if (trk->avgHeartrate().defined())
            dump << "mittl. Herzfreq.: " << (int) trk->avgHeartrate().value() << " /min" << endl;
        if (trk->avgTemperature().defined())
            dump << "mittl. Temp.:     " << trk->avgTemperature().value() << " °C" << endl;
        dump << "An-/Abstiege ";
        if (circular)
            dump << "(mittl. Anstieg in %)";
        else
            dump << "(mittl. An-/Abstieg in %)";
        dump << endl;
        for (smoothedTrack_t::const_iterator i = smoothedTrack.begin(); i != smoothedTrack.end(); ++i) {
            Track* t = (*i).second;
            dump << "  " << setw(25) << left << setfill(' ')
                 << (*i).first << ": "
                 << setprecision(4)
                 << setw(5) << right
                 << t->ascent() << "m / "
                 << setw(5) << right
                 << t->descent() << "m ("
                 << setprecision(3)
                 << setw(4) << right;
            if (circular)
                dump << (t->avgUphillSlope() + t->avgDownhillSlope()).value() / 2;
            else
                dump << t->avgUphillSlope() << "% / " << t->avgDownhillSlope();
            dump << "%)" << endl;
        }
        dump.close();
    }

    gnuplotElevationPct /= 100;
    gnuplotHeartratePct /= 100;
    gnuplotSpeedPct     /= 100;
    gnuplotSlopePct     /= 100;

    double gnuplotElevationOrigin = gnuplotSlopePct + gnuplotSpeedPct + gnuplotHeartratePct;
    double gnuplotHeartrateOrigin = gnuplotSlopePct + gnuplotSpeedPct;
    double gnuplotSpeedOrigin     = gnuplotSlopePct;
    double gnuplotSlopeOrigin     = 0.0;

    string outputFile(title);
    outputFile += "." + gnuplotSuffix;

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma region Schreiben der gnuplot-Steueranweisungen
#endif
    /////////////////////////////////////////////////////////////////
    // Schreiben der gnuplot-Steueranweisungen
    /////////////////////////////////////////////////////////////////
    if (verbose > 0 && !quiet)
        cout << endl << "Schreiben der gnuplot-Steueranweisungen nach " << gnuplotPltFile << " ..";
    int xcolumn = 1;
    string xlabel;
    string xformat;
    fstream gnuplotStream(gnuplotPltFile.c_str(), fstream::out);
    if (gnuplotStream.bad())
        errmsg("Fehler beim Öffnen der Gnuplot-Steuer-Datei '" + gnuplotPltFile + "'");

    gnuplotStream << "reset" << endl
        << "set terminal " << gnuplotFormat << endl
        << "set output \"" << outputFile << "\"" << endl
        << "set yrange [*:*]" << endl
        << "set grid" << endl
        << "set lmargin 9" << endl
        << "set rmargin 2" << endl
        << "set multiplot" << endl
        << "set datafile missing \"" << MISSING << "\"" << endl
        << "set ylabel \"Höhe [m]\" offset 1" << endl;
    if (gnuplotXAxis == "distance") {
        gnuplotStream << "set xrange [0:" << 1e-3 * trk->distance() << "]" << endl;
        xcolumn = 1;
        xlabel  = "Distanz [km]";
        xformat = "";
    }
    else if (gnuplotXAxis == "duration") {
        gnuplotStream << "set timefmt \"%H:%M:%S\"" << endl
            << "set xdata time" << endl
            << "set xrange [*:\"" << secs2timestamp(trk->duration()) << "\"]" << endl;
        xcolumn = 11;
        xlabel  = "Dauer";
        xformat = "%H:%M";
    }
    else if (gnuplotXAxis == "datetime") {
        gnuplotStream << "set timefmt \"%Y-%m-%d %H:%M:%S\"" << endl
            << "set xdata time" << endl
            << "set xrange [\"" << trk->startTimestamp().toString("%Y-%m-%d %H:%M:%S") << "\":\"" << trk->finishTimestamp().toString("%Y-%m-%d %H:%M:%S") << "\"]" << endl;
        title += " ("  + trk->startTimestamp().toString("%#d.%#m.%Y %#H:%M") + ")";
        xcolumn = 12;
        xlabel  = "Datum/Uhrzeit";
        xformat = "%H:%M";
    }
    else {
        errmsg("Nicht unterstützte X-Achsen-Skalierung: " + gnuplotXAxis);
    }
    gnuplotStream << "set title \"" << title << "\"" << endl;
    if (gnuplotElevationLo.defined() && gnuplotElevationHi.defined())
        gnuplotStream << "set yrange [" << gnuplotElevationLo.value() << ":" << gnuplotElevationHi.value() << "]" << endl;
    if ((trk->hasHeartrate() && gnuplotHeartratePct > 0) ||
        (trk->hasSpeed() && gnuplotSpeedPct > 0) ||
        (trk->hasElevation() && gnuplotSlopePct > 0))
    {
        gnuplotStream << "set format x \"\"" << endl
            << "unset xlabel" << endl
            << "set bmargin 0" << endl;
    }
    else
    {
        gnuplotStream << "set format x" << endl
            << "set xlabel \"" << xlabel << "\"" << endl
            << "set bmargin" << endl;
    }
    gnuplotStream << "set size 1.0, " << gnuplotElevationPct << endl
        << "set origin 0, " << gnuplotElevationOrigin << endl;

    gnuplotDatFile = string(smoothings.front()->id) + ".dat";
    gnuplotStream << "plot ";
    for (vector<SmoothingOptions*>::const_iterator i = smoothings.begin(); i != smoothings.end(); ++i)
    {
        if ((*i)->draw) {
            if (i != smoothings.begin())
                gnuplotStream << ", ";
            gnuplotStream << "\"" << (*i)->id << ".dat" << "\" "
                << "using " << xcolumn << ":2 "
                << (gnuplotLegend? "title \"" + string((*i)->id) + "\"" : "notitle")
                << " " << (*i)->gnuplotOption;
        }
    }
    gnuplotStream << endl;

    if (trk->hasHeartrate() && gnuplotHeartratePct > 0)
    {
        if ((trk->hasSpeed() && gnuplotSpeedPct > 0) ||
            (trk->hasElevation() && gnuplotSlopePct > 0))
        {
            gnuplotStream << "set format x \"\"" << endl
                << "unset xlabel" << endl
                << "set bmargin 0" << endl;
        }
        else
        {
            gnuplotStream << "set xlabel '" << xlabel << "'" << endl
                << "set format x" << endl
                << "set bmargin" << endl;
        }
        if (gnuplotHeartrateLo.defined() && gnuplotHeartrateHi.defined())
            gnuplotStream << "set yrange [" << gnuplotHeartrateLo.value() << ":" << gnuplotHeartrateHi.value() << "]" << endl;
        gnuplotStream << "unset title" << endl
            << "set size 1.0, " << gnuplotHeartratePct << endl
            << "set origin 0, " << gnuplotHeartrateOrigin << endl
            << "set ytics (40, 60, 80, 100, 120, 140, 160, 180, 200, 220)" << endl
            << "set ylabel 'HR' offset 1" << endl
            << "set tmargin 0" << endl;
        gnuplotStream << "plot ";
        if (gnuplotHeartrateAverage && trk->avgHeartrate().defined())
            gnuplotStream << trk->avgHeartrate().value() << " notitle lt 5 linecolor rgb \"#CAB0B0\", ";
        gnuplotStream << "\"" << gnuplotDatFile << "\" using " << xcolumn << ":3 notitle with lines linecolor rgbcolor \"#00cc00\"" << endl
            << "set ytics autofreq" << endl;
    }

    if (trk->hasSpeed() && gnuplotSpeedPct > 0)
    {
        if (trk->hasElevation() && gnuplotSlopePct > 0)
        {
            gnuplotStream << "set format x \"\"" << endl
                << "unset xlabel" << endl
                << "set bmargin 0" << endl;
        }
        else
        {
            gnuplotStream << "set xlabel \"" << xlabel << "\"" << endl
                << "set format x" << endl
                << "set bmargin" << endl;
        }
        if (gnuplotSpeedLo.defined() && gnuplotSpeedHi.defined())
            gnuplotStream << "set yrange [" << gnuplotSpeedLo.value() << ":" << gnuplotSpeedHi.value() << "]" << endl;
        else
            gnuplotStream << "set yrange [*:*]" << endl;
        gnuplotStream << "unset title" << endl
            << "set size 1.0, " << gnuplotSpeedPct << endl
            << "set origin 0, " << gnuplotSpeedOrigin << endl
            << "set tmargin 0" << endl
            << "set ylabel 'km/h' offset 1" << endl
            << "plot ";
        if (gnuplotSpeedAverage && trk->avgSpeed().defined())
            gnuplotStream << trk->avgSpeed().value() << " notitle lt 5 linecolor rgb \"#AACDA2\", ";
        gnuplotStream << "\"" << gnuplotDatFile << "\" using " << xcolumn << ":5 notitle with steps linecolor rgb \"#ff4433\"" << endl;
    }

    if (trk->hasElevation() && trk->hasDistance() && gnuplotSlopePct > 0)
    {
        string slopeSourceFile = string((gnuplotSlopeSource == "")? "Original" : gnuplotSlopeSource) + ".dat";
        if (gnuplotSlopeLo.defined() && gnuplotSlopeHi.defined())
            gnuplotStream << "set yrange [" << gnuplotSlopeLo << ":" << gnuplotSlopeHi << "]" << endl;
        else
            gnuplotStream << "set yrange [*:*]" << endl;
        gnuplotStream << "unset title" << endl
            << "set size 1.0, " << gnuplotSlopePct << endl
            << "set origin 0, " << gnuplotSlopeOrigin << endl
            << "set bmargin" << endl
            << "set tmargin 0" << endl
            << "set ylabel \"%\" offset 1" << endl
            << "set xlabel \"" << xlabel << "\"" << endl
            << "set format x" << endl
            << "plot \"" << slopeSourceFile << "\" using " << xcolumn << ":8 notitle with steps linecolor rgbcolor \"#0000ff\"" << endl;
    }

    gnuplotStream << "unset multiplot" << endl;
    gnuplotStream.close();
    if (verbose > 0 && !quiet)
        cout << " OK" << endl;
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma endregion
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma region Schreiben der gnuplot-Daten
#endif
    /////////////////////////////////////////////////////////////////
    // Schreiben der gnuplot-Daten
    /////////////////////////////////////////////////////////////////
    for (smoothedTrack_t::const_iterator j = smoothedTrack.begin(); j != smoothedTrack.end(); ++j)
    {
        if (verbose > 0 && !quiet)
            cout << "Schreiben der Gnuplot-Daten-Datei '" << (*j).first << "' ..";
        Track* t = (*j).second;
        gnuplotDatFile = string((*j).first) + string(".dat");
        fstream dat(gnuplotDatFile.c_str(), fstream::out);
        if (dat.bad())
            errmsg("Fehler beim Öffnen der Gnuplot-Daten-Datei '" + gnuplotDatFile + "'");
        // 1:dist 2:ele 3:hr 4:temp 5:speed 6:pace 7:pace2 8:slope 9:lon 10:lat 11:timestamp 12:duration
        TrackpointList& samples = t->points();
        for (TrackpointList::const_iterator i = samples.begin(); i != samples.end(); ++i)
        {
            dat << setprecision(5)
                << 1e-3 * (*i)->distance() << " ";
            if ((*i)->elevation().defined())
                dat << setprecision(4) << (*i)->elevation() << " ";
            else
                dat << "? ";
            if ((*i)->heartrate().defined())
                dat << (*i)->heartrate() << " ";
            else
                dat << "? ";
            if ((*i)->temperature().defined())
                dat << setprecision(3) << (*i)->temperature() << " ";
            else
                dat << "? ";
            dat << setprecision(4)
                << (*i)->speed() << " "
                << (*i)->pace() << " "
                << (*i)->paceString() << " "
                << (*i)->slope() << " "
                << setprecision(10)
                << (*i)->longitude() << " "
                << (*i)->latitude() << " "
                << secs2timestamp((*i)->duration()) << " "
                << (*i)->timestamp().toString("%Y-%m-%d %H:%M:%S") << " "
                << endl;
        }
        dat.close();
        if (verbose > 0 && !quiet)
            cout << " OK" << endl;
    }
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma endregion
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma region KML-Datei erzeugen
#endif
    /////////////////////////////////////////////////////////////////
    // KML-Datei mit Track und Kilometermarken
    // sowie Wegpunkten für Start und Ziel erzeugen
    /////////////////////////////////////////////////////////////////
    if (kmlFile != "")
    {
        if (verbose > 0 && !quiet)
            cout << "Schreiben der KML-Daten nach " << kmlFile << " ..";
        fstream kml(kmlFile.c_str(), fstream::out);
        if (kml.bad())
            errmsg("Öffnen der KML-Datendatei '" + kmlFile + "' fehlgeschlagen");
        kml << "<?xml version=\"1.0\" encoding=\"" << encoding << "\"?>" << endl
            << "<kml xlmns=\"http://earth.google.com/kml/2.2\">" << endl
            << "<Document>" << endl
            << "<name>" << title << "</name>" << endl
            << "<Style id=\"track\">" << endl
            << "<LineStyle>" << endl
            << "  <color>ff0000cc</color>" << endl
            << "  <width>3</width>" << endl
            << "</LineStyle>" << endl
            << "</Style>" << endl
            << "<Style id=\"finish_flag\">" << endl
            << "<IconStyle>" << endl
            << "  <Icon>" << endl
            << "    <href>http://maps.google.com/mapfiles/kml/paddle/grn-circle.png</href>" << endl
            << "  </Icon>" << endl
            << "  <hotSpot x=\"0.5\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>" << endl
            << "</IconStyle>" << endl
            << "</Style>" << endl
            << "<Style id=\"start_flag\">" << endl
            << " <IconStyle>" << endl
            << "   <Icon>" << endl
            << "     <href>http://maps.google.com/mapfiles/kml/paddle/red-circle.png</href>" << endl
            << "   </Icon>" << endl
            << "   <hotSpot x=\"0.5\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>" << endl
            << " </IconStyle>" << endl
            << "</Style>" << endl
            << "<Style id=\"km_flag\">" << endl
            << " <IconStyle>" << endl
            << "   <Icon>" << endl
            << "     <href>http://maps.google.com/mapfiles/kml/shapes/placemark_square.png</href>" << endl
            << "   </Icon>" << endl
            << " </IconStyle>" << endl
            << "</Style>" << endl;
        kml << "<Placemark>" << endl
            << " <name>" << title << "</name>" << endl
            << " <LineString>" << endl
            << "   <tesselate>1</tesselate>" << endl
            << "   <coordinates>" << endl;
        for (TrackpointList::const_iterator i = trk->points().begin(); i != trk->points().end(); ++i)
            kml << setprecision(14) << (*i)->longitude() << "," << (*i)->latitude() << ","
            << setprecision(4) << ((*i)->elevation().defined()? (*i)->elevation().value() : 0) << endl;
        kml << "   </coordinates>" << endl
            << " </LineString>" << endl
            << " <styleUrl>#track</styleUrl>" << endl
            << "</Placemark>" << endl;
        if (kmlMarkStart)
        {
            Trackpoint* trkpt = trk->points().front();
            kml << "<Placemark>" << endl
                << "  <name>Start</name>" << endl
                << "  <description>" << endl
                << "     <![CDATA[ <strong>Längengrad</strong>: " << setprecision(14) << trkpt->longitude() << "<br>" << endl
                << "               <strong>Breitengrad</strong>: " << trkpt->latitude() << "<br>" << endl
                << "               <strong>Höhe</strong>: " << setprecision(4) << trkpt->elevation() << "m ]]>" << endl
                << "  </description>" << endl
                << "  <Point>" << endl
                << "     <coordinates>" << setprecision(14) << trkpt->longitude() << "," << trkpt->latitude() << "," << setprecision(4) << trkpt->elevation() << "</coordinates>" << endl
                << "  </Point>" << endl
                << "  <styleUrl>#start_flag</styleUrl>" << endl
                << "</Placemark>" << endl;
        };
        if (kmlMarkFinish)
        {
            Trackpoint* trkpt = trk->points().back();
            kml << "<Placemark>" << endl
                << "  <name>Ziel</name>" << endl
                << "  <description>" << endl
                << "     <![CDATA[ <strong>Längengrad</strong>: " << setprecision(14) << trkpt->longitude() << "<br>" << endl
                << "               <strong>Breitengrad</strong>: " << trkpt->latitude() << "<br>" << endl
                << "               <strong>Höhe</strong>: " << setprecision(4) << trkpt->elevation() << "m ]]>" << endl
                << "  </description>" << endl
                << "  <Point>" << endl
                << "     <coordinates>" << setprecision(14) << trkpt->longitude() << "," << trkpt->latitude() << "," << setprecision(4) << trkpt->elevation() << "</coordinates>" << endl
                << "  </Point>" << endl
                << "  <styleUrl>#finish_flag</styleUrl>" << endl
                << "</Placemark>" << endl;
        };
        if (kmlKmTicks > 0)
        {
            double d = 1e3 * kmlKmTicks;
            for (TrackpointList::const_iterator i = trk->points().begin(); i != trk->points().end(); ++i)
            {
                if ((*i)->distance() >= d) {
                    TrackpointList::const_iterator prev = i - 1;
                    if ((*prev) != NULL) {
                        int bearing = 10 * (int)((*prev)->bearing(*i) / 10.0);
                        char deg[10];
#if defined(_WIN32) && (_MSC_VER >= 1400)
                        sprintf_s(deg, 9, "%03d", bearing);
#else
                        sprintf(deg, "%03d", bearing);
#endif
                        kml << "<Placemark>" << endl
                            << "  <name>" << (int) (1e-3 * d) << " km</name>" << endl
                            << "  <description>" << endl << setprecision(14)
                            << "     <![CDATA[ <strong>Längengrad</strong>: " << (*i)->longitude() << "<br>" << endl
                            << "               <strong>Breitengrad</strong>: " << (*i)->latitude() << "<br>" << endl
                            << "               <strong>Höhe</strong>: " << setprecision(4) << (*i)->elevation() << "m ]]>" << endl
                            << "  </description>" << endl
                            << "  <Point>" << endl
                            << "     <coordinates>" << setprecision(14) << (*i)->longitude() << "," << (*i)->latitude() << "," << setprecision(4) << (*i)->elevation() << "</coordinates>" << endl
                            << "  </Point>" << endl
                            << "  <styleUrl>#km_flag</styleUrl>" << endl
                            << "</Placemark>" << endl;
                    }
                    d += 1e3 * kmlKmTicks;
                }
            }
        }
        kml << "</Document>" << endl
            << "</kml>" << endl;
        kml.close();
        if (verbose > 0 && !quiet)
            cout << " OK" << endl;
    }
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma endregion
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma region Javascript mit Google-Maps-API-Aufrufen erzeugen
#endif
    /////////////////////////////////////////////////////////////////
    // Javascript mit Google-Maps-API-Aufrufen erzeugen
    /////////////////////////////////////////////////////////////////
    if (googleMapsFile != "")
    {
        if (verbose > 0 && !quiet)
            cout << "Schreiben der Google-Maps-API-Aufrufe nach " << googleMapsFile << " ..";
        fstream js(googleMapsFile.c_str(), fstream::out);
        if (js.bad())
            errmsg("Öffnen der Google-Maps-Javascript-Datei '" + googleMapsFile + "' fehlgeschlagen");
        js << "function initialize() {" << endl
            << "  if (GBrowserIsCompatible()) {" << endl
            << "  var map = new GMap2(document.getElementById(\"map_canvas\"));" << endl
            << "  function createMarker(km, lat, lng, ele, opts) {" << endl
            << "    var latlng = new GLatLng(lat, lng);" << endl
            << "    var marker = new GMarker(latlng, opts);" << endl
            << "    marker.value = km;" << endl
            << "    GEvent.addListener(marker,\"click\", function() {" << endl
            << "      var html = '<strong style=\"font-size: 125%\">' + km + '</strong>' +" << endl
            << "        '<br><b>Längengrad</b>: ' + lng +" << endl
            << "        '<br><b>Breitengrad</b>: ' + lat +" << endl
            << "        '<br><b>Höhe</b>: ' + ele + 'm';" << endl
            << "      map.openInfoWindowHtml(latlng, html);" << endl
            << "    });" << endl
            << "    return marker;" << endl
            << "  }" << endl
            << "  var track = new GPolyline([";
        for (TrackpointList::const_iterator i = trk->points().begin(); i != trk->points().end(); ++i)
        {
            if (i != trk->points().begin())
                js << ", ";
            js << "new GLatLng(" << (*i)->latitude() << ',' << (*i)->longitude() << ")";
        }
        js << "], \"#ff0000\", 5);}" << endl
            << "  map.setCenter(track.getBounds().getCenter(), 13);" << endl
            << "  map.addMapType(G_PHYSICAL_MAP);" << endl
            << "  map.setMapType(G_PHYSICAL_MAP);" << endl
            << "  map.addControl(new GMapTypeControl());" << endl
            << "  map.addControl(new GSmallMapControl());" << endl
            << "  map.addOverlay(track);" << endl
            << "  var baseIcon = new GIcon(); " << endl
            << "  baseIcon.iconSize = new GSize(32, 32); " << endl
            << "  baseIcon.iconAnchor = new GPoint(15, 15); " << endl
            << "  baseIcon.infoWindowAnchor = new GPoint(15, 15);" << endl
            << "  var startIcon = new GIcon(baseIcon); " << endl
            << "  startIcon.image = 'http://maps.google.com/mapfiles/kml/paddle/red-circle.png'; " << endl
            << "  startIcon.iconAnchor = new GPoint(15, 31); " << endl
            << "  startIcon.infoWindowAnchor = new GPoint(15, 31);" << endl
            << "  var finishIcon = new GIcon(baseIcon); " << endl
            << "  finishIcon.image = 'http://maps.google.com/mapfiles/kml/paddle/grn-circle.png'; " << endl
            << "  finishIcon.iconAnchor = new GPoint(15, 31);" << endl
            << "  finishIcon.infoWindowAnchor = new GPoint(15, 31);" << endl
            << "  var kmIcon = new GIcon(baseIcon); " << endl
            << "  kmIcon.image = 'http://maps.google.com/mapfiles/kml/shapes/placemark_square.png';" << endl;
        for (int i = 0; i < 360; i += 10)
        {
            char deg[10];
#if defined(_WIN32) && (_MSC_VER >= 1400)
            sprintf_s(deg, 9, "%03d", i);
#else
            sprintf(deg, "%03d", i);
#endif
            js << "  var arrow" << deg << " = new GIcon(baseIcon);"
                << " arrow" << deg << ".image = '" << staticImagesUrl << "/arrow" << deg << ".png';" << endl;
            // TODO: Domain konfigurierbar machen
        }
        Trackpoint* start = trk->points().front();
        Trackpoint* finish = trk->points().back();
        js << "  var startOpts = {title:'Start', icon:startIcon}; " << endl
            << "  map.addOverlay(createMarker('Start', "  << start->latitude() << ", " << start->longitude() << ", " << start->elevation() << ", startOpts));" << endl
            << "  var finishOpts = {title:'Finish', icon:finishIcon}; " << endl
            << "  map.addOverlay(createMarker('Ziel', " << finish->latitude() << ", " << finish->longitude() << ", " << finish->elevation() << ", finishOpts));" << endl;
        if (kmlKmTicks > 0)
        {
            double d = 1e3 * kmlKmTicks;
            for (TrackpointList::const_iterator i = trk->points().begin(); i != trk->points().end(); ++i)
            {
                if ((*i)->distance() >= d) {
                    TrackpointList::const_iterator prev = i - 1;
                    if ((*prev) != NULL) {
                        int bearing = 10 * (int)((*prev)->bearing(*i) / 10.0);
                        char deg[10];
                        char km[10];
#if defined(_WIN32) && (_MSC_VER >= 1400)
                        sprintf_s(deg, 9, "%03d", bearing);
                        sprintf_s(km, 9, "%05d", (int)(1e-3 * d));
#else
                        sprintf(deg, "%03d", bearing);
                        sprintf(km, "%05d", (int)(1e-3 * d));
#endif
                        js << "  var kmOpts" << km<< " = {title:\"" << (int) (1e-3 * d) << " km\", icon:arrow" << deg << "}; "
                            << "map.addOverlay(createMarker('" << (int) (1e-3 * d) <<" km', " << setprecision(12) << (*i)->latitude() << ", " << (*i)->longitude() << ", " << setprecision(4) << (*i)->elevation() << ", kmOpts" << km << "));" << endl;
                    }
                    d += 1e3 * kmlKmTicks;
                }
            }
        }
        js << "}" << endl;
        js.close();
        if (verbose > 0 && !quiet)
            cout << " OK" << endl;
    }
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma endregion
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma region Aufrufen von gnuplot
#endif
    /////////////////////////////////////////////////////////////////
    // Aufrufen von gnuplot
    /////////////////////////////////////////////////////////////////
    if (gnuplotExe != "")
    {
        string cmd = gnuplotExe + " " + gnuplotPltFile;
        if (verbose > 0 && !quiet)
            cout << "Ausführen von " << cmd << " ..";
        int rc = system(cmd.c_str());
        if (rc == -1) {
            errmsg("Ausführung von '" + cmd + "' fehlgeschlagen");
        }
        if (verbose > 0 && !quiet) {
            if (rc == 0)
                cout << " OK" << endl;
            else
                cout << " " << rc << endl;
        }
    }
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma endregion
#endif

    if (verbose > 0 && !quiet)
        cout << endl << "Fertig." << endl;

    return EXIT_SUCCESS;
}

