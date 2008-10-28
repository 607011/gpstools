// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <fstream>

#include "globals.h"

using namespace std;


void writeGnuplotData(void) 
{
    gnuplotElevationPct /= 100;
    gnuplotHeartratePct /= 100;
    gnuplotSpeedPct     /= 100;
    gnuplotSlopePct     /= 100;
    double gnuplotElevationOrigin = gnuplotSlopePct + gnuplotSpeedPct + gnuplotHeartratePct;
    double gnuplotHeartrateOrigin = gnuplotSlopePct + gnuplotSpeedPct;
    double gnuplotSpeedOrigin     = gnuplotSlopePct;
    double gnuplotSlopeOrigin     = 0.0;
    if (gnuplotPadding)
    {
        gnuplotHeartratePct /= 1.1;
        gnuplotSpeedPct     /= 1.1;
        gnuplotSlopePct     /= 1.1;
    }
    string outputFile(title);
    outputFile += "." + gnuplotSuffix;
    if (verbose > 0 && !quiet)
        cout << endl << _("Schreiben der gnuplot-Steueranweisungen nach ") << gnuplotPltFile << " ..";
    int xcolumn = 1;
    string xlabel;
    string xformat;
    fstream gnuplotStream(gnuplotPltFile.c_str(), fstream::out);
    if (gnuplotStream.bad())
        errmsg(string(_("Fehler beim Öffnen der Gnuplot-Steuer-Datei")) + " '" + gnuplotPltFile + "'");

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
        errmsg(string(_("Nicht unterstützte X-Achsen-Skalierung")) + ": " + gnuplotXAxis);
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
            gnuplotStream << trk->avgHeartrate().value() << " notitle lt 5 lc rgb \"#CAB0B0\", ";
        gnuplotStream << "\"" << gnuplotDatFile << "\" using " << xcolumn << ":3 notitle with lines lc rgb \"#00cc00\"" << endl
            << "set ytics autofreq" << endl;
    }

    if (trk->hasSpeed() && gnuplotSpeedPct > 0)
    {
        string speedSourceFile = string((gnuplotSpeedSource == "")? "Original" : gnuplotSpeedSource) + ".dat";
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
        gnuplotStream << "\"" << speedSourceFile << "\" using " << xcolumn << ":5 notitle with steps lc rgb \"#ff4433\"" << endl;
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
            << "set style fill solid 1.0" << endl
            << "plot \"" << slopeSourceFile << "\" using " << xcolumn << ":8 notitle with boxes lc rgb \"#0000ff\"" << endl;
    }

    gnuplotStream << "unset multiplot" << endl;
    gnuplotStream.close();
    if (verbose > 0 && !quiet)
        cout << " OK" << endl;


    /////////////////////////////////////////////////////////////////
    // Schreiben der gnuplot-Daten
    /////////////////////////////////////////////////////////////////
    for (smoothedTrack_t::const_iterator j = smoothedTrack.begin(); j != smoothedTrack.end(); ++j)
    {
        if (verbose > 0 && !quiet)
            cout << _("Schreiben der Gnuplot-Daten-Datei") << " '" << (*j).first << "' ..";
        GPS::Track* t = (*j).second;
        gnuplotDatFile = string((*j).first) + string(".dat");
        fstream dat(gnuplotDatFile.c_str(), fstream::out);
        if (!dat.good())
            errmsg(string(_("Fehler beim Öffnen der Gnuplot-Daten-Datei")) + " '" + gnuplotDatFile + "'");
        // 1:dist 2:ele 3:hr 4:temp 5:speed 6:pace 7:pace2 8:slope 9:lon 10:lat 11:timestamp 12:duration
        GPS::TrackpointList& samples = t->points();
        for (GPS::TrackpointList::const_iterator i = samples.begin(); i != samples.end(); ++i)
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
}


void executeGnuplot(void)
{
    if (gnuplotExe != "")
    {
        std::string cmd = gnuplotExe + " " + gnuplotPltFile;
        if (verbose > 0 && !quiet)
            cout << _("Ausführen von") << " " << cmd << " ..";
        int rc = system(cmd.c_str());
        if (rc == -1) {
            errmsg(string(_("Ausführung von")) + " '" + cmd + "' " + string(_("fehlgeschlagen")));
        }
        if (verbose > 0 && !quiet) {
            if (rc == 0)
                cout << " OK" << endl;
            else
                cout << " " << rc << endl;
        }
    }
}
