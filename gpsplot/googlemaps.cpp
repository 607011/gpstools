// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <fstream>

#include "globals.h"

using namespace std;


void writeGoogleMaps(void)
{
    if (googleMapsFile != "")
    {
        if (verbose > 0 && !quiet)
            cout << "Schreiben der Google-Maps-API-Aufrufe nach " << googleMapsFile << " ..";
        fstream js(googleMapsFile.c_str(), fstream::out);
        if (js.bad())
            errmsg("Öffnen der Google-Maps-Javascript-Datei '" + googleMapsFile + "' fehlgeschlagen");
        js << "function initialize() {" << endl
            << "  if (GBrowserIsCompatible()) {" << endl
            << "  var map = new GMap2(document.getElementById(\"map_canvas\"));" << endl
            << "  function createMarker(km, lat, lng, ele, opts) {" << endl
            << "    var latlng = new GLatLng(lat, lng);" << endl
            << "    var marker = new GMarker(latlng, opts);" << endl
            << "    marker.value = km;" << endl
            << "    GEvent.addListener(marker,\"click\", function() {" << endl
            << "      var html = '<strong style=\"font-size: 125%\">' + km + '</strong>' +" << endl
            << "        '<br><b>Längengrad</b>: ' + lng +" << endl
            << "        '<br><b>Breitengrad</b>: ' + lat +" << endl
            << "        '<br><b>Höhe</b>: ' + ele + 'm';" << endl
            << "      map.openInfoWindowHtml(latlng, html);" << endl
            << "    });" << endl
            << "    return marker;" << endl
            << "  }" << endl
            << "  var track = new GPolyline([";
        for (GPS::TrackpointList::const_iterator i = trk->points().begin(); i != trk->points().end(); ++i)
        {
            if (i != trk->points().begin())
                js << ", ";
            js << "new GLatLng(" << (*i)->latitude() << ',' << (*i)->longitude() << ")";
        }
        js << "], \"#ff0000\", 5);}" << endl
            << "  map.setCenter(track.getBounds().getCenter(), 13);" << endl
            << "  map.addMapType(G_PHYSICAL_MAP);" << endl
            << "  map.setMapType(G_PHYSICAL_MAP);" << endl
            << "  map.addControl(new GMapTypeControl());" << endl
            << "  map.addControl(new GSmallMapControl());" << endl
            << "  map.addOverlay(track);" << endl
            << "  var baseIcon = new GIcon(); " << endl
            << "  baseIcon.iconSize = new GSize(32, 32); " << endl
            << "  baseIcon.iconAnchor = new GPoint(15, 15); " << endl
            << "  baseIcon.infoWindowAnchor = new GPoint(15, 15);" << endl
            << "  var startIcon = new GIcon(baseIcon); " << endl
            << "  startIcon.image = 'http://maps.google.com/mapfiles/kml/paddle/red-circle.png'; " << endl
            << "  startIcon.iconAnchor = new GPoint(15, 31); " << endl
            << "  startIcon.infoWindowAnchor = new GPoint(15, 31);" << endl
            << "  var finishIcon = new GIcon(baseIcon); " << endl
            << "  finishIcon.image = 'http://maps.google.com/mapfiles/kml/paddle/grn-circle.png'; " << endl
            << "  finishIcon.iconAnchor = new GPoint(15, 31);" << endl
            << "  finishIcon.infoWindowAnchor = new GPoint(15, 31);" << endl
            << "  var kmIcon = new GIcon(baseIcon); " << endl
            << "  kmIcon.image = 'http://maps.google.com/mapfiles/kml/shapes/placemark_square.png';" << endl;
        for (int i = 0; i < 360; i += 10)
        {
            char deg[10];
#if defined(_WIN32) && (_MSC_VER >= 1400)
            sprintf_s(deg, sizeof(deg), "%03d", i);
#else
            sprintf(deg, "%03d", i);
#endif
            js << "  var arrow" << deg << " = new GIcon(baseIcon);"
                << " arrow" << deg << ".image = '" << staticImagesUrl << "/arrow" << deg << ".png';" << endl;
            // TODO: Domain konfigurierbar machen
        }
        GPS::Trackpoint* start = trk->points().front();
        GPS::Trackpoint* finish = trk->points().back();
        js << "  var startOpts = {title:'Start', icon:startIcon}; " << endl
            << "  map.addOverlay(createMarker('Start', "  << start->latitude() << ", " << start->longitude() << ", " << start->elevation() << ", startOpts));" << endl
            << "  var finishOpts = {title:'Finish', icon:finishIcon}; " << endl
            << "  map.addOverlay(createMarker('Ziel', " << finish->latitude() << ", " << finish->longitude() << ", " << finish->elevation() << ", finishOpts));" << endl;
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
                        char km[10];
#if defined(_WIN32) && (_MSC_VER >= 1400)
                        sprintf_s(deg, 9, "%03d", bearing);
                        sprintf_s(km, 9, "%05d", (int)(1e-3 * d));
#else
                        sprintf(deg, "%03d", bearing);
                        sprintf(km, "%05d", (int)(1e-3 * d));
#endif
                        js << "  var kmOpts" << km << " = {title:\"" << (int) (1e-3 * d) << " km\", icon:arrow" << deg << "}; "
                            << "map.addOverlay(createMarker('" << (int) (1e-3 * d) <<" km', " << setprecision(12) << (*i)->latitude() << ", " << (*i)->longitude() << ", " << setprecision(4) << (*i)->elevation() << ", kmOpts" << km << "));" << endl;
                    }
                    d += 1e3 * kmlKmTicks;
                }
            }
        }
        js << "}" << endl;
        js.close();
        if (verbose > 0 && !quiet)
            cout << " OK" << endl;
    }
}
