// $Id: GeoCoord.cpp bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <map>
#include <string>
#include <limits>

#include "helper.h"
#include "Point.h"
#include "GeoCoord.h"
#include "Ellipsoid.h"

namespace GPS {

    GeoCoord::GeoCoord(double lon, double lat, DoubleValue ele)
        : Point(), _Longitude(lon), _Latitude(lat), _Elevation(ele)
    {
        ell.choose("WGS84");
    }


    double GeoCoord::rangeTo(const GeoCoord& other) const
    {
        double F = deg2rad((_Latitude  + other.latitude())  / 2);
        double G = deg2rad((_Latitude  - other.latitude())  / 2);
        double l = deg2rad((_Longitude - other.longitude()) / 2);
        double S = sin(G) * sin(G) * cos(l) * cos(l) + cos(F) * cos(F) * sin(l) * sin(l);
        double C = cos(G) * cos(G) * cos(l) * cos(l) + sin(F) * sin(F) * sin(l) * sin(l);
        if (S == 0 || C == 0)
            return -1;
        double o  = atan(sqrt(S / C));
        double R  = sqrt(S / C) / o;
        double D  = 2 * o * ell.radius();
        double H1 = (3 * R - 1) / (2 * C);
        double H2 = (3 * R + 1) / (2 * S);
        double s  = D * (1 + ell.eccentricity() * H1 * sin(F) * sin(F) * cos(G) * cos(G)
            - ell.eccentricity() * H2 * cos(F) * cos(F) * sin(G) * sin(G));
        return s;
    }


    double GeoCoord::rangeTo(const GeoCoord* other) const
    {
        assert(other != NULL);
        if (other == NULL)
            return -1;
        return rangeTo(*other);
    }


    double GeoCoord::bearing(const GeoCoord& other) const
    {
        double lat1 = deg2rad(_Latitude);
        double lon1 = deg2rad(_Longitude);
        double lat2 = deg2rad(other.latitude());
        double lon2 = deg2rad(other.longitude());
        double angle = - atan2(sin(lon1 - lon2) * cos(lat2), cos(lat1) * sin(lat2) -
            sin(lat1) * cos(lat2) * cos(lon1 - lon2));
        if (angle < 0)
            angle += 2.0 * PI;
        return angle / PI * 180.0;
    }


    double GeoCoord::bearing(const GeoCoord* other) const
    {
        assert(other != NULL);
        if (other == NULL)
            return -1;
        return bearing(*other);
    }


    MercatorCoord GeoCoord::toMercator(void) const
    {
        double _X = PHI * _Longitude;
        double _Y = RHO * log(tan(PI360th * (90 + _Latitude)));
        return MercatorCoord(_X, _Y);
    }


    //This routine determines the correct UTM letter designator for the given latitude
    //returns 'Z' if latitude is outside the UTM limits of 84N to 80S
    //Written by Chuck Gantz- chuck.gantz@globalstar.com
    char GeoCoord::UTMLetterDesignator(double lat)
    {
        char d = UTMCoord::INVALID_ZONE;
        if      (( 84 >=lat) && (lat >=  72)) d = 'X';
        else if (( 72 > lat) && (lat >=  64)) d = 'W';
        else if (( 64 > lat) && (lat >=  56)) d = 'V';
        else if (( 56 > lat) && (lat >=  48)) d = 'U';
        else if (( 48 > lat) && (lat >=  40)) d = 'T';
        else if (( 40 > lat) && (lat >=  32)) d = 'S';
        else if (( 32 > lat) && (lat >=  24)) d = 'R';
        else if (( 24 > lat) && (lat >=  16)) d = 'Q';
        else if (( 16 > lat) && (lat >=   8)) d = 'P';
        else if ((  8 > lat) && (lat >=   0)) d = 'N';
        else if ((  0 > lat) && (lat >=  -8)) d = 'M';
        else if (( -8 > lat) && (lat >= -16)) d = 'L';
        else if ((-16 > lat) && (lat >= -24)) d = 'K';
        else if ((-24 > lat) && (lat >= -32)) d = 'J';
        else if ((-32 > lat) && (lat >= -40)) d = 'H';
        else if ((-40 > lat) && (lat >= -48)) d = 'G';
        else if ((-48 > lat) && (lat >= -56)) d = 'F';
        else if ((-56 > lat) && (lat >= -64)) d = 'E';
        else if ((-64 > lat) && (lat >= -72)) d = 'D';
        else if ((-72 > lat) && (lat >= -80)) d = 'C';
        return d;
    }

    UTMCoord GeoCoord::toUTM(void)
    {
        if (!UTMCoordCache.defined())
        {
            // code grabbed from http://www.gpsy.com/gpsinfo/geotoutm/
            double a = ell.radius();
            double e2 = ell.eccentricitySquared();
            double k0 = 0.9996;
            double _lon = (_Longitude+180)-int((_Longitude+180)/360)*360-180;
            int ZoneNumber = int((_lon + 180)/6) + 1;
            double lon = deg2rad(_lon);
            double lat = deg2rad(_Latitude);
            if (_Latitude >= 56.0 && _Latitude < 64.0 && _lon >= 3.0 && _lon < 12.0) ZoneNumber = 32;
            if (_Latitude >= 72.0 && _Latitude < 84.0) {
                if (_lon >= 0.0  && _lon <  9.0 ) ZoneNumber = 31;
                else if (_lon >= 9.0  && _lon < 21.0) ZoneNumber = 33;
                else if (_lon >= 21.0 && _lon < 33.0) ZoneNumber = 35;
                else if (_lon >= 33.0 && _lon < 42.0) ZoneNumber = 37;
            }
            double lon0 = deg2rad((ZoneNumber-1)*6-180+3);
            double ep2 = (e2)/(1-e2);
            double N = a/sqrt(1-e2*sin(lat)*sin(lat));
            double T = tan(lat)*tan(lat);
            double C = ep2*cos(lat)*cos(lat);
            double A = cos(lat)*(lon-lon0);
            double M = a*((1-e2/4-3*e2*e2/64-5*e2*e2*e2/256)*lat
                - (3*e2/8+3*e2*e2/32+45*e2*e2*e2/1024)*sin(2*lat)
                + (15*e2*e2/256+45*e2*e2*e2/1024)*sin(4*lat)
                - (35*e2*e2*e2/3072)*sin(6*lat));
            double easting = (k0*N*(A+(1-T+C)*A*A*A/6+(5-18*T+T*T+72*C-58*ep2)*A*A*A*A*A/120)+500000.0);
            double northing = (k0*(M+N*tan(lat)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24+(61-58*T+T*T+600*C-330*ep2)*A*A*A*A*A*A/720)));
            if (_Latitude < 0)
                northing += 10000000.0; //10000000 meter offset for southern hemisphere
            UTMCoordCache = UTMCoord(easting, northing, UTMLetterDesignator(_Latitude), ZoneNumber);
        }
        return UTMCoordCache.value();
    }


    std::string GeoCoord::dec2dms(double dec, enum LonLat ref)
    {
        enum { BUFSIZE = std::numeric_limits<double>::digits10 + 3 };
        char buf[BUFSIZE];
        char hemi = '?';
        switch (ref)
        {
        case LATITUDE:
            hemi = (dec < 0)? 'S' : 'N';
            break;
        case LONGITUDE:
            hemi = (dec < 0)? 'W' : 'E';
        default:
            break;
        }
        int d = (int) dec;
        double _m = (60.0 * (dec - d));
        int m = (int) _m;
        double s = 60.0 * (_m - m);
#if defined(_WIN32) && (_MSC_VER >= 1400)
        sprintf_s(buf, BUFSIZE-1, "%c%2d°%2d'%6.4f\"", hemi, d, m, s);
#else
        sprintf(buf, "%c%2d°%2d'%6.4f\"", hemi, d, m, s);
#endif
        return std::string(buf);
    }


    std::string GeoCoord::toString(void)
    {
        return dec2dms(_Longitude, LONGITUDE) + " " + dec2dms(_Latitude, LATITUDE);
    }

};
