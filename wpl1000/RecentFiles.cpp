// $Id$
// Copyright (c) 2009 Oliver Lau <oliver@ersatzworld.net>

#include "stdafx.h"


const std::string RecentFileList::find(int id) const
{
    RecentFileListBaseClass::const_iterator i;
    for (i = begin(); i != end(); ++i)
        if ((*i).first == id)
            return (*i).second;
    return std::string();
}


bool RecentFileList::add(std::string v)
{
    RecentFileListBaseClass::const_iterator i;
    bool found = false;
    int maxId = -1;
    for (i = begin(); !found && i != end(); ++i)
    {
        if ((*i).second == v)
            return false;
        if ((*i).first > maxId)
            maxId = (*i).first;
    }
    if (size() >= MaxSize)
        pop_back();
    push_front(std::make_pair((maxId < 0)? ID_RECENT_FILE_LIST : maxId+1, v));
    return true;
}


bool RecentFileList::remove(const std::string &v)
{
    RecentFileListBaseClass::const_iterator i;
    for (i = begin(); i != end(); ++i)
    {
        if ((*i).second == v)
        {
            erase(i);
            return true;
        }
    }
    return false;
}
