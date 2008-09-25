// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __PORTABLE_H_
#define __PORTABLE_H_

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#ifndef UNKNOWN_ENDIAN
#define UNKNOWN_ENDIAN 0
#endif


// determine byte order
#ifndef BYTE_ORDER

#if defined(sun)
#define BYTE_ORDER BIG_ENDIAN
#elif defined (__ppc__)
#define BYTE_ORDER BIG_ENDIAN
#elif defined(__ultrix)
#define BYTE_ORDER LITTLE_ENDIAN
#elif defined(__alpha )
#define BYTE_ORDER LITTLE_ENDIAN
#elif defined(i386)
#define BYTE_ORDER LITTLE_ENDIAN
#elif defined(_MSC_VER)
#define BYTE_ORDER LITTLE_ENDIAN
#elif defined(__INTEL_COMPILER)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER UNKNOWN_ENDIAN
#endif

#endif // !BYTE_ORDER


// some common types
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


// path delimiter
#ifdef _MSC_VER
const char PathDelimiter = '\\';
#else
const char PathDelimiter = '/';
#endif


/// Determine if platform is big-endian.
extern bool is_bigendian(void);


#endif // __PORTABLE_H_
