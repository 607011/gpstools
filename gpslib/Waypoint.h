// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
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
        DoubleValue _HDOP;

        /// Vertical Dilusion of Precision
        DoubleValue _VDOP;

        /// Position Dilusion of Precision
        DoubleValue _PDOP;

        /// Global Dilusion of Precision
        DoubleValue _GDOP;

        /// Magnetic variation (in degrees) at the point
        DoubleValue _MagneticVariation;

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

        /// Fix: "none", "2d", "3d", "dgps" oder "pps".
        std::string _Fix;

        /// Typ.
        std::string _Type;

        DoubleValue _AgeOfDGPSData;

        IntValue _DGPSStationId;

        /// Anzahl der Satelliten, die zum Fix beigetragen haben.
        UnsignedIntValue _SatCount;

        /// H�he (in Metern) des Geoids (Normal Null) �ber dem WGS84-Ellipsoid (aus NMEA-GGA-Satz).
        DoubleValue _GeoidHeight;

        /// Geschwindigkeit in km/h.
        double _Speed;   

        /// Geschwindigkeit in min/km.
        double _Pace;

        /// Kurs in �.
        DoubleValue _Course;   

    public:
        /// Konstruktor.
        Waypoint(void);

        /// Konstruktor.
        /// @param lon L�ngengrad
        /// @param lat Breitengrad
        Waypoint(double lon, double lat);

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
        inline const DoubleValue& HDOP(void) const { return _HDOP; }

        /// Vertical Dilusion of Precision (1D) zur�ckliefern.
        /// @return VDOP
        /// @see _VDOP
        inline const DoubleValue& VDOP(void) const { return _VDOP; }

        /// Position Dilusion of Precision (3D) zur�ckliefern.
        /// @return PDOP
        /// @see _PDOP
        inline const DoubleValue& PDOP(void) const { return _PDOP; }

        /// Global Dilusion of Precision (4D) zur�ckliefern.
        /// @return GDOP
        /// @see _GDOP
        inline const DoubleValue& GDOP(void) const { return _GDOP; }

        /// Magnetische Abweichung zur�ckliefern.
        /// @return Magnetische Abweichung in Grad (0.0 < x < 360.0)
        /// @see _MagneticVariation
        inline const DoubleValue& MagneticVariation(void) const { return _MagneticVariation; }

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
        inline const DoubleValue& GeoidHeight(void) const { return _GeoidHeight; }

        /// Type of GPX fix.
        /// @return Type of GPX fix
        inline const std::string& Fix(void) const { return _Fix; }

        /// Type (classification) of the waypoint.
        /// @return Type (classification) of the waypoint
        inline const std::string& Type(void) const { return _Type; }
        
        /// Number of satellites used to calculate the GPX fix.
        /// @return Number of satellites used to calculate the GPX fix
        inline const UnsignedIntValue& SatCount(void) const { return _SatCount; }

        /// Number of seconds since last DGPS update.
        /// @return Number of seconds since last DGPS update
        inline const DoubleValue& AgeOfDGPSData(void) const { return _AgeOfDGPSData; }

        /// ID of DGPS station used in differential correction.
        /// @return ID of DGPS station used in differential correction
        inline const IntValue& DGPSStationId(void) const { return _DGPSStationId; }

        /// Geschwindigkeit in km/h zur�ckgeben.
        /// @see _Speed
        /// @return Geschwindigkeit (km/h)
        inline double speed(void) const { return _Speed; }

        /// Geschwindigkeit in min/km zur�ckgeben.
        /// @see _Pace
        inline double pace(void) const { return _Pace; }

        /// Kurs (�) zur�ckgeben.
        /// @see _Course
        inline const DoubleValue& course(void) const { return _Course; }

        /// Geschwindigkeit in min/km als String im Format mm:ss zur�ckgeben.
        /// @see _Pace
        /// @return Geschwindigkeit (min/km) im Format mm:ss
        std::string paceString(void) const;

        /// Horizontal Dilusion of Precision setzen.
        /// @see _HDOP
        inline void setHDOP(double HDOP) { _HDOP = HDOP; }

        /// Vertical Dilusion of Precision setzen.
        /// @see _VDOP
        inline void setVDOP(double VDOP) { _VDOP = VDOP; }

        /// Set position dilusion of precision setzen.
        /// @see _PDOP
        inline void setPDOP(double PDOP) { _PDOP = PDOP; }

        /// Set global dilusion of precision setzen.
        /// @see _GDOP
        inline void setGDOP(double GDOP) { _GDOP = GDOP; }

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
        inline void setGeoidHeight(double GeoidHeight) { _GeoidHeight = GeoidHeight; }

        /// Magnetische Abweichung in Grad (0.0 < x < 360.0) setzen.
        /// @see _MagneticVariation
        inline void setMagneticVariation(double MagVar) { _MagneticVariation = MagVar; }

        /// Set type of GPX fix.
        inline void setFix(std::string Fix) { _Fix = Fix; }

        /// Set type (classification) of the waypoint.
        inline void setType(std::string Type) { _Type = Type; }

        /// Set number of satellites used to calculate the GPX fix.
        inline void setSatCount(unsigned int SatCount) { _SatCount = SatCount; }

        /// Set number of seconds since last DGPS update.
        inline void setAgeOfDGPSData(double AgeOfDGPSData) { _AgeOfDGPSData = AgeOfDGPSData; }

        /// Set ID of DGPS station used in differential correction.
        inline void setDGPSStationId(int DGPSStationId) { _DGPSStationId = DGPSStationId; }

        /// Zeitstempel setzen.
        /// @see _Timestamp
        inline void setTimestamp(const Timestamp& timestamp) { _Timestamp = timestamp; }

        /// Setzen der Geschwindigkeit (km/h).
        /// @see _Speed
        inline void setSpeed(double speed) { _Speed = speed; }

        /// Setzen der Geschwindigkeit (min/km).
        /// @see _Pace
        inline void setPace(double pace) { _Pace = pace; }

        /// Setzen des Kurses (�).
        /// @see _Course
        inline void setCourse(double course) { _Course = course; }

    };

};
#endif // __WAYPOINT_H_
