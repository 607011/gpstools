// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <algorithm>

#include "helper.h"

static const char* GOOGLE_CHART_API_ENCODING = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-.";


bool is_bigendian(void)
{
    // taken from http://www.ibm.com/developerworks/aix/library/au-endianc/index.html?ca=drs-
    volatile const int __i = 0x00000001;
    return (*(char*)&__i) == 0x00;
}


const char* encode_extended(size_t val)
{
    char* code = new char[2];
    code[0] = GOOGLE_CHART_API_ENCODING[val / 64];
    code[1] = GOOGLE_CHART_API_ENCODING[val % 64];
    return (const char*) code;
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
