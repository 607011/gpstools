// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __TRACK_H_
#define __TRACK_H_

#include <limits>
#include <vector>
#include <ostream>
#include <cassert>
#include <cmath>

#include "Stochastics.h"
#include "Trackpoint.h"
#include "Vector.h"
#include "TrackLine.h"

namespace GPS {

#ifdef _MSC_VER
    static const double MIN_DBL = pow(10.0f, (int) DBL_MIN_10_EXP + 1);
    static const double MAX_DBL = pow(10.0f, (int) DBL_MAX_10_EXP - 1);
#else
    static const double MIN_DBL = -1e30;
    static const double MAX_DBL = +1e30;
#endif

    typedef std::vector<Waypoint*> Route;
    typedef std::vector<Waypoint*> WaypointList;

    class Track;

    typedef std::vector<Trackpoint*> TrackpointList;
    typedef std::vector<Route*> RouteList;
    typedef std::vector<Track*> TrackList;

    /// Die Klasse Track verwaltet GPS-Tracks.
    class Track {
    private:
        /// Den in samples enthaltenen Track nach dem Douglas-Peucker-
        /// Verfahren ausdünnen, und zwar ausschließlich unter 
        /// Berücksichtigung der Höheninformationen. Diese Funktion
        /// eignet sich also nicht dafür, den Track unter Beibehaltung
        /// des Wegprofils (Längen- und Breitengrad) auszudünnen.
        /// @see DouglasPeucker2D()
        /// @see DouglasPeucker3D()
        void DouglasPeuckerSmooth(double epsilon, size_t lo, size_t hi);

        /// Den in samples enthaltenen Track nach dem Douglas-Peucker-
        /// Verfahren ausdünnen, und zwar ausschließlich unter 
        /// Beruecksichtigung von Laengen- und Breitengrad. Diese Funktion
        /// eignet sich also nicht dafür, den Track unter Beibehaltung
        /// des Höhenprofils auszudünnen.
        /// @see DouglasPeuckerSmooth()
        /// @see DouglasPeucker3D()
        void DouglasPeuckerWeed2D(double epsilon, size_t lo, size_t hi);

        /// Den in samples enthaltenen Track nach dem Douglas-Peucker-
        /// Verfahren ausdünnen, und zwar so, dass sowohl Höhen- als
        /// auch Wegprofil erhalten bleiben. 
        /// @see DouglasPeucker2D()
        /// @see DouglasPeuckerSmooth()
        void DouglasPeuckerWeed3D(double epsilon, size_t lo, size_t hi);

    private:
        /// Geringste im Track enthaltene Hoehe in Meter.
        /// Wird durch Aufruf von calculateAscentDescent() ermittelt.
        DoubleValue _MinElevation;

        /// Groesste im Track enthaltene Hoehe in Meter
        /// Wird durch Aufruf von calculateAscentDescent() ermittelt.
        DoubleValue _MaxElevation;

        /// Kumulierte Anstiege in Meter.
        /// Wird durch Aufruf von calculateAscentDescent() ermittelt.
        DoubleValue _Ascent;

        /// Kumulierte Abstiege in Meter.
        /// Wird durch Aufruf von calculateAscentDescent() ermittelt.
        DoubleValue _Descent;

        /// Name des Tracks.
        std::string _Name;

        /// Chronologisch sortierte Liste der Trackpunkte.
        TrackpointList samples;

    public:
        /// Diese mit einem logischen Oder zu verknüpfenden Flags geben an, 
        /// welche Informationen die Methode Track::merge() in den Trackpunkt 
        /// übertragen soll.
        /// @see merge()
        enum _MergeFlag
        {
            GEOCOORDS = 1 << 0,
            ELEVATION = 1 << 1,
            HEARTRATE = 1 << 2,
        };

        /// Mindestabstand zwischen zwei Trackpunkten für die Geschwindigkeitsberechnung
        /// @see calculateSpeeds()
        static const double DEFAULT_METERS_INTERVAL;

        /// Mindestzeit zwischen zwei Trackpunkten für die Geschwindigkeitsberechnung
        /// @see calculateSpeeds()
        static const time_t DEFAULT_SECONDS_INTERVAL;

    public:
        /// Konstruktor.
        Track(void);

        /// Copy-Konstruktor.
        /// @param other Track, dessen sämtliche Daten inklusive Kopien seiner Trackpunkte in das
        /// neue Objekt übertragen werden sollen.
        Track(const Track& other);

        /// Trackpunkt-Liste zurückgeben.
        /// @return Trackpunkt-Liste
        /// @see samples
        inline TrackpointList& points(void) { return samples; }

        /// Anzahl der im Track als "behalten" gekennzeichneten Punkte zurückgeben.
        /// @return Anzahl der Trackpunkte
        /// @see kept()
        size_t keptCount(void) const;

        /// Trackpunkt-Liste nachverarbeiten.
        /// @see calculateDurations()
        /// @see calculateAscentDescent()
        /// @see calculateDistances()
        /// @see calculateSlopes()
        /// @see calculateSpeeds()
        void postProcess(void);

        /// Für jeden Trackpunkt die Sekunden berechnen, die
        /// bis dahin seit dem Start vergangen sind.
        void calculateDurations(void);

        /// Kumulierte An- und Abstiege berechnen sowie
        /// die geringste und höchste Erhebung ermitteln.
        /// @see _Ascent
        /// @see _Descent
        /// @see _MinElevation
        /// @see _MaxElevation
        void calculateAscentDescent(void);

        /// Für jeden Trackpunkt die Entfernung berechnen,
        /// die seit dem Start bis dahin zurückgelegt wurde.
        void calculateDistances(void);

        /// Für jeden Trackpunkt die Steigung in Prozent zum
        /// naechsten Trackpunkt berechnen und dem Trackpunkt
        /// zuweisen.
        /// Die Steigung errechnet sich aus dem Quotienten 
        /// der Hoehendifferenz zur Entfernung. Damit diese
        /// Funktion korrekte Werte liefert, muss also
        /// vorher die Methode calculateDistances() ausgefuehrt
        /// worden sein.
        /// @see calculateDistances()
        void calculateSlopes(void);

        /// Für jeden Trackpunkt die Geschwindigkeit in km/h 
        /// berechnen und dem Trackpunkt zuweisen.
        /// Die Geschwindigkeit berechnet sich aus dem Quotienten
        /// der Entfernungsdifferenz zur Zeitdifferenz. Damit diese
        /// Funktion korrekte Werte liefert, muss also vorher die
        /// Methode calculateDistances() ausgeführt worden sein.
        /// Um den Messfehlern bei der Positionsbestimmung 
        /// entgegenzuwirken und damit die Genauigkeit bei der
        /// Geschwindigkeitsberechnung zu erhöhen, wird die
        /// Geschwindigkeit immer dann berechnet, wenn mindestens
        /// secsInterval Sekunden seit dem zuletzt betrachteten
        /// Trackpunkt verstrichen sind. Diese Geschwindigkeit
        /// wird allen Trackpunkten vom zuletzt berücksichtigten
        /// bis zum aktuellen zugewiesen.
        /// @param secsInterval Anzahl der Sekunden, die mindestens
        /// zwischen zwei Trackpunkten verstrichen sein müssen.
        /// @see calculateDistances()
        void calculateSpeeds(time_t secsInterval = DEFAULT_SECONDS_INTERVAL);

        /// Für jeden Trackpunkt die Geschwindigkeit in km/h 
        /// berechnen und dem Trackpunkt zuweisen.
        /// @param metersInterval Strecke in Metern, die mindestens
        /// zwischen den zwei Trackpunkten zurückgelegt wurde.
        void calculateSpeeds(double metersInterval = DEFAULT_METERS_INTERVAL);

        /// Werte aus einem anderen Track übertragen.
        /// @param other Track, aus dem die Werte geholt werden sollen. 
        /// @param what Flag: gibt an, welche Werte übertragen 
        /// werden sollen.
        /// @see Trackpoint::_MergeFlag
        void merge(const Track* other, int what = HEARTRATE);

        /// Startzeit zurückgeben.
        /// @return Startzeit
        Timestamp startTimestamp(void) const;

        /// Zielzeit zurückgeben.
        /// @return Zielzeit
        Timestamp finishTimestamp(void) const;

        /// Kumulierte Anstiege (in Metern) zurückgeben.
        /// @return Kumulierte Anstiege (in Metern)
        const DoubleValue& ascent(void) const
        {
            return _Ascent;
        }

        /// Kumulierte Abstiege (in Metern) zurückgeben.
        /// @return Kumulierte Abstiege (in Metern)
        const DoubleValue& descent(void) const
        {
            return _Descent;
        }

        /// Höhe (in Metern üNN) des niedrigsten Punkts zurückgeben.
        /// @return geringste Höhe (in Metern üNN)
        const DoubleValue& minElevation(void) const
        {
            return _MinElevation;
        }

        /// Höhe (in Metern üNN) des höchsten Punkts zurückgeben.
        /// @return größte Höhe (in Metern üNN)
        const DoubleValue& maxElevation(void) const
        {
            return _MaxElevation;
        }

        /// Entfernung in Metern zurückgeben.
        /// @return Entfernung in Metern
        double distance(void) const;

        /// Durch den Track eingeschlossene Fläche in Quadratmetern zurückgeben.
        /// UNGETESTET!
        /// @return Fläche in Quadratmetern
        double area(void) const;

        /// Dauer in Sekunden zurückgeben.
        /// @return Dauer in Sekunden.
        time_t duration(void) const;

        /// Mittlere Herzfrequenz (Schläge/min) zurückgeben.
        /// @return Mittlere Herzfrequenz (Schläge/min).
        DoubleValue avgHeartrate(void) const;

        /// Mittlere Temperatur (° Celsius) zurückgeben.
        /// @return Mittlere Temperatur (° Celsius).
        DoubleValue avgTemperature(void) const;

        /// Mittlere Geschwindigkeit (km/h) zurückgeben.
        /// @return Mittlere Geschwindigkeit (km/h)
        DoubleValue avgSpeed(void) const;

        /// Mittleren Anstieg in Prozent zurückgeben.
        /// @return Mittlerer Anstieg in Prozent
        DoubleValue avgUphillSlope(void) const;

        /// Mittleren Abstieg in Prozent zurückgeben.
        /// @return Mittlerer Abstieg in Prozent
        DoubleValue avgDownhillSlope(void) const;

        /// Sämtliche als kept() markierte Trackpunkte in einen neuen
        /// Track übertragen und diesen Track zurückgeben.
        /// Der Aufrufer ist dafür verantwortlich, den Track mit delete zu
        /// löschen, wenn er nicht mehr gebraucht wird.
        /// @return Track
        Track* remaining(void);

        /// Den Track in der Horizontalen ausdünnen. Diese Methode verwendet
        /// zur Ausdünnung den Douglas-Peucker-Algorithmus.
        /// @param epsilon Schwellwert (in Metern) für den Douglas-Peucker-Algorithmus
        /// @param ret Wenn true, werden als beibehaltenen Trackpunkte über die
        /// Methode remaining() zu einem neuen Track zusammengefasst und
        /// zurückgegeben. In diesem Fall ist der Aufrufer dafür verantwortlich, den
        /// Track mit delete zu löschen, falls er nicht mehr benötigt wird.
        /// Wenn false, liefert die Methode NULL zurück.
        /// @return Ausgedünnter Track
        Track* weed2D(double epsilon, bool ret = false);

        /// Den Track in der Horizontalen und Vertikalen ausdünnen. Diese Methode verwendet
        /// zur Ausdünnung den Douglas-Peucker-Algorithmus.
        /// @param epsilon Schwellwert (in Metern) für den Douglas-Peucker-Algorithmus
        /// @param ret Wenn true, werden als beibehaltenen Trackpunkte über die
        /// Methode remaining() zu einem neuen Track zusammengefasst und
        /// zurückgegeben. In diesem Fall ist der Aufrufer dafür verantwortlich, den
        /// Track mit delete zu löschen, falls er nicht mehr benötigt wird.
        /// Wenn false, liefert die Methode NULL zurück.
        /// @return Ausgedünnter Track
        Track* weed3D(double epsilon, bool ret = false);

        /// Den Track in der Vertikalen ausdünnen. Diese Methode verwendet
        /// zur Ausdünnung den Douglas-Peucker-Algorithmus.
        /// @param epsilon Schwellwert (in Metern) für den Douglas-Peucker-Algorithmus
        /// @param ret Wenn true, werden als beibehaltenen Trackpunkte über die
        /// Methode remaining() zu einem neuen Track zusammengefasst und
        /// zurückgegeben. In diesem Fall ist der Aufrufer dafür verantwortlich, den
        /// Track mit delete zu löschen, falls er nicht mehr benötigt wird.
        /// Wenn false, liefert die Methode NULL zurück.
        /// @return Ausgedünnter Track
        Track* smoothDouglasPeucker(double epsilon, bool ret = false);

        /// Das Höhenprofil des Tracks glätten. 
        /// Diese Glättungsfunktion berücksichtigt dabei nur Höhendifferenzen
        /// von mindestens epsilon Metern.
        /// @param epsilon Schwellwert
        /// @return Geglätteter Track
        Track* smoothThreshold(double epsilon);

        /// Die Zeitstempel aller Trackpunkte um die angegebenen
        /// Anzahl Sekunden verschieben.
        /// @param seconds Anzahl der Sekunden, um die die
        /// Zeitstempel aller Trackpunkte verschoben werden sollen.
        void shiftTimestamps(time_t seconds);

        /// Die Zeitstempel aller Trackpunkte um die angegebenen
        /// Anzahl Millisekunden verschieben.
        /// @param milliseconds Anzahl der Millisekunden, um die die
        /// Zeitstempel aller Trackpunkte verschoben werden sollen.
        void shiftTimestamps(timestamp_t milliseconds);

        /// Die Zeitstempel aller Trackpunkte um das angegebene Maß verschieben.
        /// @param amount Zeitverschiebung im Format [+-]hh:mm:ss
        void shiftTimestamps(std::string amount);

        /// Die Zeitstempel aller Trackpunkte auf ein bestimmtes Datum verschieben.
        /// Die Zeitintervalle zwischen den Trackpunkten bleiben erhalten.
        /// @param timestamp Zeitstempel, auf den verschoben werden soll.
        void shiftTimestamps(const Timestamp& timestamp);

        /// Track-Daten in einen Stream ausgeben.
        /// @param os Stream
        /// @param msg Nachricht
        /// @param dumpTrackpoints wenn true, Dumps aller Trackpunkte ausgeben.
        void dump(std::ostream& os, std::string msg, bool dumpTrackpoints = false);

        /// Den Track in kleinere Tracks aufspalten mit nicht mehr als n Punkten.
        /// @param n Maximale Anzahl der Punkte, die ein Track haben darf.
        /// @return Chronologisch sortierte Liste der Teil-Tracks.
        TrackList* split(size_t n);

        /// Eine Trackpunkt über seinen Zeitstempel suchen.
        /// Diese Methode erlaubt unscharfe und exakte Suche. 
        /// @param ts Zu suchender Zeitstempel.
        /// @param exact Wenn true, wird ein Trackpunkt nur dann gefunden, wenn
        /// sein Zeitstempel exakt (d.h. auf die Sekunde genau) mit dem gesuchten übereinstimmt.
        /// Wenn false, liefert die Methode den zeitlich nächstliegenden Trackpunkt zurück.
        /// @return Trackpunkt; NULL, wenn kein passender Trackpunkt gefunden werden konnte
        /// oder der Track leer ist.
        Trackpoint* getMatchingTrackpoint(const Timestamp& ts, bool exact = false) const;

        /// Den Trackpunkt suchen, der den übergebenen geografischen Koordinaten am nächsten liegt.
        /// @param coord Zu suchende geografische Koordinaten.
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// übergebenen entfernt liegen
        /// @return Trackpunkt
        Trackpoint* getMatchingTrackpoint(const GeoCoord* coord, double maxRange = MAX_DBL) const;

        /// Den Trackpunkt suchen, der den übergebenen geografischen Koordinaten am nächsten liegt.
        /// @param lon Längengrad der zu suchenden geografischen Koordinate.
        /// @param lat Breitengrad der zu suchenden geografischen Koordinate.
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// übergebenen entfernt liegen
        /// @return Trackpunkt
        Trackpoint* getMatchingTrackpoint(double lon, double lat, double maxRange = MAX_DBL) const;

        /// Den Trackpunkt suchen, der den übergebenen geografischen Koordinaten am nächsten liegt.
        /// @param lon Längengrad der zu suchenden geografischen Koordinate.
        /// @param lat Breitengrad der zu suchenden geografischen Koordinate.
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// übergebenen entfernt liegen
        /// @return Trackpunkt-Iterator
        TrackpointList::iterator getMatchingTrackpointIter(double lon, double lat, double maxRange = MAX_DBL);
    
        /// Den Trackpunkt suchen, der der übergebenen Strecke am nächsten liegt.
        /// @param line Strecke
        /// @return Trackpunkt-Iterator
        TrackpointList::iterator getMatchingTrackpointIter(const TrackLine& line);

        /// Einen Trackpunkt einfügen.
        /// @param i Stelle, an der der Trackpunkt in den Track eingefügt werden soll.
        /// @param t Einzufügender Trackpunkt.
        void insert(TrackpointList::iterator i, Trackpoint* t);

        /// Die Verbindungslinie zwischen zwei Trackpunkten suchen, die dem übergebenen 
        /// Trackpunkt am nächsten liegt.
        /// @param r Referenz-Trackpunkt
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// übergebenen entfernt liegen
        /// @return Verbindungslinie
        TrackLine getMatchingLine(Trackpoint* const r, double maxRange) const;

        /// Die Verbindungslinie zwischen zwei Trackpunkten suchen, die dem übergebenen 
        /// Trackpunkt am nächsten liegt.
        /// @param lon Längengrad des Trackpunkts
        /// @param lat Breitengrad des Trackpunkts
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// übergebenen entfernt liegen
        /// @return Verbindungslinie
        TrackLine getMatchingLine(double lon, double lat, double maxRange) const;

        /// Die Verbindungslinie zwischen zwei Trackpunkten suchen, die dem übergebenen 
        /// Trackpunkt am nächsten liegt. Gerechnet wird nicht mit Längen- und Breitengraden,
        /// sondern mit Rechts- und Hochwerten (kartesisches Koordinatensystem).
        /// @param x x-Koordinate
        /// @param y y-Koordinate
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Einheiten vom
        /// übergebenen entfernt liegen
        /// @return Verbindungslinie
        TrackLine getMatchingLineXY(double x, double y, double maxRange) const;

        /// Die Übereinstimmung des Tracks zu einem Referenztrack mit einfachen statistischen
        /// Methoden beschreiben.
        /// Der Algorithmus sucht für jeden Punkt den kürzesten senkrechten Abstand 
        /// zu den Linien, die sich aus der Verbindung sämtlicher Punktpaare des
        /// Referenztracks ergeben, und füllt damit eine Messergebnisliste. Die Messwerte
        /// werden statistisch ausgewertet, so dass sich anschließend Angaben etwa über den
        /// größten und kleinsten Abstand, die Varianz, die Standardabweichung oder den
        /// Mittelwert machen lassen.
        /// @param ref Referenztrack
        /// @return Statistische Auswertung der Messwerte mit den kleinsten Abständen.
        /// Wenn NULL, hat entweder dieser oder der Referenztrack zu wenig Punkte (<3),
        /// weshalb ein Vergleich nicht möglich ist.
        /// @see Stochastics
        Stochastics* compareTo(Track* const ref) const;

        /// Alle Trackpunkte als "behalten" markieren.
        void keepAll(void);

        /// Alle Trackpunkte als "verwerfen" markieren.
        void keepNone(void);

        /// Letzten Punkt aus der Trackpunkt-Liste entfernen.
        void pop(void);

        /// Ermitteln, ob die Trackpunkt-Liste Trackpunkte enthält.
        /// @return true, wenn die Trackpunkt-Liste leer ist.
        inline bool isEmpty(void) const
        {
            return samples.size() == 0;
        }

        /// @return true, wenn der Track Herzfrequenzdaten enthält.
        bool hasHeartrate(void) const;

        /// @return true, wenn der Track Geschwindigkeitsdaten enthält.
        bool hasSpeed(void) const;

        /// @return true, wenn der Track Höhendaten enthält.
        bool hasElevation(void) const;

        /// @return true, wenn der Track Entfernungsdaten enthält.
        bool hasDistance(void) const;

        /// @return true, wenn der Track Zeitstempel enthält.
        bool hasTimestamps(void) const;

        /// Namen des Tracks zurückgeben.
        inline const std::string& name(void) const
        {
            return _Name;
        }

        /// Namen des Tracks setzen.
        inline void setName(std::string name)
        {
            _Name = name;
        }

        /// Track-Daten löschen.
        inline void clear(void)
        {
            samples.clear();
        }

        /// Einen Trackpunkt aus dem Track entfernen.
        /// @param trkpt Zu entfernender Trackpunkt.
        inline void erase(TrackpointList::iterator trkpt)
        {
            samples.erase(trkpt);
        }

        /// Einen Trackpunkt an den Track anhängen.
        /// @param trkpt Anzuhängender Trackpunkt. NULL-Zeiger werden ignoriert.
        inline void append(Trackpoint* trkpt)
        {
            if (trkpt != NULL)
                samples.push_back(trkpt);
        }

    };


};

#endif // __TRACK_H_
