// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

#include "gpslib/GeoCoord.h"
#include "gpslib/Trackpoint.h"
#include "gpslib/Point.h"
#include "gpslib/Vector.h"
#include "gpslib/Track.h"
#include "gpslib/GPXFile.h"
#include "gpslib/Stochastics.h"
#include "gpslib/Polygon.h"

using namespace std;
using namespace GPS;

bool quiet = false;
bool doMassDump = false;
bool doWrite = true;



void polygon_tests(void)
{
    Polygon p;
    p << Point(1, 1) << Point(1, 3) << Point(5, 3);
    p.close();
    cout << "perimeter = " << p.perimeter() << endl
        << "area = " << p.area();
}


void basic_tests(void)
{
    Trackpoint p1(9.8064893, 52.380593); // UTM Zone: 32 Easting: 554895 Northing: 5803676
    Trackpoint p2(9.8065988, 52.380422); // UTM Zone: 32 Easting: 554902 Northing: 5803657

    cout << (std::string) p1 << endl;
    cout << (std::string) p2 << endl;

    UTMCoord u1 = p1;
    UTMCoord u2 = p2;

    cout << "Zone: " << u1.zoneNumber() << "(" << u1.zone() << ") Easting: " << (int) u1.easting() << " Northing: " << (int) u1.northing() << endl;
    cout << "Zone: " << u2.zoneNumber() << "(" << u2.zone() << ") Easting: " << (int) u2.easting() << " Northing: " << (int) u2.northing() << endl;

    Point au = u1;
    Point bu = u2;

    MercatorCoord m1 = p1;
    MercatorCoord m2 = p2;

    cout << "X/Y: " << (int) m1.x() << "m / " << (int) m1.y() << "m" << endl;
    cout << "X/Y: " << (int) m2.x() << "m / " << (int) m2.y() << "m" << endl;

    Point a = p1;
    Point b = p2;

    cout << "Lon/Lat distance:  " << p1.rangeTo(p2) << "m" << endl;
    cout << "UTM distance:      " << (double) (au - bu) << "m" << endl;
    cout << "Mercator distance: " << (double) (a - b) << "m" << endl;


}


void weeding_tests(void)
{
    const std::string filename = "20080621-STUNT100-Runde1.gpx";
    if (!quiet)
        cout << "Reading " << filename << " .. ";
    GPXFile gpxFile;
    int rc = gpxFile.load(filename);
    if (rc != 0) {
        cerr << "failed." << endl;
        exit(rc);
    }

    Track* trk = gpxFile.track();

    if (!quiet)
        trk->dump(cout, filename);

    for (double e = 1; e < 200; e += e)
    {
        Track* t;
        const int BUFSIZE = 50;
        char fn[BUFSIZE];
#if defined(_WIN32) && (_MSC_VER >= 1400)
        sprintf_s(fn, BUFSIZE-1, "hh-2d-%.0f.gpx", e);
#else
        sprintf(fn, "hh-2d-%.0f.gpx", e);
#endif
        t = trk->weed2D(e);
        if (t != NULL) {
            t->calculateDistances();
            t->calculateAscentDescent();
            t->calculateSlopes();
            t->calculateSpeeds(Track::DEFAULT_METERS_INTERVAL);
            if (doMassDump)
                t->dump(cout, fn);
            if (doWrite)
                gpxFile.write(fn, true);
            delete t;
        }

#if defined(_WIN32) && (_MSC_VER >= 1400)
        sprintf_s(fn, BUFSIZE-1, "hh-3d-%.0f.gpx", e);
#else
        sprintf(fn, "hh-3d-%.0f.gpx", e);
#endif
        t = trk->weed3D(e);
        if (t != NULL) {
            t->calculateDistances();
            t->calculateAscentDescent();
            t->calculateSlopes();
            t->calculateSpeeds(Track::DEFAULT_METERS_INTERVAL);
            cout << " e = " << noshowpoint << setw(7) << setfill(' ') << e
                 << "   #points = " << setw(5) << setfill(' ') << t->points().size() << endl;
            Stochastics* s = t->compareTo(trk);
            cout << "   deviation = " << setw(12) << setfill(' ') << setprecision(10) << s->average() << endl;
            if (doMassDump)
                t->dump(cout, fn);
            if (doWrite)
                gpxFile.write(fn, true);
            delete t;
        }
    }
}

int main(/*int argc, char* argv[]*/)
{
    basic_tests();
    polygon_tests();
    // weeding_tests();

    if (!quiet)
        cout << endl<< "Fertig." << endl;

    return EXIT_SUCCESS;
}
