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

/// Convert seconds into W3C timestamp.
extern std::string secs2timestamp(time_t dur);

/// Determine if platform is big-endian.
extern bool is_bigendian(void);

namespace GPS {

    /// Kleineren Wert zweier Zahlen ermitteln.
    /// @param a erste Zahl
    /// @param b zweite Zahl
    /// @return kleineren der beiden Werte a und b
    template <typename T>
    inline const T &min(const T &a, const T &b) { if (a < b) return a; return b; }


    /// Gr��eren Wert zweier Zahlen ermitteln.
    /// @param a erste Zahl
    /// @param b zweite Zahl
    /// @return Gr��eren der beiden Werte a und b
    template <typename T>
    inline const T &max(const T &a, const T &b) { if (a > b) return a; return b; }


    /// Eine Ganzzahl in einen String wandeln.
    /// @param v die zu wandelnde Zahl
    /// @param padTo auf so viele Stelle soll der resultierende String nach links aufgef�llt werden.
    /// @param padChar mit diesem Zeichen soll der resultierende String nach links aufgef�llt werden.
    /// @return String-Repr�sentation der Ganzzahl
    template <typename T>
    std::string convert(T v, int padTo = 0, char padChar = '0')
    {
        const int bsize = 256;
        if (padTo > bsize)
            return std::string();
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
        int padding = padTo - (buffer + bsize - tmp);
        while (padding-- > 0)
            *--tmp = padChar;
        return std::string(tmp);
    }


#ifdef _DEBUG
    inline std::string toHex(unsigned char c)
    {
        char buf[3];
        static const char digits[17] = "0123456789abcdef";
        buf[2] = 0;
        buf[1] = digits[c & 0x0f];
        buf[0] = digits[c >> 4];
        return std::string(buf);
    }

    inline std::string toBin(unsigned char c)
    {
        char buf[9];
        for (int i = 7; i >= 0; --i)
        {
            buf[i] = (c & 1) + '0';
            c >>= 1;
        }
        buf[8] = 0;
        return std::string(buf);
    }
#endif // _DEBUG


    template <typename T>    
    inline void swap(T& a, T& b)
    {
        T t = b;
        b = a;
        a = t;
    }


    /// Gradma� in Bogenma� umwandeln.
    /// @param rad Gradma�
    /// @return Bogenma�
    inline double deg2rad(double deg)
    {
        return deg * PI / 180.0;
    }


    /// Bogenma� in Gradma� umwandeln.
    /// @param rad Bogenma�
    /// @return Gradma�
    inline double rad2deg(double rad)
    {
        return rad / PI * 180.0;
    }


    /// Eine beliebigen Wert via std::stringstream in einen String umwandeln.
    /// @param zu wandelnder Wert
    /// @return String-Repr�sentation des Wertes 
    template <typename T>
    inline std::string tos(T v)
    {
        std::stringstream s;
        s << v;
        return s.str();
    }


};

#endif
