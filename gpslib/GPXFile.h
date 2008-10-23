// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __GPXFILE_H_
#define __GPXFILE_H_

#include <errno.h>
#include <ctime>
#include <string>

#include "Waypoint.h"
#include "GPSTrackFile.h"
#include "Track.h"

#include "tinyxml/tinyxml.h"

namespace GPS {

    /// Die Klasse GPXFile liest/schreibt Track aus/in GPX-Dateien.
    class GPXFile : public GPSTrackFile {
    private:
        TiXmlDocument gpx;

    public:
        /// Konstruktor.
        GPXFile(const char* filename = NULL);

        /// Konstruktor.
        GPXFile(const std::string& filename);

        /// Destruktor.
        ~GPXFile() { /* ... */ }

        /// GPS-Datei laden.
        /// @param filename Name der Datei, aus der geladen werden soll.
        /// @return Fehlercode: 0, wenn alles OK
        errno_t load(const std::string& filename = std::string());

        /// GPS-Datei laden.
        /// @param filename Name der Datei, aus der geladen werden soll.
        /// @return Fehlercode: 0, wenn alles OK
        errno_t load(const char* filename = NULL);

        /// Track in angegebene GPS-Datei schreiben.
        /// @param filename Name der Datei, in die geschrieben werden soll.
        /// @param onlyKept nur Trackpoints schreiben, die als _Keep markiert sind.
        /// @return Fehlercode: 0, wenn alles OK
        errno_t write(const std::string& filename = std::string(), bool onlyKept = false);

        /// Aus der Liste der eingelesenen Tracks denjenigen mit dem
        /// angegebenen Namen (trk/name) herauspicken.
        /// @param name Name des Tracks
        /// @return Track
        Track* trackByName(const std::string& name);

        /// Aus der Liste der eingelesenen Tracks denjenigen mit der
        /// angegebenen Nummer (trk/number) herauspicken.
        /// @param name Nummer des Tracks
        /// @return Track
        Track* trackByNumber(unsigned int number);

    private:
        static void getWptType(Waypoint* wpt, TiXmlNode* gpxWpt);
    };

};

#endif // __GPXFILE_H_
