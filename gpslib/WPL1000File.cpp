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
    inline void readWPL1000TrackpointData(fstream& fs, T* data) 
    {
        fs.read(reinterpret_cast<char*>(data), sizeof(T));
    }


    int WPL1000Trackpoint::readFrom(fstream& fs)
    {
        readWPL1000TrackpointData<unsigned char>(fs, &_Type);
        readWPL1000TrackpointData<unsigned char>(fs, &_Unknown);
        readWPL1000TrackpointData<GPS::WPL1000Time>(fs, &_T);
        readWPL1000TrackpointData<unsigned int>(fs, &_WPL1000lat);
        readWPL1000TrackpointData<unsigned int>(fs, &_WPL1000lon);
        readWPL1000TrackpointData<unsigned short>(fs, &_WPL1000ele);
        if (_Type != CONTAINS_TRACKPOINT_DATA)
        {
            double lat = (double) _WPL1000lat / 10000000.0;
            double lon = (double) _WPL1000lon / 10000000.0;
            double ele = (double) _WPL1000ele;
            if (lat > 90)  { lat = 360 - lat; }
            setLatitude(lat);
            setLongitude(lon);
            setElevation(ele);
            setTimestamp(Timestamp(_T.year(), _T.month(), _T.day(), _T.hours(), _T.mins(), _T.secs()));
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
        if (nvpipe.bad())
            return EIO;
        nvpipe.seekg(0x00001000);
        if (nvpipe.eof())
            return EOF;
        _Trk = NULL;
        while (point.readFrom(nvpipe) == WPL1000Trackpoint::READ_OK && !nvpipe.eof())
        {
            if (_Trk == NULL)
                _Trk = new Track;
            if (!point.isNull())
            {
                Trackpoint* trkpt = new Trackpoint(point);
                if ((point.type() == WPL1000Trackpoint::TRACK_START) && (_Trk->points().size() > 0))
                {
                    addTrack(_Trk);
                    _Trk = NULL;
                }
                if (_Trk != NULL)
                    _Trk->append(trkpt);
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


    errno_t WPL1000File::write(const std::string& /* filename */, bool /* onlyKept */)
    {
        // it's futile to write a track to a NVPIPE.DAT file
        return 0;
    }
};
