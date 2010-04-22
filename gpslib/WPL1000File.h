// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#ifndef __WPL1000FILE_H_
#define __WPL1000FILE_H_

#include "portable.h"
#include "helper.h"
#include "GPSTrackFile.h"

#include <sys/types.h>

#include <cstdlib>
#include <cerrno>
#include <ctime>
#include <string>


namespace GPS {

    struct WPL1000Time {
        union _wpl1000time
        {
            uint32_t tval;
#if BYTE_ORDER == BIG_ENDIAN
            struct _datetime_big_endian
            {
                unsigned int y:6; // year (-2000)
                unsigned int m:4; // month
                unsigned int d:5; // day of month
                unsigned int h:5; // hours
                unsigned int i:6; // mins
                unsigned int s:6; // secs
            } t;
#else
            struct _datetime_little_endian
            {
                unsigned int s:6; // secs
                unsigned int i:6; // mins
                unsigned int h:5; // hours
                unsigned int d:5; // day of month
                unsigned int m:4; // month
                unsigned int y:6; // year (-2000)
            } t;
#endif
        } t ;
        WPL1000Time(void)
        {
            t.tval = 0;
        }
        inline unsigned int secs(void)  { return t.t.s; }
        inline unsigned int mins(void)  { return t.t.i; }
        inline unsigned int hours(void) { return t.t.h; }
        inline unsigned int day(void)   { return t.t.d; }
        inline unsigned int month(void) { return t.t.m; }
        inline unsigned int year(void)  { return t.t.y + 2000; }
    };


    class WPL1000Data : public Trackpoint {
    public:
        enum _PointType
        {
            WPL1000_TRACKPOINT        = 0x00,
            WPL1000_TRACK_START       = 0x01,
            WPL1000_WAYPOINT          = 0x02,
            WPL1000_UNKNOWN_POINTTYPE = 0x04,
            WPL1000_END_OF_LOG        = 0xff,
            WPL1000_ERROR             = -1
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
