// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include <cstdlib>
#include <cstring>
#include <cassert>

#include <iostream>
#include <fstream>
#include <vector>

#include "SuuntoDatafile.h"
#include "Timestamp.h"

using namespace std;

namespace GPS {


    SuuntoDatafile::SuuntoDatafile(const char* filename)
    {
        _Trk = new Track;
        _Filename = filename;
    }


    SuuntoDatafile::SuuntoDatafile(const std::string& filename)
    {
        _Trk = new Track;
        _Filename = filename.c_str();
    }


    static void tokenize(const string& str, vector<string>& tokens, const string& delimiters)
    {
        string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        string::size_type pos     = str.find_first_of(delimiters, lastPos);
        while (string::npos != pos || string::npos != lastPos)
        {
            // Found a token, add it to the vector.
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            lastPos = str.find_first_not_of(delimiters, pos);
            pos = str.find_first_of(delimiters, lastPos);
        }
    }


    int SuuntoDatafile::load(const char* filename)
    {
        const size_t BUFSIZE = 512;
        char buf[BUFSIZE*2];

        if (filename != NULL)
            _Filename = filename;

        fstream sdf(_Filename, fstream::in);
        if (sdf.bad())
            return EIO;

        // head on to "[HEADER]"
        while (!sdf.eof())
        {
            sdf.getline(buf, BUFSIZE);
            char* res = strstr(buf, "[HEADER]");
            if ((int)(res - buf) == 0)
                break;
        }

        // read variables below "[HEADER]" until "[POINTS]" is reached
        while (!sdf.eof())
        {
            sdf.getline(buf, BUFSIZE);
            char* res = strstr(buf, "[POINTS]");
            if ((int)(res - buf) == 0)
                break;
            vector<string> var;
            tokenize(buf, var, "=");
            _Var[var.front()] = var.at(1);
        }
        if (_Var.find("NOTES") != _Var.end()) {
            _Trk->setName(_Var["NOTES"]);
        }
        else if (_Var.find("NAME") != _Var.end()) {
            _Trk->setName(_Var["NAME"]);
        }
        while (!sdf.eof())
        {
            // "T6LOG",11.08.2007,07:39.55,351,0.000,0,122,0.829,41,62,36.000,13.028874485783,0, 0 , 0.00,0.000
            // $action, $date, $time, $ele, $f0, $f1, $hr, $epoc, $breathfreq, $ventilation, $vo2, $kcal, $f8, $f9, $dist, $f11
            // 0        1      2      3     4    5    6    7      8            9             10     11    12   13   14     15
            sdf.getline(buf, BUFSIZE);
            char* res = strstr(buf, "[CUSTOM");
            if ((int)(res - buf) == 0)
                break;
            std::vector<string> fields;
            tokenize(buf, fields, ",");
            if (fields.size() < 15)
                continue;
            if (fields.front() != "\"T6LOG\"")
                continue;
            int day, month, year, hours, minutes, seconds;
            Timestamp timestamp;
            int rc_date, rc_time;
#if defined(_WIN32) && (_MSC_VER >= 1400)
            rc_date = sscanf_s(fields.at(1).c_str(), "%2d.%2d.%4d", &day, &month, &year);
            rc_time = sscanf_s(fields.at(2).c_str(), "%2d:%2d.%2d", &hours, &minutes, &seconds);
#else
            rc_date = sscanf(fields.at(1).c_str(), "%2d.%2d.%4d", &day, &month, &year);
            rc_time = sscanf(fields.at(2).c_str(), "%2d:%2d.%2d", &hours, &minutes, &seconds);
#endif
            if ((rc_date == 3) && (rc_time == 3))
                timestamp = Timestamp::toMs(year, month, day, hours, minutes, seconds);
            // TODO: extract latitude and longitude
            double lat = 0.0;
            double lon = 0.0;
            DoubleValue ele;
            const char* eleStr = fields.at(3).c_str();
            if (eleStr != NULL)
                ele = atof(eleStr);
            Trackpoint* trkpt = new Trackpoint(lon, lat, ele, timestamp);
            const char* distanceStr = fields.at(15).c_str();
            if (distanceStr != NULL) {
                double distance = atof(distanceStr);
                trkpt->setDistance(distance);
            }
            const char* hrStr = fields.at(6).c_str();
            if (hrStr != NULL) {
                unsigned int hr = atoi(hrStr);
                if (hr > 0)
                    trkpt->setHeartrate(hr);
            }
            _Trk->append(trkpt);
        }
        sdf.close();
        _Trk->pop();
        _Trk->postProcess();
        return 0;
    }


    int SuuntoDatafile::load(const std::string& filename)
    {
        int rc = load(filename.c_str());
        return rc;
    }

};
