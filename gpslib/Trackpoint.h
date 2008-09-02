// $Id: Trackpoint.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __TRACKPOINT_H_
#define __TRACKPOINT_H_

#include <ostream>
#include <string>
#include <cmath>
#include "Timestamp.h"
#include "Waypoint.h"

namespace GPS {

    /// Die Klasse Trackpoint verwaltet von GPS-Geräten aufgezeichnete Trackpunkte.
    class Trackpoint : public Waypoint
    {
    protected:
        /// Entfernung vom Start in Metern.
        double _Distance;

        /// Anstieg zum nächsten Trackpunkt in Prozent.
        double _Slope;

        /// Geschwindigkeit in km/h.
        double _Speed;   

        /// Geschwindigkeit in min/km.
        double _Pace;

        /// Herzfrequenz in Schlägen pro Minute.
        IntValue _Heartrate;

        /// Temperatur in °C.
        DoubleValue _Temperature;

        /// Verstrichene Zeit seit dem Start in Sekunden.
        time_t _Duration;

        /// Flag, das angibt, ob dieser Trackpunkt nach dem Ausdünnen via
        /// Track::smoothDouglasPeucker(), Track::weed2D() oder Track::weed3D()
        /// beibehalten wurde.
        bool _Keep;

    public:
        /// Default-Konstruktor.
        Trackpoint(void) { /* ... */ }

        /// Konstruktor.
        Trackpoint(double lon, double lat, DoubleValue ele = DoubleValue(), Timestamp timestamp = Timestamp());

        /// Copy-Konstruktor.
        /// Der Copy-Konstruktor überträgt alle Membervariablen bis auf _Keep.
        /// _Keep wird auf false gesetzt.
        /// @param other Trackpunkt, von dem die Kopie erstellt werden soll.
        Trackpoint(const Trackpoint& other);

        /// Abstand des Trackpunktes zu einer gedachten berechnen, die durch die beiden übergebenen Trackpunkten verläuft.
        /// Die Berechnung erfolgt in zwei Dimensionen, und zwar nur unter Einbeziehung der Entfernung vom Start und der Höhe.
        /// @param p1 Erster Punkt, durch den die gedachte Linie verlaufen soll.
        /// @param p2 Zweiter Punkt, durch den die gedachte Linie verlaufen soll.
        /// @return (Senkrechter) Abstand.
        double perpendicularDistanceElevation(const Trackpoint* p1, const Trackpoint* p2);

        /// Abstand des Trackpunktes zu einer gedachten berechnen, die durch die beiden übergebenen Trackpunkten verläuft.
        /// Die Berechnung erfolgt in zwei Dimensionen, und zwar nur unter Einbeziehung der Entfernung vom Start und der Höhe.
        /// @param p1 Erster Punkt, durch den die gedachte Linie verlaufen soll.
        /// @param p2 Zweiter Punkt, durch den die gedachte Linie verlaufen soll.
        /// @return (Senkrechter) Abstand.
        double perpendicularDistanceElevation(const Trackpoint& p1, const Trackpoint& p2);

        /// Abstand des Trackpunktes zu einer gedachten berechnen, die durch die beiden übergebenen Trackpunkten verläuft.
        /// Die Berechnung erfolgt in zwei Dimensionen, und zwar ausschließlich unter Einbeziehung von Längen- und Breitengrad. Die Höhe wird ignoriert.
        /// @param p1 Erster Punkt, durch den die gedachte Linie verlaufen soll.
        /// @param p2 Zweiter Punkt, durch den die gedachte Linie verlaufen soll.
        /// @return (Senkrechter) Abstand.
        double perpendicularDistance2D(Trackpoint* p1, Trackpoint* p2);

        /// Abstand des Trackpunktes zu einer gedachten berechnen, die durch die beiden übergebenen Trackpunkten verläuft.
        /// Die Berechnung erfolgt in zwei Dimensionen, und zwar ausschließlich unter Einbeziehung von Längen- und Breitengrad. Die Höhe wird ignoriert.
        /// @param p1 Erster Punkt, durch den die gedachte Linie verlaufen soll.
        /// @param p2 Zweiter Punkt, durch den die gedachte Linie verlaufen soll.
        /// @return (Senkrechter) Abstand.
        double perpendicularDistance2D(Trackpoint& p1, Trackpoint& p2);

        /// Abstand des Trackpunktes zu einer gedachten berechnen, die durch die beiden übergebenen Trackpunkten verläuft.
        /// Die Berechnung erfolgt in drei Dimensionen, also unter Einbeziehung von Längen- und Breitengrad sowie der Höhe.
        /// @param p1 Erster Punkt, durch den die gedachte Linie verlaufen soll.
        /// @param p2 Zweiter Punkt, durch den die gedachte Linie verlaufen soll.
        /// @return (Senkrechter) Abstand.
        double perpendicularDistance3D(Trackpoint* p1, Trackpoint* p2);

        /// Abstand des Trackpunktes zu einer gedachten berechnen, die durch die beiden übergebenen Trackpunkten verläuft.
        /// Die Berechnung erfolgt in drei Dimensionen, also unter Einbeziehung von Längen- und Breitengrad sowie der Höhe.
        /// @param p1 Erster Punkt, durch den die gedachte Linie verlaufen soll.
        /// @param p2 Zweiter Punkt, durch den die gedachte Linie verlaufen soll.
        /// @return (Senkrechter) Abstand.
        double perpendicularDistance3D(Trackpoint& p1, Trackpoint& p2);

        /// Alle relevanten Daten in einer vom Menschen lesbaren Weise ausgeben.
        /// @param os Stream, in den die Daten ausgegeben werden sollen.
        void dump(std::ostream& os);

        /// Entfernung vom Start in Metern zurückgeben.
        /// @return Entfernung in Metern
        /// @see _Distance
        inline double distance(void) const { return _Distance; }

        /// Anstieg in Prozent zum nächsten Trackpunkt zurückgeben.
        /// @return Anstieg in Prozent
        /// @see _Slope
        inline double slope(void) const { return _Slope; }

        /// Geschwindigkeit in km/h zurückgeben.
        /// @see _Speed
        /// @return Geschwindigkeit (km/h)
        inline double speed(void) const { return _Speed; }

        /// Geschwindigkeit in min/km zurückgeben.
        /// @see _Pace
        inline double pace(void) const { return _Pace; }

        /// Geschwindigkeit in min/km als String im Format mm:ss zurückgeben.
        /// @see _Pace
        /// @return Geschwindigkeit (min/km) im Format mm:ss
        std::string paceString(void) const;

        /// Herzfrequenz in Schlägen pro Minute zurückgeben.
        /// @see _Heartrate
        /// @return HF (1/min)
        inline const IntValue& heartrate(void) const { return _Heartrate; }

        /// Temperatur in °C zurückgeben.
        /// @see _Temperature
        /// @return Temperatur in °C
        inline const DoubleValue& temperature(void) const { return _Temperature; }

        /// Seit dem Start verstrichene Zeit in Sekunden zurückgeben.
        /// @return Seit dem Start verstrichene Zeit in Sekunden
        /// @see _Duration
        inline time_t duration(void) const { return _Duration; }

        /// Den Wert der Membervariablen _Keep zurückliefern.
        /// @see _Keep
        inline bool kept(void) const { return _Keep; }

        /// Setzen der Entfernung (Meter).
        /// @see _Distance
        inline void setDistance(double distance) { _Distance = distance; }

        /// Setzen der Hearzfrequenz (Schläge pro Minute).
        /// @see _Heartrate
        inline void setHeartrate(IntValue heartrate) { _Heartrate = heartrate; }
        
        /// Setzen der Temperatur (° Celsius).
        /// @see _Temperature
        inline void setTemperature(DoubleValue temperature) { _Temperature = temperature; }
        
        /// Setzen der Höhe (Meter üNN).
        /// @see _Elevation
        inline void setElevation(DoubleValue elevation) { _Elevation = elevation; }

        /// Setzen des Anstiegs (Prozent)
        /// @see _Slope
        inline void setSlope(double slope) { _Slope = slope; }

        /// Setzen der Geschwindigkeit (km/h).
        /// @see _Speed
        inline void setSpeed(double speed) { _Speed = speed; }

        /// Setzen der Geschwindigkeit (min/km).
        /// @see _Pace
        inline void setPace(double pace) { _Pace = pace; }

        /// Setzen der Dauer seit Start (Sekunden)
        /// @see _Duration
        inline void setDuration(time_t duration) { _Duration = duration; }

        /// Setzen des Flags _Keep.
        /// @see _Keep
        inline void keep(bool flag = true) { _Keep = flag; }

        /// Umwandeln der geografischen Koordinaten des Trackpunkts in UTM-Koordinaten.
        inline operator UTMCoord() { return toUTM(); }

        /// Umwandeln der geografischen Koordinaten des Trackpunkts via UTM-Koordinaten in ein Point-Objekt.
        inline operator Point() { return toUTM(); }
    };

};

#endif // __TRACKPOINT_H_
