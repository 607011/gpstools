// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include "portable.h"
#include "helper.h"
#include "WPL1000File.h"

#include <fstream>

#ifdef _DEBUG
#include <iostream>
#include <iomanip>
#endif


using namespace std;


namespace GPS {

    WPL1000File::WPL1000File(const char* filename) : GPSTrackFile(filename)
    { /* ... */ }


    WPL1000File::WPL1000File(const string& filename) : GPSTrackFile(filename)
    { /* ... */ }


    template <typename T>
    inline void readField(fstream& fs, T* data) 
    {
        char* d = reinterpret_cast<char*>(data);
        fs.read(d, sizeof(T));
#if BYTE_ORDER == BIG_ENDIAN
        switch (sizeof(T))
        {
        case 4:
            swap(d[0], d[3]);
            swap(d[1], d[2]);
            break;
        case 2:
            swap(d[0], d[1]);
            break;
        default:
            // ignore
            break;
        }
#endif
    }


    int WPL1000Data::readFrom(fstream& fs)
    {
        // Reverse Engineering mit freundlicher Unterstützung von Eckhard Zemp, Berlin (www.zemp.ch)
        // und Gerhard Mehldau, Los Angeles (Kalifornien)
        readField<uint8_t>(fs, &_Type);
        readField<uint8_t>(fs, &_Unknown);
        readField<GPS::WPL1000Time>(fs, &_T);
        readField<int32_t>(fs, &_WPL1000lat);
        readField<int32_t>(fs, &_WPL1000lon);
        readField<int16_t>(fs, &_WPL1000ele);
        Timestamp ts(_T.year(), _T.month(), _T.day(), _T.hours(), _T.mins(), _T.secs());
        switch (_Type)
        {
        case WPL1000_WAYPOINT:
            setName(ts.toString());
            // fall-through
        case WPL1000_TRACKPOINT:
            // fall-through
        case WPL1000_TRACK_START:
            setLatitude(1e-7 * (double) _WPL1000lat);
            setLongitude(1e-7 * (double) _WPL1000lon);
            setElevation((double) _WPL1000ele);
            setTimestamp(ts);
            break;
        default:
            break;
        }
        return _Type;
    }


    errno_t WPL1000File::load(const std::string& filename)
    {
        if (filename != "")
            _Filename = filename;
        WPL1000Data point;
        fstream nvpipe;
        nvpipe.open(_Filename.c_str(), ios::binary | ios::in);
        if (!nvpipe.good())
            return EIO;
        nvpipe.seekg(0x00001000);
        if (nvpipe.eof())
            return EOF;
        _Trk = NULL;
        while (!nvpipe.eof())
        {
            int rc = point.readFrom(nvpipe);
            if (rc == WPL1000Data::WPL1000_END_OF_LOG)
            {
                if (_Trk != NULL && _Trk->points().size() > 0)
                    addTrack(_Trk);
                break;
            }
            switch (rc)
            {
            case WPL1000Data::WPL1000_TRACK_START:
                // fall-through
            case WPL1000Data::WPL1000_TRACKPOINT:
                if (_Trk == NULL)
                    _Trk = new Track;
                if (!point.isNull())
                {
                    Trackpoint* trkpt = new Trackpoint(point);
                    if ((point.type() == WPL1000Data::WPL1000_TRACK_START) && (_Trk->points().size() > 0))
                    {
                        addTrack(_Trk);
                        _Trk = NULL;
                    }
                    if (_Trk != NULL)
                        _Trk->append(trkpt);
                }
                break;
            case WPL1000Data::WPL1000_WAYPOINT:
                if (!point.isNull())
                {
                    Waypoint* wpt = new Waypoint(point);
                    addWaypoint(wpt);
                }
                break;
            default:
                // ignore
                break;
            }
        }
        nvpipe.close();
        for (GPS::TrackList::iterator i = tracks().begin(); i != tracks().end(); ++i)
        {
            (*i)->setName((*i)->startTimestamp().toString());
            (*i)->postProcess();
        }
        return 0;
    }


};
