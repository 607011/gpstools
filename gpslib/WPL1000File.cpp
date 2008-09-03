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


    template <typename T>
    inline bool readWPL1000TrackpointData(fstream& fs, T data)
    {
        fs.read(reinterpret_cast<char*>(&data), sizeof(T));
        return fs.eof();
    }


    int WPL1000Trackpoint::readFrom(fstream& fs)
    {
        bool eof;
        eof = readWPL1000TrackpointData(fs, _Type);
        if (eof) return READ_ERROR;
        eof = readWPL1000TrackpointData(fs, _Unknown);
        if (eof) return READ_ERROR;
        eof = readWPL1000TrackpointData(fs, _T);
        if (eof) return READ_ERROR;
        eof = readWPL1000TrackpointData(fs, _WPL1000lat);
        if (eof) return READ_ERROR;
        eof = readWPL1000TrackpointData(fs, _WPL1000lon);
        if (eof) return READ_ERROR;
        eof = readWPL1000TrackpointData(fs, _WPL1000ele);
        if (eof) return READ_ERROR;
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
