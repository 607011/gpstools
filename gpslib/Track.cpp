// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#include <iostream>
#include <iomanip>
#include <cmath>

#include "helper.h"
#include "Polygon.h"
#include "Track.h"
#include "TrackLine.h"
#include "Timestamp.h"

using namespace std;

namespace GPS {

    const double Track::DEFAULT_METERS_INTERVAL = 200;
    const time_t Track::DEFAULT_SECONDS_INTERVAL = 60;


    Track::Track(void) : _MinElevation(), _MaxElevation(), _Ascent(), _Descent()
    { /* ... */ }


    Track::Track(const Track& o)
    {
        _MinElevation = o._MinElevation;
        _MaxElevation = o._MaxElevation;
        _Ascent       = o._Ascent;
        _Descent      = o._Descent;
        _Name         = o._Name;
        // do a deeeeep copy of trackpoints
        for (TrackpointList::const_iterator i = o.samples.begin(); i != o.samples.end(); ++i)
            samples.push_back(new Trackpoint(*(*i)));
    }


    void Track::merge(const Track* other, int what)
    {
        if (what == 0)
            return;
        for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
        {
            Trackpoint* match = other->getMatchingTrackpoint((*i)->timestamp());
            if (match != NULL)
            {
                if (what & HEARTRATE) {
                    IntValue hr = match->heartrate();
                    if (hr.defined())
                        (*i)->setHeartrate(hr.value());
                }
                if (what & GEOCOORDS) {
                    (*i)->setLongitude(match->longitude());
                    (*i)->setLatitude(match->latitude());
                }
                if (what & ELEVATION) {
                    (*i)->setElevation(match->elevation());
                }
            }
        }
    }


    double Track::distance(void) const
    {
        return isEmpty()
            ? 0
            : samples.back()->distance();
    }


    time_t Track::duration(void) const
    {
        return isEmpty()
            ? 0
            : samples.back()->duration();
    }


    Timestamp Track::startTimestamp(void) const
    {
        return isEmpty()
            ? Timestamp()
            : samples.front()->timestamp();
    }


    Timestamp Track::finishTimestamp(void) const
    {
        return isEmpty()
            ? Timestamp()
            : samples.back()->timestamp();
    }


    DoubleValue Track::avgHeartrate(void) const
    {
        if (isEmpty())
            return DoubleValue();
        double sum = 0.0;
        int n = 0;
        for (TrackpointList::const_iterator i = samples.begin(); i != samples.end(); ++i) {
                if ((*i)->heartrate().defined()) {
                    sum += (*i)->heartrate().value();
                    ++n;
                }
        }
        if (n > 0)
            return DoubleValue(sum / n);
        return DoubleValue();
    }


    DoubleValue Track::avgTemperature(void) const
    {
        if (isEmpty())
            return DoubleValue();
        double sum = 0.0;
        int n = 0;
        for (TrackpointList::const_iterator i = samples.begin(); i != samples.end(); ++i) {
                if ((*i)->temperature().defined())
                {
                    sum += (*i)->temperature().value();
                    ++n;
                }
        }
        if (n > 0)
            return sum / n;
        return DoubleValue();
    }


    DoubleValue Track::avgSpeed(void) const
    {
        return isEmpty()
            ? DoubleValue()
            : 1e-3 * distance() * 60.0 * 60.0 / duration();
    }


    DoubleValue Track::avgUphillSlope(void) const
    {
        return isEmpty()
            ? DoubleValue()
            : 2 * 1e2 * _Ascent / distance();
    }


    DoubleValue Track::avgDownhillSlope(void) const
    {
        return isEmpty()
            ? DoubleValue()
            : 2 * 1e2 * _Descent / distance();
    }


    bool Track::hasHeartrate(void) const
    {
        // TODO: Track::hasHeartrate() implementieren
        bool rc = true;
        return rc;
    }


    bool Track::hasSpeed(void) const
    {
        // TODO: Track::hasSpeed() implementieren
        bool rc = true;
        return rc;
    }


    bool Track::hasElevation(void) const
    {
        // TODO: Track::hasElevation() implementieren
        bool rc = true;
        return rc;
    }


    bool Track::hasDistance(void) const
    {
        // TODO: Track::hasDistance() implementieren
        bool rc = true;
        return rc;
    }


    bool Track::hasTimestamps(void) const
    {
        // TODO: Track::hasTimestamps() implementieren
        bool rc = true;
        return rc;
    }


    void Track::DouglasPeuckerSmooth(double epsilon, size_t lo, size_t hi)
    {
        assert(epsilon > 0);
        assert(lo < hi);
        double dMax = 0.0;
        size_t idx = 0;
        for (size_t i = lo + 1; i < hi - 1; ++i) {
            double d = samples[i]->perpendicularDistanceElevation(samples.at(lo), samples.at(hi));
            if (d > dMax)
            {
                idx = i;
                dMax = d;
            }
        }
        if (dMax > epsilon)
        {
            samples.at(idx)->keep();
        }
        else
        {
            samples.at(lo)->keep();
            samples.at(hi)->keep();
            return;
        }
        DouglasPeuckerSmooth(epsilon, lo, idx);
        DouglasPeuckerSmooth(epsilon, idx, hi);
    }


    void Track::DouglasPeuckerWeed2D(double epsilon, size_t lo, size_t hi)
    {
        assert(epsilon > 0);
        assert(lo < hi);
        double dMax = 0.0;
        size_t idx = 0;
        for (size_t i = lo + 1; i < hi - 1; ++i)
        {
            double d = samples[i]->perpendicularDistance2D(samples.at(lo), samples.at(hi));
            if (d > dMax)
            {
                idx = i;
                dMax = d;
            }
        }
        if (dMax > epsilon)
        {
            samples.at(idx)->keep();
        }
        else
        {
            samples.at(lo)->keep();
            samples.at(hi)->keep();
            return;
        }
        DouglasPeuckerWeed2D(epsilon, lo, idx);
        DouglasPeuckerWeed2D(epsilon, idx, hi);
    }


    void Track::DouglasPeuckerWeed3D(double epsilon, size_t lo, size_t hi)
    {
        assert(epsilon > 0);
        assert(lo < hi);
        double dMax = 0.0;
        size_t idx = 0;
        for (size_t i = lo + 1; i < hi - 1; ++i) {
            double d = samples[i]->perpendicularDistance3D(samples.at(lo), samples.at(hi));
            if (d > dMax)
            {
                idx = i;
                dMax = d;
            }
        }
        if (dMax > epsilon)
        {
            samples.at(idx)->keep();
        }
        else
        {
            samples.at(lo)->keep();
            samples.at(hi)->keep();
            return;
        }
        DouglasPeuckerWeed3D(epsilon, lo, idx);
        DouglasPeuckerWeed3D(epsilon, idx, hi);
    }


    Track* Track::smoothDouglasPeucker(double epsilon, bool ret)
    {
        assert(epsilon > 0);
        for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
            (*i)->keep(false);
        DouglasPeuckerSmooth(epsilon, 0, samples.size() - 1);
        return (ret)? remaining() : NULL;
    }


    void Track::keepAll(void)
    {
         for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
            (*i)->keep(true);
    }


    void Track::keepNone(void)
    {
         for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
            (*i)->keep(false);
    }


    Track* Track::weed2D(double epsilon, bool ret)
    {
        assert(epsilon > 0);
        if (samples.size() < 2)
            return NULL;
        keepNone();
        DouglasPeuckerWeed2D(epsilon, 0, samples.size() - 1);
        return (ret)? remaining() : NULL;
    }


    Track* Track::weed3D(double epsilon, bool ret)
    {
        assert(epsilon > 0);
        if (samples.size() < 2)
            return NULL;
        keepNone();
        DouglasPeuckerWeed3D(epsilon, 0, samples.size() - 1);
        return (ret)? remaining() : NULL;
    }


    Track* Track::smoothThreshold(double epsilon)
    {
        assert(epsilon > 0);
        if (samples.size() < 2)
            return NULL;
        Track* t = new Track(*this);
        Trackpoint* prev = t->points().front();
        keepAll();
        for (TrackpointList::iterator i = t->points().begin(); i != t->points().end(); ++i)
        {
            Trackpoint* curr = *i;
            if (curr != NULL)
            {
                if (fabs(curr->elevation().value() - prev->elevation().value()) < epsilon)
                    curr->setElevation(prev->elevation());
                else
                    prev = curr;
            }
        }
        t->calculateAscentDescent();
        t->calculateSlopes();
        return t;
    }


    TrackList* Track::split(size_t n)
    {
        TrackList* tracks = new TrackList;
        if (samples.size() < n)
        {
            tracks->push_back(new Track(*this));
        }
        else
        {
            int partNum = 0;
            size_t i = 0;
            while (i < samples.size()) 
            {
                Track* trk = new Track;
                trk->setName(_Name + " (" + tos(partNum++) + ")");
                for (size_t j = 0; j < n && i < samples.size(); ++j)
                    trk->append(new Trackpoint(*samples.at(i++)));
                tracks->push_back(trk);
            }
        }
        return tracks;
    }


    Stochastics* Track::compareTo(Track* const ref) const
    {
        if (samples.size() < 3 && ref->points().size() < 3)
            return NULL;
        Stochastics* s = new Stochastics;
        for (TrackpointList::const_iterator r = ref->points().begin(); r != ref->points().end(); ++r)
        {
            double d_max = MAX_DBL;
            Trackpoint* p1 = samples.front();
            // geringsten senkrechten Abstand zu den gedachten Verbindungslinien
            // zwischen sämtlichen Punkten des Referenztracks suchen
            for (TrackpointList::const_iterator i = samples.begin()+1; i != samples.end(); ++i)
            {
                Trackpoint* p2 = *i;
                double dist = (*r)->perpendicularDistance2D(p1, p2);
                if (dist < d_max)
                    d_max = dist;
                p1 = p2;
            }
            s->add(d_max);
        }
        s->calculate();
        return s;
    }


    Track* Track::remaining(void)
    {
        Track* trk = new Track;
        for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
            if ((*i)->kept())
                trk->append(*i);
        trk->calculateAscentDescent();
        trk->calculateSlopes();
        trk->calculateSpeeds(DEFAULT_METERS_INTERVAL);
        return trk;
    }


    void Track::calculateDurations(void)
    {
        if (samples.size() < 2)
            return;
        Trackpoint* prev = samples.front();
        time_t startTime = prev->timestamp().seconds();
        if (prev == NULL)
            return;
        for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
        {
            Trackpoint* curr = *i;
            if (curr != NULL && curr->kept())
            {
                time_t elapsed = curr->timestamp().seconds() - startTime;
                curr->setDuration(elapsed);
            }
        }
    }


    void Track::calculateDistances(void)
    {
        if (samples.size() < 2)
            return;
        Trackpoint* prev = samples.front();
        if (prev == NULL)
            return;
        for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
        {
            Trackpoint* curr = (*i);
            if (curr != NULL && curr->kept())
            {
                double dist = curr->rangeTo(prev);
                curr->setDistance(prev->distance() + dist);
                prev = curr;
            }
        }
    }


    void Track::calculateSlopes(void)
    {
        if (samples.size() < 2)
            return;
        Trackpoint* prev = samples.front();
        if (prev == NULL)
            return;
        for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
        {
            Trackpoint* curr = *i;
            if (curr != NULL && curr->kept())
            {
                if (prev->elevation().defined() && curr->elevation().defined())
                {
                    double ed = (curr->elevation() - prev->elevation()).value();
                    double dd = curr->distance() - prev->distance();
                    prev->setSlope(1e2 * ed / dd);
                }
                prev = curr;
            }
        }    
    }


    void Track::calculateSpeeds(time_t secsInterval)
    {
        if (samples.size() < 2)
            return;
        Trackpoint* prev = samples.front();
        if (prev == NULL)
            return;
        size_t lastGoodIdx = 0;
        for (size_t i = 1; i < samples.size(); ++i)
        {
            Trackpoint* curr = samples.at(i);
            if (curr != NULL && curr->kept())
            {
                time_t dt = curr->timestamp().seconds() - prev->timestamp().seconds();
                if (dt > secsInterval) {
                    double dd = curr->distance() - prev->distance();
                    double speed = 1e-3 * 3600 * dd / (double) dt;
                    double pace = (double) dt / dd / 60 * 1e3;
                    for (size_t j = lastGoodIdx; j < i; ++j) {
                        samples.at(j)->setSpeed(speed);
                        samples.at(j)->setPace(pace);
                    }
                    prev = curr;
                    lastGoodIdx = i;
                }
            }
        } 
    }


    void Track::calculateSpeeds(double metersInterval)
    {
        if (samples.size() < 2)
            return;
        Trackpoint* prev = samples.front();
        if (prev == NULL)
            return;
        size_t lastGoodIdx = 0;
        for (size_t i = 1; i < samples.size(); ++i)
        {
            Trackpoint* curr = samples.at(i);
            if (curr != NULL && curr->kept())
            {
                double dd = curr->distance() - prev->distance();
                if (dd > metersInterval) {
                    time_t dt = curr->timestamp().seconds() - prev->timestamp().seconds();
                    double speed = 1e-3 * 3600 * dd / (double) dt;
                    double pace = (double) dt / dd / 60 * 1e3;
                    for (size_t j = lastGoodIdx; j < i; ++j) {
                        samples.at(j)->setSpeed(speed);
                        samples.at(j)->setPace(pace);
                    }
                    prev = curr;
                    lastGoodIdx = i;
                }
            }
        } 
    }


    void Track::calculateAscentDescent(void)
    {
        if (samples.size() < 2)
            return;
        _MinElevation = MAX_DBL;
        _MaxElevation = MIN_DBL;
        _Ascent       = 0.0;
        _Descent      = 0.0;
        Trackpoint* prev = samples.front();
        for (TrackpointList::const_iterator i = samples.begin(); i != samples.end(); ++i)
        {
            Trackpoint* curr = *i;
            if (curr->kept()) {
                if (prev->elevation().defined() && curr->elevation().defined())
                {
                    double eleDiff = (curr->elevation() - prev->elevation()).value();
                    if (eleDiff < 0)
                        _Descent += -eleDiff;
                    else
                        _Ascent += eleDiff;
                    if (curr->elevation() > _MaxElevation)
                        _MaxElevation = curr->elevation();
                    if (curr->elevation() <_MinElevation)
                        _MinElevation = curr->elevation();
                }
                prev = curr;
            }
        }
    }


    void Track::postProcess(void)
    {
        calculateDurations();
        calculateDistances();
        calculateSlopes();
        calculateSpeeds(DEFAULT_METERS_INTERVAL);
        calculateAscentDescent();
    }


    Trackpoint* Track::getMatchingTrackpoint(const GeoCoord* t, double maxRange) const
    {
        assert(t != NULL);
        double bestRange = MAX_DBL;
        Trackpoint* match = NULL;
        for (TrackpointList::const_iterator i = samples.begin(); i != samples.end(); ++i)
        {
            double range = (*i)->rangeTo(t);
            if (range <= maxRange && range < bestRange)
            {
                bestRange = range;
                match = *i;
            }
        }
        return match;
    }


    Trackpoint* Track::getMatchingTrackpoint(double lon, double lat, double maxRange) const
    {
        GeoCoord coord(lon, lat);
        return getMatchingTrackpoint(&coord, maxRange);
    }


    TrackpointList::iterator Track::getMatchingTrackpointIter(const TrackLine& line)
    {
        TrackpointList::iterator match;
        for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
        {
            if ((*i) == line.p1() && (*(i+1)) == line.p2())
                return i+1;
        }
        return match;
    }


    void Track::insert(TrackpointList::iterator i, Trackpoint* t)
    {
        samples.insert(i, t);
    }


    TrackpointList::iterator Track::getMatchingTrackpointIter(double lon, double lat, double maxRange)
    {
        double bestRange = MAX_DBL;
        GeoCoord coord(lon, lat);
        TrackpointList::iterator match;
        for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
        {
            double range = (*i)->rangeTo(coord);
            if (range <= maxRange && range < bestRange)
            {
                bestRange = range;
                match = i;
            }
        }
        return match;
    }


    Trackpoint* Track::getMatchingTrackpoint(const Timestamp& t, bool exact) const
    {
        if (isEmpty())
            return NULL;
        int lo = 0;
        int hi = (int) samples.size() - 1;
        Trackpoint* match;
        do {
            int mid = (lo + hi) / 2;
            match = samples.at(mid);
            if (match != NULL) {
                if (match->timestamp() > t) {
                    hi = mid - 1;
                } else if (match->timestamp() < t) {
                    lo = mid + 1;
                } else return match;
            }
            else { --hi; ++lo; }
        } while (lo <= hi);
        return exact? NULL : match;
    }


    TrackLine Track::getMatchingLine(Trackpoint* const r, double maxRange) const
    {
        if (samples.size() < 3)
            return TrackLine();
        TrackLine match;
        Trackpoint* p1 = samples.front();
        for (TrackpointList::const_iterator i = samples.begin()+1; i != samples.end(); ++i)
        {
            Trackpoint* p2 = *i;
            double dist = r->perpendicularDistance2D(p1, p2);
            if (dist < maxRange)
            {
                match = TrackLine(p1, p2);
                maxRange = dist;
            }
            p1 = p2;
        }
        return match;
    }


    TrackLine Track::getMatchingLine(double lon, double lat, double maxRange) const
    {
        Trackpoint t(lon, lat);
        TrackLine line = getMatchingLine(&t, maxRange);
        return line;
    }


    TrackLine Track::getMatchingLineXY(double x, double y, double maxRange) const
    {
        if (samples.size() < 3)
            return TrackLine();
        Point r(x, y);
        TrackLine match;
        Trackpoint* p1 = samples.front();
        for (TrackpointList::const_iterator i = samples.begin()+1; i != samples.end(); ++i)
        {
            Trackpoint* p2 = *i;
            if (p1->kept() && p2->kept()) 
            {
                double dist = r.perpendicularDistanceToSegment(*p1, *p2);
                if (dist < maxRange)
                {
                    match = TrackLine(p1, p2);
                    maxRange = dist;
                }
            }
            p1 = p2;
        }
        return match;
    }


    void Track::shiftTimestamps(timestamp_t milliseconds)
    {
        for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
            (*i)->setTimestamp((*i)->timestamp() + milliseconds);
    }


    void Track::shiftTimestamps(time_t seconds)
    {
        shiftTimestamps((timestamp_t) seconds * (timestamp_t) 1000);
    }


    void Track::shiftTimestamps(std::string offset)
    {
        time_t seconds = Timestamp::offsetToSeconds(offset);
        shiftTimestamps(seconds);
    }


    void Track::shiftTimestamps(const Timestamp& timestamp)
    {
        if (samples.size() > 0)
        {
            Trackpoint* trkpt = samples.front();
            if (trkpt != NULL)
            {
                Timestamp offset = timestamp - trkpt->timestamp();
                for (TrackpointList::iterator i = samples.begin(); i != samples.end(); ++i)
                    (*i)->setTimestamp((*i)->timestamp() - offset);
            }
        }
    }


    size_t Track::keptCount(void) const
    {
        size_t n = 0;
        for (TrackpointList::const_iterator i = samples.begin(); i != samples.end(); ++i)
            if ((*i)->kept())
                ++n;
        return n;
    }


    void Track::dump(ostream& os, string msg, bool dumpTrackpoints)
    {
        if (os.good()) 
        {
            os << endl 
                << "Track: " << name() << endl 
                << "  >>" << msg << "<<" << endl
                << "  Anzahl Punkte: " << samples.size() << endl
                << "  Startzeit: " << startTimestamp().toString() << endl
                << "  Zielzeit: " << finishTimestamp().toString() << endl
                << setprecision(5) << noshowpoint
                << "  Entfernung: " << 1e-3*distance() << " km" << endl
                << setprecision(4) << showpoint
                << "  An-/Abstiege = " << ascent() << " m / " << descent() << " m" << endl
                << "  Höhe min/max = " << minElevation() << " m / " << maxElevation() << " m" << endl
                << setprecision(3)
                << "  mittl. Geschw. = " << avgSpeed() << " km/h" << endl
                << "  mittl. Anstieg = " << avgUphillSlope() << " %" << endl
                << "  mittl. Abstieg = " << avgDownhillSlope() << " %" << endl;
            if (avgHeartrate().defined())
                os << "  mittl. HF = " << (int) avgHeartrate().value() << " bpm" << endl;
            if (avgTemperature().defined())
                os << "  mittl. Temp. = " << avgTemperature() << " °C" << endl;
            if (dumpTrackpoints)
            {
                for (TrackpointList::const_iterator i = samples.begin(); i != samples.end(); ++i)
                    if ((*i) != NULL)
                        (*i)->dump(os);
            }
        }
    }

};
