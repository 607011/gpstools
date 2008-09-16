// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __HELPER_H_
#define __HELPER_H_

#include <cstdlib>
#include <string>
#include <sstream>
#include <limits>

#include "Timestamp.h"


#ifdef _MSC_VER
#define int8_t     __int8
#define int16_t    __int16
#define int32_t    __int32
#define int64_t    __int64
#define uint8_t    unsigned __int8
#define uint16_t   unsigned __int16
#define uint32_t   unsigned __int32
#define uint64_t   unsigned __int64
#else
#include <inttypes.h>
#endif


#define UNUSED(x) (void)x

/// Pi
const double PI = 3.1415926535897932384626433832795L;

/// Convert unsigned integer to Google Chart API extended encoding.
/// @see http://code.google.com/apis/chart/#extended
extern const char* encode_extended(size_t val);

/// Convert seconds into W3C timestamp
extern std::string secs2timestamp(time_t dur);

namespace GPS {

    template <typename T>
    inline const T &min(const T &a, const T &b) { if (a < b) return a; return b; }

    template <typename T>
    inline const T &max(const T &a, const T &b) { if (a > b) return a; return b; }

    template <typename T> std::string convert(T v)
    {
        const bsize = 64;
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


    inline double deg2rad(double deg)
    {
        return deg * PI / 180.0;
    }


    template <typename T>
    inline std::string tos(T i)
    {
        std::stringstream s;
        s << i;
        return s.str();
    }


};

#endif
