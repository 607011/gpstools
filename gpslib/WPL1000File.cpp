// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <fstream>

#ifdef _DEBUG
#include <iostream>
#include <iomanip>
#endif // _DEBUG

#include "helper.h"
#include "WPL1000File.h"


using namespace std;


namespace GPS {

    WPL1000File::WPL1000File(const char* filename) : GPSTrackFile(filename)
    { /* ... */ }


    WPL1000File::WPL1000File(const string& filename) : GPSTrackFile(filename)
    { /* ... */ }


    inline void swapBytes(char& a, char& b)
    {
        char t = b;
        b = a;
        a = t;
    }


#ifdef _DEBUG
  inline std::string toHex(unsigned char c)
  {
    char buf[3];
    static const char digits[17] = "0123456789abcdef";
    buf[2] = 0;
    buf[1] = digits[c & 0x0f];
    buf[0] = digits[c >> 4];
    return std::string(buf);
  }

  inline std::string toBin(unsigned char c)
  {
    char buf[9];
    for (int i = 7; i >= 0; --i) {
      buf[i] = (c & 1) + '0';
      c >>= 1;
    }
    buf[8] = 0;
    return std::string(buf);
  }
#endif // _DEBUG


    template <typename T>
    inline void readField(fstream& fs, T* data) 
    {
        char* d = reinterpret_cast<char*>(data);
        fs.read(d, sizeof(T));
        if (is_bigendian())
        {
            switch (sizeof(T))
            {
            case 4:
                swapBytes(d[0], d[3]);
                swapBytes(d[1], d[2]);
#ifdef _DEBUG
		cout << toHex(d[0]) << " " << toHex(d[1]) << " " << toHex(d[2]) << " " << toHex(d[3]) << "  ";
		cout << toBin(d[0]) << " " << toBin(d[1]) << " " << toBin(d[2]) << " " << toBin(d[3]) << endl;
#endif // _DEBUG
                break;
            case 2:
	        swapBytes(d[0], d[1]);
                break;
            case 1:
	        break;
            default:
                break;
            }
        }
    }


    int WPL1000Data::readFrom(fstream& fs)
    {
        // Reverse Engineering mit freundlicher Unterst�tzung von Eckhard Zemp, Berlin (www.zemp.ch)
        readField<uint8_t>(fs, &_Type);
        readField<uint8_t>(fs, &_Unknown);
        readField<GPS::WPL1000Time>(fs, &_T);
        readField<int32_t>(fs, &_WPL1000lat);
        readField<int32_t>(fs, &_WPL1000lon);
        readField<int16_t>(fs, &_WPL1000ele);
        Timestamp ts(_T.year(), _T.month(), _T.day(), _T.hours(), _T.mins(), _T.secs());
        switch (_Type)
        {
        case WAYPOINT:
            setName(ts.toString());
            /* fall-through */
        case TRACKPOINT:
            /* fall-through */
        case TRACK_START:
            {
                setLatitude(1e-7 * (double) _WPL1000lat);
                setLongitude(1e-7 * (double) _WPL1000lon);
                setElevation((double) _WPL1000ele);
                setTimestamp(ts);
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
                break;
            case WPL1000Data::WAYPOINT:
                if (!point.isNull())
                {
                    Waypoint* wpt = new Waypoint(point);
                    addWaypoint(wpt);
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


};
