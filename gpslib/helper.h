// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __HELPER_H_
#define __HELPER_H_

#include <cstdlib>
#include <string>
#include <sstream>
#include <limits>

#include "portable.h"

#include "Timestamp.h"


#define UNUSED(x) (void)x

/// Pi
const double PI = 3.1415926535897932384626433832795L;

/// Convert unsigned integer to Google Chart API extended encoding.
/// @see http://code.google.com/apis/chart/#extended
extern const char* encode_extended(size_t val);

/// Convert seconds into W3C timestamp.
extern std::string secs2timestamp(time_t dur);

namespace GPS {

    /// Kleineren Wert zweier Zahlen ermitteln.
    /// @param a erste Zahl
    /// @param b zweite Zahl
    /// @return kleineren der beiden Werte a und b
#ifndef min
    template <typename T>
    inline const T &min(const T &a, const T &b) { if (a < b) return a; return b; }
#endif

    /// Grˆﬂeren Wert zweier Zahlen ermitteln.
    /// @param a erste Zahl
    /// @param b zweite Zahl
    /// @return Grˆﬂeren der beiden Werte a und b
#ifndef max
    template <typename T>
    inline const T &max(const T &a, const T &b) { if (a > b) return a; return b; }
#endif


    /// Eine Ganzzahl in einen String wandeln.
    /// @param v die zu wandelnde Zahl
    /// @param padTo auf so viele Stelle soll der resultierende String nach links aufgef¸llt werden.
    /// @param padChar mit diesem Zeichen soll der resultierende String nach links aufgef¸llt werden.
    /// @return String-Repr‰sentation der Ganzzahl
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


    /// Gradmaﬂ in Bogenmaﬂ umwandeln.
    /// @param rad Gradmaﬂ
    /// @return Bogenmaﬂ
    inline double deg2rad(double deg)
    {
        return deg * PI / 180.0;
    }


    /// Bogenmaﬂ in Gradmaﬂ umwandeln.
    /// @param rad Bogenmaﬂ
    /// @return Gradmaﬂ
    inline double rad2deg(double rad)
    {
        return rad / PI * 180.0;
    }


    /// Eine beliebigen Wert via std::stringstream in einen String umwandeln.
    /// @param zu wandelnder Wert
    /// @return String-Repr‰sentation des Wertes 
    template <typename T>
    inline std::string tos(T v)
    {
        std::stringstream s;
        s << v;
        return s.str();
    }


};

#endif
