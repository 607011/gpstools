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

        /// Name des Wegpunkts. Sollte m�glichst kurz sein.
        std::string _Name;

        /// (Optionaler) Kommentar zum Wegpunkt. Darf beliebig lange sein.
        std::string _Comment;

        /// (Optionale) Beschreibung des Wegpunkts. Darf etwas l�nger sein als der Name.
        std::string _Description;

        /// (Optionaler) Link zu einer Webseite, die mehr Informationen �ber den Wegpunkt enth�lt.
        std::string _Link;

        /// (Optionale) Informationen zur Herkunft des Wegpunkts, zum Beispiel "Garmin GPSmap 60 CSx"
        std::string _Src;

        /// H�he (in Metern) des Geoids (Normal Null) �ber dem WGS84-Ellipsoid (aus NMEA-GGA-Satz).
        FloatValue _GeoidHeight;

    public:
        /// Konstruktor.
        Waypoint(void);

        /// Konstruktor.
        /// @param lon L�ngengrad
        /// @param lat Breitengrad
        /// @param ele H�he �NN in Metern
        /// @param timestamp Zeitstempel
        Waypoint(double lon, double lat, DoubleValue ele, Timestamp timestamp);

        /// Einen Wegpunkt durch Interpolation zweier anderer erzeugen.
        /// @param wpt1 Wegpunkt 1
        /// @param wpt2 Wegpunkt 2
        /// @return Interpolierter Wegpunkt
        static Waypoint interpolated(const Waypoint& wpt1, const Waypoint& wpt2);

        /// Zeitstempel zur�ckgeben.
        /// @return Zeitstempel
        /// @see _Timestamp
        inline const Timestamp& timestamp(void) const { return _Timestamp; }

        /// Horizontal Dilusion of Precision (2D) zur�ckliefern.
        /// @return HDOP
        /// @see _HDOP
        inline const FloatValue& HDOP(void) const { return _HDOP; }

        /// Vertical Dilusion of Precision (1D) zur�ckliefern.
        /// @return VDOP
        /// @see _VDOP
        inline const FloatValue& VDOP(void) const { return _VDOP; }

        /// Position Dilusion of Precision (3D) zur�ckliefern.
        /// @return PDOP
        /// @see _PDOP
        inline const FloatValue& PDOP(void) const { return _PDOP; }

        /// Global Dilusion of Precision (4D) zur�ckliefern.
        /// @return GDOP
        /// @see _GDOP
        inline const FloatValue& GDOP(void) const { return _GDOP; }

        /// Den Namen des Wegpunkts zur�ckliefern.
        /// @return Name des Wegpunkts
        /// @see _Name
        inline std::string name(void) const { return _Name; }

        /// Die Beschreibung des Wegpunkts zur�ckliefern.
        /// @return Kurzbeschreibung des Wegpunkts
        /// @see _Description
        inline std::string description(void) const { return _Description; }

        /// Den Kommentar zum Wegpunkt zur�ckliefern.
        /// @return Kommentar zum Wegpunkt
        /// @see _Comment
        inline std::string comment(void) const { return _Comment; }

        /// Link auf weiterf�hrende Informationen �ber den Wegpunkt zur�ckliefern.
        /// @return Link
        /// @see _Link
        inline std::string link(void) const { return _Link; }

        /// Informationen �ber die Herkunft des Wegpunkts zur�ckliefern.
        /// @return Text mit Informationen �ber die Herkunft des Wegpunkts
        /// @see _Src
        inline std::string source(void) const { return _Src; }

        /// H�he des Geoids �ber dem WGS84-Ellipsoid zur�ckliefern.
        /// @return Geoid-H�he in Metern
        /// @see _GeoidHeight
        inline const FloatValue& GeoidHeight(void) const { return _GeoidHeight; }

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

        /// Informationen �ber die Herkunft des Wegpunkts setzen.
        /// @see _Src
        inline void setSource(std::string src) { _Src = src; }

        /// Link auf weiterf�hrende Informationen �ber den Wegpunkt setzen.
        /// @see _Link
        inline void setLink(std::string link) { _Link = link; }

        /// H�he des Geoids �ber dem WGS84-Ellipsoid setzen.
        /// @see _GeoidHeight
        inline void setGeoidHeight(float GeoidHeight) { _GeoidHeight = GeoidHeight; }

        /// Zeitstempel setzen.
        /// @see _Timestamp
        inline void setTimestamp(const Timestamp& timestamp) { _Timestamp = timestamp; }
    };

};
#endif // __WAYPOINT_H_
