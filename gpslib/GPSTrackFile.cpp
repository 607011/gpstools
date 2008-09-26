// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <string>
#include <cstdlib>

#include "GPSTrackFile.h"

namespace GPS {

    GPSTrackFile::GPSTrackFile(const char* filename)
    {
        init((filename != NULL)? filename : "");
    }


    GPSTrackFile::GPSTrackFile(const std::string& filename)
    {
        init(filename);
    }


    void GPSTrackFile::init(const std::string& filename)
    {
        _Trk = NULL;
        _Route = NULL;
        _Filename = filename;
        metadataLinkUrl = "http://von-und-fuer-lau.de/";
        metadataLinkText = "von und fuer lau";
        creator = "gpslib";
        encoding = "UTF-8";
    }
  

    errno_t GPSTrackFile::load(const char* filename)
    {
        if (filename != NULL)
            _Filename = filename;
        errno_t rc = load(_Filename);
        return rc;
    }


    errno_t GPSTrackFile::write(const char* filename, bool onlyKept)
    {
        if (filename == NULL)
            return EINVAL;
        errno_t rc = write(std::string(filename), onlyKept);
        return rc;
    }


    void GPSTrackFile::addTrack(Track* trk)
    {
        if (trk == NULL) {
            trk = new Track(*_Trk);
            trk->postProcess();
        }
        _TrkList.push_back(trk);
    }


    void GPSTrackFile::addRoute(Route* rte)
    {
        if (rte == NULL)
            rte = new Route(*_Route);
        _Routes.push_back(rte);
    }
};
