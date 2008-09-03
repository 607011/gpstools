// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __HELPER_H_
#define __HELPER_H_

#include <cstdlib>
#include <string>

#include "Timestamp.h"


/// Pi
const double PI = 3.1415926535897932384626433832795L;

/// Grad in Radians wandeln.
/// @param _Deg Gradangabe
/// @return Radians
extern double deg2rad(double deg);

/// Convert unsigned integer to Google Chart API extended encoding.
/// @see http://code.google.com/apis/chart/#extended
extern const char* encode_extended(size_t val);

/// Convert int to string.
extern std::string itos(int _Val);

/// Convert double to string.
extern std::string dtos(double d);

/// Convert seconds into W3C timestamp
extern std::string secs2timestamp(time_t dur);

namespace GPS {

    template <typename T>
    inline const T &min(const T &a, const T &b) { if (a < b) return a; return b; }

    template <typename T>
    inline const T &max(const T &a, const T &b) { if (a > b) return a; return b; }

    template <typename T> std::string convert(T v)
    {
        enum { bsize = std::numeric_limits<T>::digits10 + 3 };
        char buffer[bsize];
        const char digits[11] = "0123456789";
        bool neg = v < 0;
        char *tmp = buffer + bsize;
        *tmp = 0;
        *--tmp = digits[(neg? -(v + (v < -10? 10: 0)): v) % 10];
        v /= 10;
        if (neg)
            v = -v;
        for (; v > 0; v /= 10)
            *--tmp = digits[v % 10];
        if (neg)
            *--tmp = '-';   
        return std::string(tmp);
    }

};

#endif
