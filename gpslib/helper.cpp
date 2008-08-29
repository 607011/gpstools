// $Id: helper.cpp bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>

#include "helper.h"

static const char* GOOGLE_CHART_API_ENCODING = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-.";


double deg2rad(double deg)
{
    return deg * PI / 180.0;
}


const char* encode_extended(size_t val)
{
    char* code = new char[2];
    code[0] = GOOGLE_CHART_API_ENCODING[val / 64];
    code[1] = GOOGLE_CHART_API_ENCODING[val % 64];
    return (const char*) code;
}


std::string itos(int i)
{
    std::stringstream s;
    s << i;
    return s.str();
}


std::string secs2timestamp(time_t dur)
{
    char buf[10];
    time_t h = dur / 3600;
    time_t m = (dur - (h * 3600)) / 60;
    time_t s = dur - h * 3600 - m * 60;
#if defined(_WIN32) && (_MSC_VER >= 1400)
    sprintf_s(buf, 10, "%02d:%02d:%02d", (int) h, (int) m, (int) s);
#else
    sprintf(buf, "%02d:%02d:%02d", (int) h, (int) m, (int) s);
#endif
    return std::string(buf);
}
