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
    inline void readField(fstream& fs, T* data) 
    {
        fs.read(reinterpret_cast<char*>(data), sizeof(T));
    }


    int WPL1000Data::readFrom(fstream& fs)
    {
        readField<unsigned char>(fs, &_Type);
        readField<unsigned char>(fs, &_Unknown);
        readField<GPS::WPL1000Time>(fs, &_T);
        readField<int>(fs, &_WPL1000lat);
        readField<int>(fs, &_WPL1000lon);
        readField<short>(fs, &_WPL1000ele);
        switch (_Type)
        {
        case WAYPOINT:
            /* fall-through */
        case TRACKPOINT:
            /* fall-through */
        case TRACK_START:
            {
                setLatitude(1e-7 * (double) _WPL1000lat);
                setLongitude(1e-7 * (double) _WPL1000lon);
                setElevation((double) _WPL1000ele);
                setTimestamp(Timestamp(_T.year(), _T.month(), _T.day(), _T.hours(), _T.mins(), _T.secs()));
            }
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
        if (nvpipe.bad())
            return EIO;
        nvpipe.seekg(0x00001000);
        if (nvpipe.eof())
            return EOF;
        _Trk = NULL;
        while (!nvpipe.eof())
        {
            int rc = point.readFrom(nvpipe);
            if (rc == WPL1000Data::END_OF_LOG)
            {
                if (_Trk != NULL && _Trk->points().size() > 0)
                    addTrack(_Trk);
                break;
            }
            switch (rc)
            {
            case WPL1000Data::TRACK_START:
                /* fall-through */
            case WPL1000Data::TRACKPOINT:
                {
                    if (_Trk == NULL)
                        _Trk = new Track;
                    if (!point.isNull())
                    {
                        Trackpoint* trkpt = new Trackpoint(point);
                        if ((point.type() == WPL1000Data::TRACK_START) && (_Trk->points().size() > 0))
                        {
                            addTrack(_Trk);
                            _Trk = NULL;
                        }
                        if (_Trk != NULL)
                            _Trk->append(trkpt);
                    }
                }
                break;
            case WPL1000Data::WAYPOINT:
                {
                    if (!point.isNull())
                    {
                        Waypoint* wpt = new Waypoint(point);
                        addWaypoint(wpt);
                    }
                }
                break;
            default:
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


    errno_t WPL1000File::write(const std::string& /* filename */, bool /* onlyKept */)
    {
        // it's futile to write a track to a NVPIPE.DAT file
        return 0;
    }
};
