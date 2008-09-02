// $Id: GPXFile.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __GPXFILE_H_
#define __GPXFILE_H_

#include <errno.h>
#include <ctime>
#include <string>

#include "GPSTrackFile.h"

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
        errno_t load(const std::string& filename = std::string());

        /// GPS-Datei laden.
        /// @param filename Name der Datei, aus der geladen werden soll.
        errno_t load(const char* filename = NULL);

        /// Track in angegebene GPS-Datei schreiben.
        /// @param filename Name der Datei, in die geschrieben werden soll.
        /// @param onlyKept nur Trackpoints schreiben, die als _Keep markiert sind.
        errno_t write(const std::string& filename = std::string(), bool onlyKept = false);

    };

};

#endif // __GPXFILE_H_
