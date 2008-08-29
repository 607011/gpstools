// $Id: GPXFile.cpp a5c498e66508 2008/08/28 06:31:06 Oliver Lau <oliver.lau@gmail.com> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <errno.h>
#include "GPXFile.h"

using namespace std;

namespace GPS {

    GPXFile::GPXFile(const char* filename) : GPSTrackFile(filename)
    { /* ... */ }


    GPXFile::GPXFile(const std::string& filename) : GPSTrackFile(filename)
    { /* ... */ }


    errno_t GPXFile::load(const char* filename)
    {
        return load((filename != NULL)? std::string(filename) : "");
    }


    errno_t GPXFile::load(const std::string& filename)
    {
        if (!filename.empty())
            _Filename = filename;
        if (_Filename.empty())
            return EINVAL;
        if (!gpx.LoadFile(_Filename.c_str()))
            return (errno_t) gpx.ErrorId();
        TiXmlHandle gpxHandle(&gpx);
        TiXmlNode* gpxTrk = gpxHandle.FirstChild("gpx").FirstChild("trk").ToNode();
        while (gpxTrk != NULL) 
        {
            _Trk = new Track;
            TiXmlNode* nameNode = gpxTrk->FirstChild("name");
            if (nameNode != NULL) {
                TiXmlElement* nameElement = nameNode->ToElement();
                if (nameElement != NULL)
                    _Trk->setName((nameElement->GetText() != NULL)? nameElement->GetText() : "");
            }
            TiXmlNode* gpxTrkSeg = gpxTrk->FirstChild("trkseg");
            while (gpxTrkSeg != NULL) {
                TiXmlNode* gpxTrkpt = gpxTrkSeg->FirstChild("trkpt");
                while (gpxTrkpt != NULL)
                {
                    TiXmlElement* trkpt = gpxTrkpt->ToElement();
                    double lat;
                    double lon;
                    trkpt->QueryDoubleAttribute("lat", &lat);
                    trkpt->QueryDoubleAttribute("lon", &lon);
                    DoubleValue ele;
                    TiXmlElement* gpxEle = trkpt->FirstChildElement("ele");
                    if (gpxEle != NULL && gpxEle->GetText() != NULL)
                        ele = atof(gpxEle->GetText());
                    const char* timestamp = NULL;
                    TiXmlElement* gpxTime = trkpt->FirstChildElement("time");
                    if (gpxTime != NULL)
                        timestamp = gpxTime->GetText();
                    _Trk->append(new Trackpoint(lon, lat, ele, Timestamp(timestamp)));
                    gpxTrkpt = gpxTrkpt->NextSibling();
                }
                gpxTrkSeg = gpxTrkSeg->NextSibling();
            }

            _Trk->postProcess();
            addTrack(_Trk);
            gpxTrk = gpxTrk->NextSibling();
        }
        return 0;
    }


    errno_t GPXFile::write(const std::string& filename, bool onlyKept)
    {
        if (!filename.empty())
            _Filename = filename;
        if (_Filename.empty())
            return EINVAL;
        fstream os(_Filename.c_str(), fstream::out);
        if (os.bad())
            return EIO;
        os << "<?xml version=\"1.0\" encoding=\"" << encoding << "\" standalone=\"no\" ?>" << endl
            << "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\""
            << " creator=\"" << creator << "\" version=\"1.1\""
            << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
            << " xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">" 
            << "<metadata>" << endl
            << " <link href=\"" << metadataLinkUrl << "\">" << endl
            << "  <text>" << metadataLinkText << "</text>" << endl
            << " </link>" << endl
            << "</metadata>" << endl;
        for (TrackList::const_iterator j = _TrkList.begin(); j != _TrkList.end(); ++j)
        {
            os << "<trk>" << endl;
            if (!(*j)->name().empty()) {
                os << " <name>" << (*j)->name() << "</name>" << endl;
            }
            os << " <trkseg>" << endl;
            for (TrackpointList::const_iterator i = (*j)->points().begin(); i != (*j)->points().end(); ++i)
            {
                if ((onlyKept && (*i)->kept()) || !onlyKept) {
                    os << setprecision(14)
                        << "  <trkpt lat=\"" << (*i)->latitude() << "\" lon=\"" << (*i)->longitude() << "\">" << endl;
                    if ((*i)->elevation().defined()) {
                        os << setprecision(4)
                            << "   <ele>" << (*i)->elevation() << "</ele>" << endl;
                    }
                    if (!(*i)->timestamp().isNull()) {
                        os <<"   <time>" << (*i)->timestamp().toString() << "</time>" << endl;
                    }
                    os << "  </trkpt>" << endl;
                }
            }
            os << " </trkseg>" << endl
                << "</trk>" << endl;
        }
        os << "</gpx>" << endl;
        // TODO: also save waypoints and routes contained in GPX file
        os.close();
        return 0;
    }


};
