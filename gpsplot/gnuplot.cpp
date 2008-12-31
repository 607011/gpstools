// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <fstream>

#include "globals.h"


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
    std::string outputFile((title.size() > 0)? title : "profile");
    outputFile += "." + gnuplotSuffix;
    if (verbose > 0 && !quiet)
        std::cout << std::endl << _("Schreiben der gnuplot-Steueranweisungen nach ") << gnuplotPltFile << " ..";
    int xcolumn = 1;
    std::string xlabel;
    std::string xformat;
    std::fstream gnuplotStream(gnuplotPltFile.c_str(), std::fstream::out);
    if (gnuplotStream.bad())
        errmsg(std::string(_("Fehler beim Öffnen der Gnuplot-Steuer-Datei")) + " '" + gnuplotPltFile + "'");

    gnuplotStream << "reset" << std::endl
        << "set terminal " << gnuplotFormat << std::endl
        << "set output \"" << outputFile << "\"" << std::endl
        << "set yrange [*:*]" << std::endl
        << "set grid" << std::endl
        << "set lmargin 9" << std::endl
        << "set rmargin 2" << std::endl
        << "set multiplot" << std::endl
        << "set datafile missing \"" << MISSING << "\"" << std::endl
        << "set ylabel \"Höhe [m]\" offset 1" << std::endl;
    if (gnuplotXAxis == "distance") {
        gnuplotStream << "set xrange [0:" << 1e-3 * trk->distance() << "]" << std::endl;
        xcolumn = 1;
        xlabel  = "Distanz [km]";
        xformat = "";
    }
    else if (gnuplotXAxis == "duration") {
        gnuplotStream << "set timefmt \"%H:%M:%S\"" << std::endl
            << "set xdata time" << std::endl
            << "set xrange [*:\"" << secs2timestamp(trk->duration()) << "\"]" << std::endl;
        xcolumn = 11;
        xlabel  = "Dauer";
        xformat = "%H:%M";
    }
    else if (gnuplotXAxis == "datetime") {
        gnuplotStream << "set timefmt \"%Y-%m-%d %H:%M:%S\"" << std::endl
            << "set xdata time" << std::endl
            << "set xrange [\"" << trk->startTimestamp().toString("%Y-%m-%d %H:%M:%S") << "\":\"" << trk->finishTimestamp().toString("%Y-%m-%d %H:%M:%S") << "\"]" << std::endl;
        title += " ("  + trk->startTimestamp().toString("%#d.%#m.%Y %#H:%M") + ")";
        xcolumn = 12;
        xlabel  = "Datum/Uhrzeit";
        xformat = "%H:%M";
    }
    else {
        errmsg(std::string(_("Nicht unterstützte X-Achsen-Skalierung")) + ": " + gnuplotXAxis);
    }
    if (title.size() > 0)
        gnuplotStream << "set title \"" << title << "\"" << std::endl;
    else
        gnuplotStream << "unset title" << std::endl;
    if (gnuplotElevationLo.defined() && gnuplotElevationHi.defined())
        gnuplotStream << "set yrange [" << gnuplotElevationLo.value() << ":" << gnuplotElevationHi.value() << "]" << std::endl;
    if ((trk->hasHeartrate() && gnuplotHeartratePct > 0) ||
        (trk->hasSpeed() && gnuplotSpeedPct > 0) ||
        (trk->hasElevation() && gnuplotSlopePct > 0))
    {
        gnuplotStream << "set format x \"\"" << std::endl
            << "unset xlabel" << std::endl
            << "set bmargin 0" << std::endl;
    }
    else
    {
        gnuplotStream << "set format x" << std::endl
            << "set xlabel \"" << xlabel << "\"" << std::endl
            << "set bmargin" << std::endl;
    }
    gnuplotStream << "set size 1.0, " << gnuplotElevationPct << std::endl
        << "set origin 0, " << gnuplotElevationOrigin << std::endl;

    gnuplotDatFile = smoothings.front()->id + ".dat";
    gnuplotStream << "plot ";
    for (std::vector<SmoothingOptions*>::const_iterator i = smoothings.begin(); i != smoothings.end(); ++i)
    {
        if ((*i)->draw) {
            if (i != smoothings.begin())
                gnuplotStream << ", ";
            gnuplotStream << "\"" << (*i)->id << ".dat" << "\" "
                << "using " << xcolumn << ":2 "
                << (gnuplotLegend? "title \"" + std::string((*i)->id) + "\"" : "notitle")
                << " " << (*i)->gnuplotOption;
        }
    }
    gnuplotStream << std::endl;

    if (trk->hasHeartrate() && gnuplotHeartratePct > 0)
    {
        if ((trk->hasSpeed() && gnuplotSpeedPct > 0) ||
            (trk->hasElevation() && gnuplotSlopePct > 0))
        {
            gnuplotStream << "set format x \"\"" << std::endl
                << "unset xlabel" << std::endl
                << "set bmargin 0" << std::endl;
        }
        else
        {
            gnuplotStream << "set xlabel '" << xlabel << "'" << std::endl
                << "set format x" << std::endl
                << "set bmargin" << std::endl;
        }
        if (gnuplotHeartrateLo.defined() && gnuplotHeartrateHi.defined())
            gnuplotStream << "set yrange [" << gnuplotHeartrateLo.value() << ":" << gnuplotHeartrateHi.value() << "]" << std::endl;
        gnuplotStream << "unset title" << std::endl
            << "set size 1.0, " << gnuplotHeartratePct << std::endl
            << "set origin 0, " << gnuplotHeartrateOrigin << std::endl
            << "set ytics (40, 60, 80, 100, 120, 140, 160, 180, 200, 220)" << std::endl
            << "set ylabel 'HR' offset 1" << std::endl
            << "set tmargin 0" << std::endl;
        gnuplotStream << "plot ";
        if (gnuplotHeartrateAverage && trk->avgHeartrate().defined())
            gnuplotStream << trk->avgHeartrate().value() << " notitle lt 5 lc rgb \"#CAB0B0\", ";
        gnuplotStream << "\"" << gnuplotDatFile << "\" using " << xcolumn << ":3 notitle with lines lc rgb \"#00cc00\"" << std::endl
            << "set ytics autofreq" << std::endl;
    }

    if (trk->hasSpeed() && gnuplotSpeedPct > 0)
    {
        std::string speedSourceFile = std::string((gnuplotSpeedSource == "")? "Original" : gnuplotSpeedSource) + ".dat";
        if (trk->hasElevation() && gnuplotSlopePct > 0)
        {
            gnuplotStream << "set format x \"\"" << std::endl
                << "unset xlabel" << std::endl
                << "set bmargin 0" << std::endl;
        }
        else
        {
            gnuplotStream << "set xlabel \"" << xlabel << "\"" << std::endl
                << "set format x" << std::endl
                << "set bmargin" << std::endl;
        }
        if (gnuplotSpeedLo.defined() && gnuplotSpeedHi.defined())
            gnuplotStream << "set yrange [" << gnuplotSpeedLo.value() << ":" << gnuplotSpeedHi.value() << "]" << std::endl;
        else
            gnuplotStream << "set yrange [*:*]" << std::endl;
        gnuplotStream << "unset title" << std::endl
            << "set size 1.0, " << gnuplotSpeedPct << std::endl
            << "set origin 0, " << gnuplotSpeedOrigin << std::endl
            << "set tmargin 0" << std::endl
            << "set ylabel 'km/h' offset 1" << std::endl
            << "plot ";
        if (gnuplotSpeedAverage && trk->avgSpeed().defined())
            gnuplotStream << trk->avgSpeed().value() << " notitle lt 5 linecolor rgb \"#AACDA2\", ";
        gnuplotStream << "\"" << speedSourceFile << "\" using " << xcolumn << ":5 notitle with steps lc rgb \"#ff4433\"" << std::endl;
    }

    if (trk->hasElevation() && trk->hasDistance() && gnuplotSlopePct > 0)
    {
        std::string slopeSourceFile = std::string((gnuplotSlopeSource == "")? "Original" : gnuplotSlopeSource) + ".dat";
        if (gnuplotSlopeLo.defined() && gnuplotSlopeHi.defined())
            gnuplotStream << "set yrange [" << gnuplotSlopeLo << ":" << gnuplotSlopeHi << "]" << std::endl;
        else
            gnuplotStream << "set yrange [*:*]" << std::endl;
        gnuplotStream << "unset title" << std::endl
            << "set size 1.0, " << gnuplotSlopePct << std::endl
            << "set origin 0, " << gnuplotSlopeOrigin << std::endl
            << "set bmargin" << std::endl
            << "set tmargin 0" << std::endl
            << "set ylabel \"%\" offset 1" << std::endl
            << "set xlabel \"" << xlabel << "\"" << std::endl
            << "set format x" << std::endl
            << "set style fill solid 1.0" << std::endl
            << "plot \"" << slopeSourceFile << "\" using " << xcolumn << ":8 notitle with boxes lc rgb \"#0000ff\"" << std::endl;
    }

    gnuplotStream << "unset multiplot" << std::endl;
    gnuplotStream.close();
    if (verbose > 0 && !quiet)
        std::cout << " OK" << std::endl;


    /////////////////////////////////////////////////////////////////
    // Schreiben der gnuplot-Daten
    /////////////////////////////////////////////////////////////////
    for (smoothedTrack_t::const_iterator j = smoothedTrack.begin(); j != smoothedTrack.end(); ++j)
    {
        if (verbose > 0 && !quiet)
            std::cout << _("Schreiben der Gnuplot-Daten-Datei") << " '" << (*j).first << "' ..";
        GPS::Track* t = (*j).second;
        gnuplotDatFile = std::string((*j).first) + std::string(".dat");
        std::fstream dat(gnuplotDatFile.c_str(), std::fstream::out);
        if (!dat.good())
            errmsg(std::string(_("Fehler beim Öffnen der Gnuplot-Daten-Datei")) + " '" + gnuplotDatFile + "'");
        // 1:dist 2:ele 3:hr 4:temp 5:speed 6:pace 7:pace2 8:slope 9:lon 10:lat 11:timestamp 12:duration
        GPS::TrackpointList& samples = t->points();
        for (GPS::TrackpointList::const_iterator i = samples.begin(); i != samples.end(); ++i)
        {
            dat << std::setprecision(5)
                << 1e-3 * (*i)->distance() << " ";
            if ((*i)->elevation().defined())
                dat << std::setprecision(4) << (*i)->elevation() << " ";
            else
                dat << "? ";
            if ((*i)->heartrate().defined())
                dat << (*i)->heartrate() << " ";
            else
                dat << "? ";
            if ((*i)->temperature().defined())
                dat << std::setprecision(3) << (*i)->temperature() << " ";
            else
                dat << "? ";
            dat << std::setprecision(4)
                << (*i)->speed() << " "
                << (*i)->pace() << " "
                << (*i)->paceString() << " "
                << (*i)->slope() << " "
                << std::setprecision(10)
                << (*i)->longitude() << " "
                << (*i)->latitude() << " "
                << secs2timestamp((*i)->duration()) << " "
                << (*i)->timestamp().toString("%Y-%m-%d %H:%M:%S") << " "
                << std::endl;
        }
        dat.close();
        if (verbose > 0 && !quiet)
            std::cout << " OK" << std::endl;
    }
}


void executeGnuplot(void)
{
    if (gnuplotExe != "")
    {
        std::string cmd = gnuplotExe + " " + gnuplotPltFile;
        if (verbose > 0 && !quiet)
            std::cout << _("Ausführen von") << " " << cmd << " ..";
        int rc = system(cmd.c_str());
        if (rc == -1) {
            errmsg(std::string(_("Ausführung von")) + " '" + cmd + "' " + std::string(_("fehlgeschlagen")));
        }
        if (verbose > 0 && !quiet) {
            if (rc == 0)
                std::cout << " OK" << std::endl;
            else
                std::cout << " " << rc << std::endl;
        }
    }
}
