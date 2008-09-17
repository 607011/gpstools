// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <cmath>
#include <algorithm>

#include "Stochastics.h"

namespace GPS {

    void Stochastics::calculate(void)
    {
        if (N() < 2)
            return;
        // Messwerte werden nur sortiert, um den Median bestimmen zu können
        sort(_Values.begin(), _Values.end());
        _Median = _Values.at(N() / 2);
        _Min = _Values.front();
        _Max = _Values.back();
        _Sum = 0.0;
        _SumSquared = 0.0;
        for (std::vector<double>::const_iterator i = _Values.begin(); i != _Values.end(); ++i)
        {
            _Sum += (*i);
            _SumSquared += (*i) * (*i);
        }
        _Average = _Sum / N();
        _Variance = _SumSquared / N();
        _StandardDeviation = sqrt(_Variance);
    }


};
