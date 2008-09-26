// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __GPSTRACKFILE_H_
#define __GPSTRACKFILE_H_

#include <cerrno>
#include <ctime>
#include <vector>
#include <string>

#include "Track.h"
#include "Route.h"

#if !defined(WIN32) && !defined(errno_t)
#define errno_t int
#endif

namespace GPS {

    class Route;
    typedef std::vector<Route*> RouteList;



    /// (Teil-)Abstrakte Klasse zur Verwaltung von Dateien, die GPS-Tracks enthalten.
    class GPSTrackFile { // TODO: Die Klasse müsste eigentlich GPSFile heißen.

    protected:
        /// Aktueller GPS-Track.
        Track* _Trk;

        /// Liste von GPS-Tracks, falls GPS-Datei mehrere Tracks enthält
        TrackList _TrkList;

        /// Liste von Wegpunkten, falls die GPS-Datei welche enthält
        WaypointList _Waypoints;

        /// Aktuelle Route.
        Route* _Route;

        /// Liste von Routen, falls die GPS-Datei welche enthält
        RouteList _Routes;

        /// Name der Datei
        std::string _Filename;

    protected:
        /// Initialisierung
        /// @param filename
        void init(const std::string& filename);

    public:
        /// GPS file URL
        std::string metadataLinkUrl;
        /// GPS file URL text
        std::string metadataLinkText;
        /// GPS file creator
        std::string creator;
        /// GPS file encoding
        std::string encoding;

    public:
        /// Konstruktor.
        /// @param filename
        GPSTrackFile(const char* filename);

        /// Konstruktor.
        /// @param filename
        GPSTrackFile(const std::string& filename);

        /// Destruktor.
        virtual ~GPSTrackFile() { /* ... */ }

        /// GPS-Datei laden.
        errno_t load(const char* filename = NULL);

        /// GPS-Datei laden.
        virtual errno_t load(const std::string& filename) = 0;

        /// Track in angegebene GPS-Datei schreiben.
        virtual errno_t write(const char* filename, bool onlyKept = false);

        /// Track in angegebene GPS-Datei schreiben.
        virtual errno_t write(const std::string& filename, bool onlyKept = false) = 0;

        /// Namen der GPS-Datei zurückgeben.
        /// @return Name der GPS-Datei
        inline std::string& filename(void)
        {
            return _Filename;
        }

        /// Track zurückgeben.
        /// @return Track
        inline Track* track(void)
        {
            return _Trk;
        }

        /// Track zurückgeben.
        /// @return Track
        inline const Track* track(void) const
        {
            return _Trk;
        } 

        /// Track-Liste zurückgeben.
        /// @return Track-Liste
        inline TrackList& tracks(void)
        {
            return _TrkList;
        }

        /// Track-Liste zurückgeben.
        /// @return Track-Liste
        inline const TrackList& tracks(void) const
        {
            return _TrkList;
        }

        /// Wegpunkte-Liste zurückgeben.
        /// @return Wegpunkte-Liste
        inline WaypointList& waypoints(void)
        {
            return _Waypoints;
        }

        /// Wegpunkte-Liste zurückgeben.
        /// @return Wegpunkte-Liste
        inline const WaypointList& waypoints(void) const
        {
            return _Waypoints;
        }

        /// Wegpunkt-Liste setzen.
        /// @param wptList Wegpunkt-Liste
        inline void setWaypoints(const WaypointList& wptList)
        {
            _Waypoints = wptList;
        }

        /// Einen Wegpunkt zur Liste der Wegpunkte hinzufügen.
        /// @param wpt Wegpunkt
        inline void addWaypoint(Waypoint* wpt)
        {
            _Waypoints.push_back(wpt);
        }

        /// Routen-Liste zurückgeben.
        /// @return Routen-Liste
        inline RouteList& routes(void)
        {
            return _Routes;
        }

        /// Routen-Liste zurückgeben.
        /// @return Routen-Liste
        inline const RouteList& routes(void) const
        {
            return _Routes;
        }

        /// Track setzen.
        /// @param trk Track
        inline void setTrack(Track* trk)
        {
            _Trk = trk;
        }

        /// Track-Liste setzen.
        /// @param trkList Track-Liste
        inline void setTracks(const TrackList& trkList)
        {
            _TrkList = trkList;
        }

        /// Track zur Trackliste hinzufügen
        /// @param trk Track; wenn trk = NULL, dann wird _Trk hinzugefügt.
        void addTrack(Track* trk = NULL);

        /// Route zur Routenliste hinzufügen
        /// @param rte Route; wenn rte = NULL, dann wird _Route hinzugefügt.
        void addRoute(Route* rte = NULL);
    };

};

#endif // __GPSTRACKFILE_H_
