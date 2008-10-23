// $Id$
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


    void GPXFile::getWptType(Waypoint* wpt, TiXmlNode* gpxWpt)
    {
        TiXmlElement* element = gpxWpt->ToElement();

        double lat;
        element->QueryDoubleAttribute("lat", &lat);
        wpt->setLatitude(lat);

        double lon;
        element->QueryDoubleAttribute("lon", &lon);
        wpt->setLongitude(lon);

        TiXmlElement* timeElement = element->FirstChildElement("time");
        if (timeElement != NULL)
            wpt->setTimestamp(timeElement->GetText());

        TiXmlElement* eleElement = element->FirstChildElement("ele");
        if (eleElement != NULL && eleElement->GetText() != NULL)
            wpt->setElevation(atof(eleElement->GetText()));

        TiXmlElement* nameElement = element->FirstChildElement("name");
        if (nameElement != NULL && nameElement->GetText() != NULL)
            wpt->setName(nameElement->GetText());

        TiXmlElement* cmtElement = element->FirstChildElement("cmt");
        if (cmtElement != NULL && cmtElement->GetText() != NULL)
            wpt->setComment(cmtElement->GetText());

        TiXmlElement* descElement = element->FirstChildElement("desc");
        if (descElement != NULL && descElement->GetText() != NULL)
            wpt->setName(descElement->GetText());

        TiXmlElement* hdopElement = element->FirstChildElement("hdop");
        if (hdopElement != NULL && hdopElement->GetText() != NULL)
            wpt->setHDOP(atof(hdopElement->GetText()));

        TiXmlElement* pdopElement = element->FirstChildElement("pdop");
        if (pdopElement != NULL && pdopElement->GetText() != NULL)
            wpt->setPDOP(atof(pdopElement->GetText()));

        TiXmlElement* vdopElement = element->FirstChildElement("vdop");
        if (vdopElement != NULL && vdopElement->GetText() != NULL)
            wpt->setVDOP(atof(vdopElement->GetText()));

        TiXmlElement* srcElement = element->FirstChildElement("src");
        if (srcElement != NULL && srcElement->GetText() != NULL)
            wpt->setSource(srcElement->GetText());

        TiXmlElement* linkElement = element->FirstChildElement("link");
        if (linkElement != NULL && linkElement->GetText() != NULL)
            wpt->setLink(linkElement->GetText());

        TiXmlElement* geoidheightElement = element->FirstChildElement("geoidheight");
        if (geoidheightElement != NULL && geoidheightElement->GetText() != NULL)
            wpt->setGeoidHeight(atof(geoidheightElement->GetText()));

        TiXmlElement* magvarElement = element->FirstChildElement("magvar");
        if (magvarElement != NULL && magvarElement->GetText() != NULL)
            wpt->setMagneticVariation(atof(magvarElement->GetText()));

        TiXmlElement* fixElement = element->FirstChildElement("fix");
        if (fixElement != NULL && fixElement->GetText() != NULL)
            wpt->setFix(fixElement->GetText());

        TiXmlElement* typeElement = element->FirstChildElement("type");
        if (typeElement != NULL && typeElement->GetText() != NULL)
            wpt->setType(typeElement->GetText());

        TiXmlElement* satElement = element->FirstChildElement("sat");
        if (satElement != NULL && satElement->GetText() != NULL)
            wpt->setSatCount(atoi(satElement->GetText()));

        TiXmlElement* stationElement = element->FirstChildElement("dgpsid");
        if (stationElement != NULL && stationElement->GetText() != NULL)
            wpt->setDGPSStationId(atoi(stationElement->GetText()));

        TiXmlElement* ageElement = element->FirstChildElement("ageofdgpsdata");
        if (ageElement != NULL && ageElement->GetText() != NULL)
            wpt->setAgeOfDGPSData(atof(ageElement->GetText()));
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

        // read tracks
        TiXmlNode* gpxTrk = gpxHandle.FirstChild("gpx").FirstChild("trk").ToNode();
        while (gpxTrk != NULL) 
        {
            _Trk = new Track;

            TiXmlElement* nameElement = gpxTrk->FirstChildElement("name");
            if (nameElement != NULL && nameElement->GetText() != NULL)
                _Trk->setName(nameElement->GetText());
            TiXmlElement* numberElement = gpxTrk->FirstChildElement("number");
            if (numberElement != NULL && numberElement->GetText() != NULL)
                _Trk->setNumber((unsigned int) atoi(numberElement->GetText()));
            
            TiXmlNode* gpxTrkSeg = gpxTrk->FirstChild("trkseg");
            while (gpxTrkSeg != NULL) {
                TiXmlNode* gpxTrkpt = gpxTrkSeg->FirstChild("trkpt");
                while (gpxTrkpt != NULL)
                {
                    Trackpoint* trkpt = new Trackpoint;
                    getWptType(trkpt, gpxTrkpt);
                    _Trk->append(trkpt);
                    gpxTrkpt = gpxTrkpt->NextSibling();
                }
                gpxTrkSeg = gpxTrkSeg->NextSibling();
            }

            _Trk->postProcess();
            addTrack(_Trk);
            gpxTrk = gpxTrk->NextSibling();
        }

        // read waypoints
        TiXmlNode* gpxWpt = gpxHandle.FirstChild("gpx").FirstChild("wpt").ToNode();
        while (gpxWpt != NULL) 
        {
            Waypoint* wpt = new Waypoint;
            getWptType(wpt, gpxWpt);
            _Waypoints.push_back(wpt);
            gpxWpt = gpxWpt->NextSibling();
        }

        // TODO: load routes
        TiXmlNode* gpxRte = gpxHandle.FirstChild("gpx").FirstChild("rte").ToNode();
        while (gpxRte != NULL) 
        {
            _Route = new Route;

            TiXmlElement* nameElement = gpxTrk->FirstChildElement("name");
            if (nameElement != NULL && nameElement->GetText() != NULL)
                _Route->setName(nameElement->GetText());
            
            TiXmlNode* gpxRtept = gpxRte->FirstChild("rtept");
            while (gpxRtept != NULL)
            {
                Waypoint* wpt = new Waypoint;
                getWptType(wpt, gpxRtept);
                _Route->append(wpt);
                gpxRtept = gpxRtept->NextSibling();
            }
            _Trk->postProcess();
            addTrack(_Trk);
            gpxRte = gpxRte->NextSibling();
        }

        return 0;
    }


    // TODO: auﬂer Tracks auch Wegpunkte und Routen speichern
    errno_t GPXFile::write(const std::string& filename, bool onlyKept)
    {
        if (!filename.empty())
            _Filename = filename;
        if (_Filename.empty())
            return EINVAL;
        fstream os(_Filename.c_str(), fstream::out);
        if (!os.good())
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
        // TODO: also save routes contained in GPX file
        for (WaypointList::const_iterator j = _Waypoints.begin(); j != _Waypoints.end(); ++j)
        {
            os << setprecision(14)
                << "<wpt lat=\"" << (*j)->latitude() << "\" lon=\"" << (*j)->longitude() << "\">" << endl;
            if (!(*j)->name().empty())
                os << " <name>" << (*j)->name() << "</name>" << endl;
            if (!(*j)->timestamp().isNull())
                os <<" <time>" << (*j)->timestamp().toString() << "</time>" << endl;
            if ((*j)->elevation().defined())
                os << setprecision(4) << " <ele>" << (*j)->elevation().value() << "</ele>" << endl;
            if ((*j)->HDOP().defined())
                os << setprecision(3) << " <hdop>" << (*j)->HDOP().value() << "</hdop>" << endl;
            if ((*j)->VDOP().defined())
                os << setprecision(3) << " <vdop>" << (*j)->VDOP().value() << "</vdop>" << endl;
            if ((*j)->PDOP().defined())
                os << setprecision(3) << " <pdop>" << (*j)->PDOP().value() << "</pdop>" << endl;
            os << "</wpt>" << endl;
        }
        for (TrackList::const_iterator j = _TrkList.begin(); j != _TrkList.end(); ++j)
        {
            os << "<trk>" << endl;
            if (!(*j)->name().empty())
                os << " <name>" << (*j)->name() << "</name>" << endl;
            if ((*j)->number().defined())
                os << " <number>" << (*j)->number().value() << "</number>" << endl;
            os << " <trkseg>" << endl;
            const TrackpointList& points = (*j)->points();
            for (TrackpointList::const_iterator i = points.begin(); i != points.end(); ++i)
            {
                if ((onlyKept && (*i)->kept()) || !onlyKept) {
                    os << setprecision(14)
                        << "  <trkpt lat=\"" << (*i)->latitude() << "\" lon=\"" << (*i)->longitude() << "\">" << endl;
                    if ((*i)->elevation().defined())
                        os << setprecision(4) << "   <ele>" << (*i)->elevation().value() << "</ele>" << endl;
                    if (!(*i)->timestamp().isNull())
                        os <<"   <time>" << (*i)->timestamp().toString() << "</time>" << endl;
                    if ((*i)->HDOP().defined())
                        os << setprecision(3) << " <hdop>" << (*i)->HDOP().value() << "</hdop>" << endl;
                    if ((*i)->VDOP().defined())
                        os << setprecision(3) << " <vdop>" << (*i)->VDOP().value() << "</vdop>" << endl;
                    if ((*i)->PDOP().defined())
                        os << setprecision(3) << " <pdop>" << (*i)->PDOP().value() << "</pdop>" << endl;
                    os << "  </trkpt>" << endl;
                }
            }
            os << " </trkseg>" << endl
                << "</trk>" << endl;
        }
        os << "</gpx>" << endl;
        os.close();
        return 0;
    }


    Track* GPXFile::trackByName(const std::string& name)
    {
        for (TrackList::const_iterator i = _TrkList.begin(); i != _TrkList.end(); ++i)
            if ((*i)->name() == name)
                return (*i);
        return NULL;
    }


    Track* GPXFile::trackByNumber(unsigned int number)
    {
        for (TrackList::const_iterator i = _TrkList.begin(); i != _TrkList.end(); ++i)
            if ((*i)->number().defined() && (*i)->number().value() == number)
                return (*i);
        return NULL;
    }

};
