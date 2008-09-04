// $Id$
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
        union _wpl1000time
        {
            struct _bitfield
            {
                unsigned int s:6;
                unsigned int i:6;
                unsigned int h:5;
                unsigned int d:5;
                unsigned int m:4;
                unsigned int Y:6;
            } t;
            unsigned int tval;
        } t ;
        WPL1000Time(void)
        {
            t.tval = 0;
        }
        inline unsigned int secs(void) const { return t.t.s; }
        inline unsigned int mins(void) const { return t.t.i; }
        inline unsigned int hours(void) const { return t.t.h; }
        inline unsigned int day(void) const { return t.t.d; }
        inline unsigned int month(void) const { return t.t.m; }
        inline unsigned int year(void) const { return t.t.Y + 2000; }
    };


    class WPL1000Data : public Trackpoint {
    private:
        unsigned char _Type;
        unsigned char _Unknown;
        int _WPL1000lat;
        int _WPL1000lon;
        short _WPL1000ele;
        WPL1000Time _T;
    public:
        WPL1000Data(void) : Trackpoint(), _Type(0), _Unknown(0), _WPL1000lat(0), _WPL1000lon(0), _WPL1000ele(0)
        { /* ... */ }
        WPL1000Data(const WPL1000Data& other) : Trackpoint(other)
        { /* ... */ }
        inline unsigned char type(void) const { return _Type; }
        inline bool isNull(void) const { return _T.t.tval == 0; }
        int readFrom(std::fstream& fs);
        enum _PointType {
            TRACKPOINT = 0x00,
            TRACK_START = 0x01,
            WAYPOINT = 0x02,
            END_OF_LOG = 0xff,
            ERROR = -1
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
