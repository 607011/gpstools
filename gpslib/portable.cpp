// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#include "portable.h"


bool is_bigendian(void)
{
    // taken from http://www.ibm.com/developerworks/aix/library/au-endianc/index.html?ca=drs-
    volatile const int __i = 0x00000001;
    return (*(char*)&__i) == 0x00;
}
