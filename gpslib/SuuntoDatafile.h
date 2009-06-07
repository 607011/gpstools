// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#ifndef __SUUNTODATAFILE_H_
#define __SUUNTODATAFILE_H_

#include <string>
#include <map>

#include <errno.h>
#include <ctime>

#include "Trackpoint.h"
#include "Track.h"

namespace GPS {

    typedef std::map<std::string, std::string> var_t;

    /// \class SuuntoDatafile
    /// Klasse zum Verwalten von SDF-Dateien, wie sie der Suunto Training Manager
    /// erzeugt. 
    /// Implementiert ist derzeit nur das Lesen von Daten, und das auch nur von Daten
    /// der t6-Uhr.
    class SuuntoDatafile {
    private:
        /// Track
        Track* _Trk;

        /// Name der SDF-Datei
        const char* _Filename;

        /// Variablen nebst Werten im [HEADER]-Abschnitt der SDF-Datei
        var_t _Var;

    public:
        /// Konstruktor.
        /// @param filename Name der zu ladenden SDF-Datei.
        SuuntoDatafile(const char* filename = 0);

        /// Konstruktor.
        /// @param filename Name der zu ladenden SDF-Datei.
        SuuntoDatafile(const std::string& filename);

        /// SDF-Datei laden.
        /// @param filename Name der zu ladenden SDF-Datei.
        /// Wird kein Dateiname übergeben, wird der bereits in der Membervariablen
        /// _Filename gespeicherte verwendet. Ist auch dieser leer, kehrt die 
        /// Methode sofort zurück.
        /// @return 0, wenn keine Fehler aufgetreten sind, sonst Fehlercode
        int load(const char* filename = NULL);

        /// SDF-Datei laden.
        /// @param filename Name der zu ladenden SDF-Datei.
        /// Wird kein Dateiname übergeben, wird der bereits in der Membervariablen
        /// _Filename gespeicherte verwendet. Ist auch dieser leer, kehrt die 
        /// Methode sofort zurück.
        /// @return 0, wenn keine Fehler aufgetreten sind, sonst Fehlercode
        int load(const std::string& filename);

        /// Track zurückgeben.
        /// @see trk
        /// @return Track
        inline Track* track(void) { return _Trk; }
    };

};

#endif // __SUUNTODATAFILE_H_
