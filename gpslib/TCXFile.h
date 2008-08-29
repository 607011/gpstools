// $Id: TCXFile.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __TCXFILE_H_
#define __TCXFILE_H_

#include <errno.h>
#include <time.h>
#include <string>

#include "GPSTrackFile.h"

#include "tinyxml/tinyxml.h"


namespace GPS {

    /// Die Klasse GPXFile liest/schreibt Track aus/in GPX-Dateien.
    class TCXFile : public GPSTrackFile {
    private:
        TiXmlDocument tcx;

    public:
        /// Konstruktor.
        TCXFile(const char* filename = NULL);

        /// Konstruktor.
        TCXFile(const std::string& filename);

        /// Destruktor.
        ~TCXFile() { /* ... */ }

        /// TCX-Datei laden.
        /// @param filename Name der Datei, aus der geladen werden soll.
        errno_t load(const std::string& filename);

        /// Track in angegebene TCX-Datei schreiben.
        /// @param filename Name der Datei, in die geschrieben werden soll.
        /// @param onlyKept nur Trackpoints schreiben, die als _Keep markiert sind.
        errno_t write(const std::string& filename, bool onlyKept = false);

    };

};

#endif // __GPXFILE_H_
