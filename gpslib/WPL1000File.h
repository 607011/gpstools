// $Id: WPL1000File.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __WPL1000FILE_H_
#define __WPL1000FILE_H_

#include <errno.h>
#include <ctime>
#include <string>

#include "GPSTrackFile.h"

#include <sys/types.h>

namespace GPS {

    struct WPL1000Time {
        unsigned int s:6;
        unsigned int i:6;
        unsigned int h:5;
        unsigned int d:5;
        unsigned int m:4;
        unsigned int Y:6;
    };


    class WPL1000Trackpoint : public Trackpoint {
    private:
        unsigned char _Type;
        unsigned char _Unknown;
        unsigned int _WPL1000lat;
        unsigned int _WPL1000lon;
        unsigned short _WPL1000ele;
        WPL1000Time _T;
    public:
        WPL1000Trackpoint(void) : Trackpoint()
        { /* ... */ }
        WPL1000Trackpoint(const WPL1000Trackpoint& other) : Trackpoint(other)
        { /* ... */ }
        inline unsigned char type(void) const { return _Type; }
        int readFrom(std::fstream& fs);
        enum _ReadStatus {
            READ_ERROR = -1,
            READ_OK    = 0
        };
        enum _PointType {
            TRACK_START = 0x01,
            CONTAINS_TRACKPOINT_DATA = 0xff
        };
    };


    class WPL1000File : public GPSTrackFile {
    public:
        /// Konstruktor.
        /// @param filename Name der Datei, aus der geladen werden soll.
        WPL1000File(const char* filename = NULL);

        /// Konstruktor.
        /// @param filename Name der Datei, aus der geladen werden soll.
        WPL1000File(const std::string& filename);

        /// Destruktor.
        ~WPL1000File() { /* ... */ }

        /// GPS-Datei laden.
        /// @param filename Name der Datei, aus der geladen werden soll.
        errno_t load(const std::string& filename);

        /// GPS-Datei speichern.
        /// @param filename Name der Datei, in die gespeichert werden soll.
        /// @param onlyKept nur Trackpoints schreiben, die als _Keep markiert sind.
        errno_t write(const std::string& filename, bool onlyKept = false);
    };

};

#endif // __WPL1000FILE_H_
