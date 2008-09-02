// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <fstream>

#include "WPL1000File.h"

using namespace std;

namespace GPS {

    WPL1000File::WPL1000File(const char* filename) : GPSTrackFile(filename)
    { /* ... */ }


    WPL1000File::WPL1000File(const string& filename) : GPSTrackFile(filename)
    { /* ... */ }


#define readWPL1000TrackpointData(_Stream, _W) _Stream.read(reinterpret_cast<char*>(&_W), sizeof(_W))

    int WPL1000Trackpoint::readFrom(fstream& fs)
    {
        readWPL1000TrackpointData(fs, _Type);
        readWPL1000TrackpointData(fs, _Unknown);
        readWPL1000TrackpointData(fs, _T);
        readWPL1000TrackpointData(fs, _WPL1000lat);
        readWPL1000TrackpointData(fs, _WPL1000lon);
        readWPL1000TrackpointData(fs, _WPL1000ele);
        if (_Type != CONTAINS_TRACKPOINT_DATA)
        {
            setLatitude((double) _WPL1000lat / 10000000.0f);
            setLongitude((double) _WPL1000lon / 10000000.0f);
            setElevation((double) _WPL1000ele);
            setTimestamp(Timestamp(2000 + _T.Y, _T.m, _T.d, _T.h, _T.i, _T.s));
            return READ_OK;
        }
        return READ_ERROR;
    }


    errno_t WPL1000File::load(const std::string& filename)
    {
        if (filename != "")
            _Filename = filename;
        WPL1000Trackpoint point;
        fstream nvpipe;
        nvpipe.open(_Filename.c_str(), ios::binary | ios::in);
        nvpipe.seekg(0x00001000);
        _Trk = new Track;
        while (point.readFrom(nvpipe) == WPL1000Trackpoint::READ_OK)
        {
            Trackpoint* trkpt = new Trackpoint(point);
            if ((point.type() == WPL1000Trackpoint::TRACK_START) && (_Trk->points().size() > 0))
            {
                _Trk->postProcess();
                addTrack(_Trk);
            }
            _Trk->append(trkpt);
        }
        nvpipe.close();
        _Trk->postProcess();
        return 0;
    }


    errno_t WPL1000File::write(const std::string& /* filename */, bool /* onlyKept */)
    {
        // do nothing
        return 0;
    }
};
