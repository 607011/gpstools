// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __WPL1000FILE_H_
#define __WPL1000FILE_H_

#include <cstdlib>
#include <cerrno>
#include <ctime>
#include <string>

#include "helper.h"
#include "GPSTrackFile.h"

#include <sys/types.h>

namespace GPS {

    struct WPL1000Time {
        union _wpl1000time
        {
            struct _datetime_little_endian
            {
                unsigned int s:6; // secs
                unsigned int i:6; // mins
                unsigned int h:5; // hours
                unsigned int d:5; // day of month
                unsigned int m:4; // month
                unsigned int Y:6; // year (-2000)
            } little;
            struct _datetime_big_endian
            {
                unsigned int Y:6; // year (-2000)
                unsigned int m:4; // month
                unsigned int d:5; // day of month
                unsigned int h:5; // hours
                unsigned int i:6; // mins
                unsigned int s:6; // secs
            } big;
            uint32_t tval;
        } t ;
        WPL1000Time(void)
        {
            t.tval = 0;
        }
    };


    class WPL1000Data : public Trackpoint {
    public:
        enum _PointType
        {
            TRACKPOINT = 0x00,
            TRACK_START = 0x01,
            WAYPOINT = 0x02,
            END_OF_LOG = 0xff,
            ERROR = -1
        };
    private:
        uint8_t _Type;
        uint8_t _Unknown;
        WPL1000Time _T;
        int32_t _WPL1000lat;
        int32_t _WPL1000lon;
        int16_t _WPL1000ele;
    public:
        WPL1000Data(void) : Trackpoint(), _Type(0), _Unknown(0), _WPL1000lat(0), _WPL1000lon(0), _WPL1000ele(0)
        { /* ... */ }
        WPL1000Data(const WPL1000Data& other) : Trackpoint(other)
        { /* ... */ }
        inline uint8_t type(void) const { return _Type; }
        inline uint8_t unknown(void) const { return _Unknown; }
        inline int32_t lat(void) const { return _WPL1000lat; }
        inline int32_t lon(void) const { return _WPL1000lon; }
        inline int32_t ele(void) const { return _WPL1000ele; }
        inline const WPL1000Time& time(void) const { return _T; }
        inline bool isNull(void) const { return _T.t.tval == 0; }
        int readFrom(std::fstream& fs);
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
        inline errno_t write(const std::string& filename, bool onlyKept = false)
        {
            UNUSED(filename);
            UNUSED(onlyKept);
            // it's futile to write a track to an NVPIPE.DAT file
            return -1;
        }
    };

};

#endif // __WPL1000FILE_H_
