// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __WAYPOINT_H_
#define __WAYPOINT_H_

#include <string>
#include <cmath>
#include "Timestamp.h"
#include "GeoCoord.h"
#include "Value.h"

namespace GPS {

    /// Die Klasse Waypoint verwaltet Wegpunkte.
    class Waypoint : public GeoCoord {
    protected:
        /// Zeitstempel (wann der Wegpunkt aufgezeichnet wurde).
        Timestamp _Timestamp;

        /// Horizontal Dilusion of Precision
        FloatValue _HDOP;

        /// Vertical Dilusion of Precision
        FloatValue _VDOP;

        /// Position Dilusion of Precision
        FloatValue _PDOP;

        /// Global Dilusion of Precision
        FloatValue _GDOP;

        /// Name des Wegpunkts. Sollte möglichst kurz sein.
        std::string _Name;

        /// (Optionale) Beschreibung des Wegpunkts. Darf etwas länger sein als der Name.
        std::string _Description;

        /// (Optionaler) Kommentar zum Wegpunkt. Darf beliebig lange sein.
        std::string _Comment;

    public:
        /// Konstruktor.
        Waypoint(void);

        /// Konstruktor.
        /// @param lon Längengrad
        /// @param lat Breitengrad
        /// @param ele Höhe üNN in Metern
        /// @param timestamp Zeitstempel
        Waypoint(double lon, double lat, DoubleValue ele, Timestamp timestamp);

        /// Einen Wegpunkt durch Interpolation zweier anderer erzeugen.
        /// @param wpt1 Wegpunkt 1
        /// @param wpt2 Wegpunkt 2
        /// @return Interpolierter Wegpunkt
        static Waypoint interpolated(const Waypoint& wpt1, const Waypoint& wpt2);

        /// Zeitstempel zurückgeben.
        /// @return Zeitstempel
        /// @see _Timestamp
        inline const Timestamp& timestamp(void) const { return _Timestamp; }

        /// Horizontal Dilusion of Precision (2D) zurückliefern.
        /// @return HDOP
        /// @see _HDOP
        inline const FloatValue& HDOP(void) const { return _HDOP; }

        /// Vertical Dilusion of Precision (1D) zurückliefern.
        /// @return VDOP
        /// @see _VDOP
        inline const FloatValue& VDOP(void) const { return _VDOP; }

        /// Position Dilusion of Precision (3D) zurückliefern.
        /// @return PDOP
        /// @see _PDOP
        inline const FloatValue& PDOP(void) const { return _PDOP; }

        /// Global Dilusion of Precision (4D) zurückliefern.
        /// @return GDOP
        /// @see _GDOP
        inline const FloatValue& GDOP(void) const { return _GDOP; }

        /// Den Namen des Wegpunkts zurückliefern.
        /// @return Name des Wegpunkts
        /// @see _Name
        inline std::string name(void) const { return _Name; }

        /// Die Beschreibung des Wegpunkts zurückliefern.
        /// @return Kurzbeschreibung des Wegpunkts
        /// @see _Description
        inline std::string description(void) const { return _Description; }

        /// Den Kommentar zum Wegpunkt zurückliefern.
        /// @return Kommentar zum Wegpunkt
        /// @see _Comment
        inline std::string comment(void) const { return _Comment; }

        /// Horizontal Dilusion of Precision setzen.
        /// @see _HDOP
        inline void setHDOP(float HDOP) { _HDOP = HDOP; }

        /// Vertical Dilusion of Precision setzen.
        /// @see _VDOP
        inline void setVDOP(float VDOP) { _VDOP = VDOP; }

        /// Set position dilusion of precision setzen.
        /// @see _PDOP
        inline void setPDOP(float PDOP) { _PDOP = PDOP; }

        /// Set global dilusion of precision setzen.
        /// @see _GDOP
        inline void setGDOP(float GDOP) { _GDOP = GDOP; }

        /// Namen des Wegpunkts setzen.
        /// @see _Name
        inline void setName(std::string name) { _Name = name; }

        /// Beschreibung des Wegpunkts setzen.
        /// @see _Description
        inline void setDescription(std::string description) { _Description = description; }

        /// Kommentar zum Wegpunkt setzen.
        /// @see _Comment
        inline void setComment(std::string comment) { _Comment = comment; }

        /// Zeitstempel setzen.
        /// @see _Timestamp
        inline void setTimestamp(const Timestamp& timestamp) { _Timestamp = timestamp; }
    };

};
#endif // __WAYPOINT_H_
