// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <errno.h>
#include "TCXFile.h"

using namespace std;

namespace GPS {

    TCXFile::TCXFile(const char* filename) : GPSTrackFile(filename)
    { /* ... */ }


    TCXFile::TCXFile(const std::string& filename) : GPSTrackFile(filename)
    { /* ... */ }


    errno_t TCXFile::load(const std::string& filename)
    {
        if (filename != "")
            _Filename = filename;
        if (!tcx.LoadFile(_Filename.c_str()))
            return (errno_t) tcx.ErrorId();
        TiXmlHandle gpxHandle(&tcx);
        /// TODO: Lesen der TCX-Daten implementieren
        _Trk->postProcess();
        return 0;
    }


    errno_t TCXFile::write(const std::string& filename, bool /*onlyKept*/)
    {
        if (filename != "")
            _Filename = filename;
        fstream os(_Filename.c_str(), fstream::out);
        if (os.bad())
            return EIO;
        /// TODO: Schreiben der TCX-Daten implementieren
        os.close();
        return 0;
    }


};
