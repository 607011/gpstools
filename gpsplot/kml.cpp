// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <fstream>

#include "globals.h"

using namespace std;

void writeKml(void)
{
    if (kmlFile != "")
    {
        if (verbose > 0 && !quiet)
            cout << _("Schreiben der KML-Daten nach ") << kmlFile << " ..";
        fstream kml(kmlFile.c_str(), fstream::out);
        if (kml.bad())
            errmsg("Öffnen der KML-Datendatei '" + kmlFile + "' fehlgeschlagen");
        kml << "<?xml version=\"1.0\" encoding=\"" << encoding << "\"?>" << endl
            << "<kml xlmns=\"http://earth.google.com/kml/2.2\">" << endl
            << "<Document>" << endl
            << "<name>" << title << "</name>" << endl
            << "<Style id=\"track\">" << endl
            << "<LineStyle>" << endl
            << "  <color>ff0000cc</color>" << endl
            << "  <width>3</width>" << endl
            << "</LineStyle>" << endl
            << "</Style>" << endl
            << "<Style id=\"finish_flag\">" << endl
            << "<IconStyle>" << endl
            << "  <Icon>" << endl
            << "    <href>http://maps.google.com/mapfiles/kml/paddle/grn-circle.png</href>" << endl
            << "  </Icon>" << endl
            << "  <hotSpot x=\"0.5\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>" << endl
            << "</IconStyle>" << endl
            << "</Style>" << endl
            << "<Style id=\"start_flag\">" << endl
            << " <IconStyle>" << endl
            << "   <Icon>" << endl
            << "     <href>http://maps.google.com/mapfiles/kml/paddle/red-circle.png</href>" << endl
            << "   </Icon>" << endl
            << "   <hotSpot x=\"0.5\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>" << endl
            << " </IconStyle>" << endl
            << "</Style>" << endl
            << "<Style id=\"km_flag\">" << endl
            << " <IconStyle>" << endl
            << "   <Icon>" << endl
            << "     <href>http://maps.google.com/mapfiles/kml/shapes/placemark_square.png</href>" << endl
            << "   </Icon>" << endl
            << " </IconStyle>" << endl
            << "</Style>" << endl;
        kml << "<Placemark>" << endl
            << " <name>" << title << "</name>" << endl
            << " <LineString>" << endl
            << "   <tesselate>1</tesselate>" << endl
            << "   <coordinates>" << endl;
        for (GPS::TrackpointList::const_iterator i = trk->points().begin(); i != trk->points().end(); ++i)
            kml << setprecision(14) << (*i)->longitude() << "," << (*i)->latitude() << ","
            << setprecision(4) << ((*i)->elevation().defined()? (*i)->elevation().value() : 0) << endl;
        kml << "   </coordinates>" << endl
            << " </LineString>" << endl
            << " <styleUrl>#track</styleUrl>" << endl
            << "</Placemark>" << endl;
        if (kmlMarkStart)
        {
            GPS::Trackpoint* trkpt = trk->points().front();
            kml << "<Placemark>" << endl
                << "  <name>Start</name>" << endl
                << "  <description>" << endl
                << "     <![CDATA[ <strong>Längengrad</strong>: " << setprecision(14) << trkpt->longitude() << "<br>" << endl
                << "               <strong>Breitengrad</strong>: " << trkpt->latitude() << "<br>" << endl
                << "               <strong>Höhe</strong>: " << setprecision(4) << trkpt->elevation() << "m ]]>" << endl
                << "  </description>" << endl
                << "  <Point>" << endl
                << "     <coordinates>" << setprecision(14) << trkpt->longitude() << "," << trkpt->latitude() << "," << setprecision(4) << trkpt->elevation() << "</coordinates>" << endl
                << "  </Point>" << endl
                << "  <styleUrl>#start_flag</styleUrl>" << endl
                << "</Placemark>" << endl;
        };
        if (kmlMarkFinish)
        {
            GPS::Trackpoint* trkpt = trk->points().back();
            kml << "<Placemark>" << endl
                << "  <name>Ziel</name>" << endl
                << "  <description>" << endl
                << "     <![CDATA[ <strong>"<< _("Längengrad") << "</strong>: " << setprecision(14) << trkpt->longitude() << "<br>" << endl
                << "               <strong>"<< _("Breitengrad") << "</strong>: " << trkpt->latitude() << "<br>" << endl
                << "               <strong>"<< _("Höhe") << "</strong>: " << setprecision(4) << trkpt->elevation() << _("m") << " ]]>" << endl
                << "  </description>" << endl
                << "  <Point>" << endl
                << "     <coordinates>" << setprecision(14) << trkpt->longitude() << "," << trkpt->latitude() << "," << setprecision(4) << trkpt->elevation() << "</coordinates>" << endl
                << "  </Point>" << endl
                << "  <styleUrl>#finish_flag</styleUrl>" << endl
                << "</Placemark>" << endl;
        };
        if (kmlKmTicks > 0)
        {
            double d = 1e3 * kmlKmTicks;
            for (GPS::TrackpointList::const_iterator i = trk->points().begin(); i != trk->points().end(); ++i)
            {
                if ((*i)->distance() >= d) {
                    GPS::TrackpointList::const_iterator prev = i - 1;
                    if ((*prev) != NULL) {
                        int bearing = 10 * (int)((*prev)->bearing(*i) / 10.0);
                        char deg[10];
#if defined(_WIN32) && (_MSC_VER >= 1400)
                        sprintf_s(deg, 9, "%03d", bearing);
#else
                        sprintf(deg, "%03d", bearing);
#endif
                        kml << "<Placemark>" << endl
                            << "  <name>" << (int) (1e-3 * d) << " " << _("km") << "</name>" << endl
                            << "  <description>" << endl << setprecision(14)
                            << "     <![CDATA[ <strong>"<< _("Längengrad") << "</strong>: " << (*i)->longitude() << "<br>" << endl
                            << "               <strong>"<< _("Breitengrad") << "</strong>: " << (*i)->latitude() << "<br>" << endl
                            << "               <strong>"<< _("Höhe") << "</strong>: " << setprecision(4) << (*i)->elevation() << " " << _("m") << " ]]>" << endl
                            << "  </description>" << endl
                            << "  <Point>" << endl
                            << "     <coordinates>" << setprecision(14) << (*i)->longitude() << "," << (*i)->latitude() << "," << setprecision(4) << (*i)->elevation() << "</coordinates>" << endl
                            << "  </Point>" << endl
                            << "  <styleUrl>#km_flag</styleUrl>" << endl
                            << "</Placemark>" << endl;
                    }
                    d += 1e3 * kmlKmTicks;
                }
            }
        }
        kml << "</Document>" << endl
            << "</kml>" << endl;
        kml.close();
        if (verbose > 0 && !quiet)
            cout << " OK" << endl;
    }
}
