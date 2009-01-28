// $Id$
// Copyright (c) 2009 Oliver Lau <oliver@ersatzworld.net>

#pragma once

#include "stdafx.h"

typedef std::pair<int, std::string> RecentFileListEntry;
typedef std::deque<RecentFileListEntry> RecentFileListBaseClass;
class RecentFileList : public RecentFileListBaseClass
{
public:
    static const int MaxSize = 10;

    /// @return matching string
    const std::string find(int id) const;

    /// @return true on successful add
    bool add(std::string v);

    /// @return true on successful removal
    bool remove(const std::string &v);
};
