// $Id$
// Copyright (c) 2009 Oliver Lau <oliver@ersatzworld.net>

#pragma once

typedef struct TrackListColumnType {
    LPSTR pszText;
    int cx;
    int fmt;
} TRACKLISTCOLUMNTYPE;

struct TrackListInfoType
{
    TrackListInfoType(void)
        : pszStart(NULL), pszFinish(NULL), pszPointCount(NULL), pszDistance(NULL), pszDuration(NULL) { }
    ~TrackListInfoType()
    {
        if (pszStart != NULL)
            delete [] pszStart;
        if (pszFinish != NULL)
            delete [] pszFinish;
        if (pszPointCount != NULL)
            delete [] pszPointCount;
        if (pszDistance != NULL)
            delete [] pszDistance;
        if (pszDuration != NULL)
            delete [] pszDuration;
    }
    LPSTR pszStart;
    LPSTR pszFinish;
    LPSTR pszPointCount;
    LPSTR pszDistance;
    LPSTR pszDuration;
};

typedef struct TrackListInfoType TRACKLISTINFOTYPE;
