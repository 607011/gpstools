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

#include "AbstractWaypointList.h"
#include "Stochastics.h"
#include "Trackpoint.h"
#include "Vector.h"
#include "TrackLine.h"
#include "Duration.h"

namespace GPS {

#ifdef _MSC_VER
    static const double MIN_DBL = pow(10.0, (int) DBL_MIN_10_EXP + 1);
    static const double MAX_DBL = pow(10.0, (int) DBL_MAX_10_EXP - 1);
#else
    static const double MIN_DBL = -1e30;
    static const double MAX_DBL = +1e30;
#endif

    class Duration;

    typedef std::vector<Trackpoint*> TrackpointList;

    class Track;
    typedef std::vector<Track*> TrackList;


    /// Die Klasse Track verwaltet GPS-Tracks.
    class Track : public AbstractWaypointList<Trackpoint> {
    private:
        /// Den in samples enthaltenen Track nach dem Douglas-Peucker-
        /// Verfahren ausd�nnen, und zwar ausschlie�lich unter 
        /// Ber�cksichtigung der H�heninformationen. Diese Funktion
        /// eignet sich also nicht daf�r, den Track unter Beibehaltung
        /// des Wegprofils (L�ngen- und Breitengrad) auszud�nnen.
        /// @see DouglasPeucker2D()
        /// @see DouglasPeucker3D()
        void DouglasPeuckerSmooth(double epsilon, size_t lo, size_t hi);

        /// Den in samples enthaltenen Track nach dem Douglas-Peucker-
        /// Verfahren ausd�nnen, und zwar ausschlie�lich unter 
        /// Beruecksichtigung von Laengen- und Breitengrad. Diese Funktion
        /// eignet sich also nicht daf�r, den Track unter Beibehaltung
        /// des H�henprofils auszud�nnen.
        /// @see DouglasPeuckerSmooth()
        /// @see DouglasPeucker3D()
        void DouglasPeuckerWeed2D(double epsilon, size_t lo, size_t hi);

        /// Den in samples enthaltenen Track nach dem Douglas-Peucker-
        /// Verfahren ausd�nnen, und zwar so, dass sowohl H�hen- als
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

    public:
        /// Diese mit einem logischen Oder zu verkn�pfenden Flags geben an, 
        /// welche Informationen die Methode Track::merge() in den Trackpunkt 
        /// �bertragen soll.
        /// @see merge()
        enum _MergeFlag
        {
            GEOCOORDS = 1 << 0,
            ELEVATION = 1 << 1,
            HEARTRATE = 1 << 2,
        };

        /// Mindestabstand zwischen zwei Trackpunkten f�r die Geschwindigkeitsberechnung
        /// @see calculateSpeeds()
        static const double DEFAULT_METERS_INTERVAL;

        /// Mindestzeit zwischen zwei Trackpunkten f�r die Geschwindigkeitsberechnung
        /// @see calculateSpeeds()
        static const time_t DEFAULT_SECONDS_INTERVAL;

    public:
        /// Konstruktor.
        Track(void);

        /// Copy-Konstruktor.
        /// @param other Track, dessen s�mtliche Daten inklusive Kopien seiner Trackpunkte in das
        /// neue Objekt �bertragen werden sollen.
        Track(const Track& other);

        /// Anzahl der im Track als "behalten" gekennzeichneten Punkte zur�ckgeben.
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

        /// F�r jeden Trackpunkt die Sekunden berechnen, die
        /// bis dahin seit dem Start vergangen sind.
        void calculateDurations(void);

        /// Kumulierte An- und Abstiege berechnen sowie
        /// die geringste und h�chste Erhebung ermitteln.
        /// @see _Ascent
        /// @see _Descent
        /// @see _MinElevation
        /// @see _MaxElevation
        void calculateAscentDescent(void);

        /// F�r jeden Trackpunkt die Entfernung berechnen,
        /// die seit dem Start bis dahin zur�ckgelegt wurde.
        void calculateDistances(void);

        /// F�r jeden Trackpunkt die Steigung in Prozent zum
        /// naechsten Trackpunkt berechnen und dem Trackpunkt
        /// zuweisen.
        /// Die Steigung errechnet sich aus dem Quotienten 
        /// der Hoehendifferenz zur Entfernung. Damit diese
        /// Funktion korrekte Werte liefert, muss also
        /// vorher die Methode calculateDistances() ausgefuehrt
        /// worden sein.
        /// @see calculateDistances()
        void calculateSlopes(void);

        /// F�r jeden Trackpunkt die Geschwindigkeit in km/h 
        /// berechnen und dem Trackpunkt zuweisen.
        /// Die Geschwindigkeit berechnet sich aus dem Quotienten
        /// der Entfernungsdifferenz zur Zeitdifferenz. Damit diese
        /// Funktion korrekte Werte liefert, muss also vorher die
        /// Methode calculateDistances() ausgef�hrt worden sein.
        /// Um den Messfehlern bei der Positionsbestimmung 
        /// entgegenzuwirken und damit die Genauigkeit bei der
        /// Geschwindigkeitsberechnung zu erh�hen, wird die
        /// Geschwindigkeit immer dann berechnet, wenn mindestens
        /// secsInterval Sekunden seit dem zuletzt betrachteten
        /// Trackpunkt verstrichen sind. Diese Geschwindigkeit
        /// wird allen Trackpunkten vom zuletzt ber�cksichtigten
        /// bis zum aktuellen zugewiesen.
        /// @param secsInterval Anzahl der Sekunden, die mindestens
        /// zwischen zwei Trackpunkten verstrichen sein m�ssen.
        /// @see calculateDistances()
        void calculateSpeeds(time_t secsInterval = DEFAULT_SECONDS_INTERVAL);

        /// F�r jeden Trackpunkt die Geschwindigkeit in km/h 
        /// berechnen und dem Trackpunkt zuweisen.
        /// @param metersInterval Strecke in Metern, die mindestens
        /// zwischen den zwei Trackpunkten zur�ckgelegt wurde.
        void calculateSpeeds(double metersInterval = DEFAULT_METERS_INTERVAL);

        /// Werte aus einem anderen Track �bertragen.
        /// @param other Track, aus dem die Werte geholt werden sollen. 
        /// @param what Flag: gibt an, welche Werte �bertragen 
        /// werden sollen.
        /// @see Trackpoint::_MergeFlag
        void merge(const Track* other, int what = HEARTRATE);

        /// Startzeit zur�ckgeben.
        /// @return Startzeit
        Timestamp startTimestamp(void) const;

        /// Zielzeit zur�ckgeben.
        /// @return Zielzeit
        Timestamp finishTimestamp(void) const;

        /// Kumulierte Anstiege (in Metern) zur�ckgeben.
        /// @return Kumulierte Anstiege (in Metern)
        const DoubleValue& ascent(void) const
        {
            return _Ascent;
        }

        /// Kumulierte Abstiege (in Metern) zur�ckgeben.
        /// @return Kumulierte Abstiege (in Metern)
        const DoubleValue& descent(void) const
        {
            return _Descent;
        }

        /// H�he (in Metern �NN) des niedrigsten Punkts zur�ckgeben.
        /// @return geringste H�he (in Metern �NN)
        const DoubleValue& minElevation(void) const
        {
            return _MinElevation;
        }

        /// H�he (in Metern �NN) des h�chsten Punkts zur�ckgeben.
        /// @return gr��te H�he (in Metern �NN)
        const DoubleValue& maxElevation(void) const
        {
            return _MaxElevation;
        }

        /// Entfernung in Metern zur�ckgeben.
        /// @return Entfernung in Metern
        double distance(void) const;

        /// Dauer in Sekunden zur�ckgeben.
        /// @return Dauer in Sekunden.
        time_t duration(void) const;

        /// Mittlere Herzfrequenz (Schl�ge/min) zur�ckgeben.
        /// @return Mittlere Herzfrequenz (Schl�ge/min).
        DoubleValue avgHeartrate(void) const;

        /// Mittlere Temperatur (� Celsius) zur�ckgeben.
        /// @return Mittlere Temperatur (� Celsius).
        DoubleValue avgTemperature(void) const;

        /// Mittlere Geschwindigkeit (km/h) zur�ckgeben.
        /// @return Mittlere Geschwindigkeit (km/h)
        DoubleValue avgSpeed(void) const;

        /// Mittleren Anstieg in Prozent zur�ckgeben.
        /// @return Mittlerer Anstieg in Prozent
        DoubleValue avgUphillSlope(void) const;

        /// Mittleren Abstieg in Prozent zur�ckgeben.
        /// @return Mittlerer Abstieg in Prozent
        DoubleValue avgDownhillSlope(void) const;

        /// S�mtliche als kept() markierte Trackpunkte in einen neuen
        /// Track �bertragen und diesen Track zur�ckgeben.
        /// Der Aufrufer ist daf�r verantwortlich, den Track mit delete zu
        /// l�schen, wenn er nicht mehr gebraucht wird.
        /// @return Track
        Track* remaining(void);

        /// Den Track in der Horizontalen ausd�nnen. Diese Methode verwendet
        /// zur Ausd�nnung den Douglas-Peucker-Algorithmus.
        /// @param epsilon Schwellwert (in Metern) f�r den Douglas-Peucker-Algorithmus
        /// @param ret Wenn true, werden die beibehaltenen Trackpunkte �ber die
        /// Methode remaining() zu einem neuen Track zusammengefasst und
        /// zur�ckgegeben. In diesem Fall ist der Aufrufer daf�r verantwortlich, den
        /// Track mit delete zu l�schen, falls er nicht mehr ben�tigt wird.
        /// Wenn false, liefert die Methode NULL zur�ck.
        /// @return Ausged�nnter Track
        Track* weed2D(double epsilon, bool ret = false);

        /// Den Track in der Horizontalen und Vertikalen ausd�nnen. Diese Methode verwendet
        /// zur Ausd�nnung den Douglas-Peucker-Algorithmus.
        /// @param epsilon Schwellwert (in Metern) f�r den Douglas-Peucker-Algorithmus
        /// @param ret Wenn true, werden die beibehaltenen Trackpunkte �ber die
        /// Methode remaining() zu einem neuen Track zusammengefasst und
        /// zur�ckgegeben. In diesem Fall ist der Aufrufer daf�r verantwortlich, den
        /// Track mit delete zu l�schen, falls er nicht mehr ben�tigt wird.
        /// Wenn false, liefert die Methode NULL zur�ck.
        /// @return Ausged�nnter Track
        Track* weed3D(double epsilon, bool ret = false);

        /// Den Track in der Vertikalen ausd�nnen. Diese Methode verwendet
        /// zur Ausd�nnung den Douglas-Peucker-Algorithmus.
        /// @param epsilon Schwellwert (in Metern) f�r den Douglas-Peucker-Algorithmus
        /// @param ret Wenn true, werden die beibehaltenen Trackpunkte �ber die
        /// Methode remaining() zu einem neuen Track zusammengefasst und
        /// zur�ckgegeben. In diesem Fall ist der Aufrufer daf�r verantwortlich, den
        /// Track mit delete zu l�schen, falls er nicht mehr ben�tigt wird.
        /// Wenn false, liefert die Methode NULL zur�ck.
        /// @return Ausged�nnter Track
        Track* smoothDouglasPeucker(double epsilon, bool ret = false);

        /// Das H�henprofil des Tracks gl�tten. 
        /// Diese Gl�ttungsfunktion ber�cksichtigt dabei nur H�hendifferenzen
        /// von mindestens epsilon Metern.
        /// @param epsilon Schwellwert
        /// @return Gegl�tteter Track
        Track* smoothThreshold(double epsilon);

        /// Die H�heninformationen aller Trackpunkte um die angegebenen
        /// Anzahl Meter verschieben.
        /// @param m Meter, um die die H�heninformationen aller Trackpunkte
        /// verschoben werden sollen.
        void shiftElevation(double m);

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

        /// Die Zeitstempel aller Trackpunkte um das angegebene Ma� verschieben.
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

        /// Eine Trackpunkt �ber seinen Zeitstempel suchen.
        /// Diese Methode erlaubt unscharfe und exakte Suche. 
        /// @param ts Zu suchender Zeitstempel.
        /// @param exact Wenn true, wird ein Trackpunkt nur dann gefunden, wenn
        /// sein Zeitstempel exakt (d.h. auf die Sekunde genau) mit dem gesuchten �bereinstimmt.
        /// Wenn false, liefert die Methode den zeitlich n�chstliegenden Trackpunkt zur�ck.
        /// @return Trackpunkt; NULL, wenn kein passender Trackpunkt gefunden werden konnte
        /// oder der Track leer ist.
        Trackpoint* getMatchingTrackpoint(const Timestamp& ts, bool exact = false) const;

        /// Den Trackpunkt suchen, der den �bergebenen geografischen Koordinaten am n�chsten liegt.
        /// @param coord Zu suchende geografische Koordinaten.
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// �bergebenen entfernt liegen
        /// @return Trackpunkt
        Trackpoint* getMatchingTrackpoint(const GeoCoord* coord, double maxRange = MAX_DBL) const;

        /// Den Trackpunkt suchen, der den �bergebenen geografischen Koordinaten am n�chsten liegt.
        /// @param lon L�ngengrad der zu suchenden geografischen Koordinate.
        /// @param lat Breitengrad der zu suchenden geografischen Koordinate.
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// �bergebenen entfernt liegen
        /// @return Trackpunkt
        Trackpoint* getMatchingTrackpoint(double lon, double lat, double maxRange = MAX_DBL) const;

        /// Den Trackpunkt suchen, der den �bergebenen geografischen Koordinaten am n�chsten liegt.
        /// @param lon L�ngengrad der zu suchenden geografischen Koordinate.
        /// @param lat Breitengrad der zu suchenden geografischen Koordinate.
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// �bergebenen entfernt liegen
        /// @return Trackpunkt-Iterator
        TrackpointList::iterator getMatchingTrackpointIter(double lon, double lat, double maxRange = MAX_DBL);
    
        /// Den Trackpunkt suchen, der der �bergebenen Strecke am n�chsten liegt.
        /// @param line Strecke
        /// @return Trackpunkt-Iterator
        TrackpointList::iterator getMatchingTrackpointIter(const TrackLine& line);

        /// Die Verbindungslinie zwischen zwei Trackpunkten suchen, die dem �bergebenen 
        /// Trackpunkt am n�chsten liegt.
        /// @param r Referenz-Trackpunkt
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// �bergebenen entfernt liegen
        /// @return Verbindungslinie
        TrackLine getMatchingLine(Trackpoint* const r, double maxRange) const;

        /// Die Verbindungslinie zwischen zwei Trackpunkten suchen, die dem �bergebenen 
        /// Trackpunkt am n�chsten liegt.
        /// @param lon L�ngengrad des Trackpunkts
        /// @param lat Breitengrad des Trackpunkts
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Meter vom
        /// �bergebenen entfernt liegen
        /// @return Verbindungslinie
        TrackLine getMatchingLine(double lon, double lat, double maxRange) const;

        /// Die Verbindungslinie zwischen zwei Trackpunkten suchen, die dem �bergebenen 
        /// Trackpunkt am n�chsten liegt. Gerechnet wird nicht mit L�ngen- und Breitengraden,
        /// sondern mit Rechts- und Hochwerten (kartesisches Koordinatensystem).
        /// @param x x-Koordinate
        /// @param y y-Koordinate
        /// @param maxRange der zu suchende Punkt darf nicht weiter als maxRange Einheiten vom
        /// �bergebenen entfernt liegen
        /// @return Verbindungslinie
        TrackLine getMatchingLineXY(double x, double y, double maxRange) const;

        /// Die �bereinstimmung des Tracks zu einem Referenztrack mit einfachen statistischen
        /// Methoden beschreiben.
        /// Der Algorithmus sucht f�r jeden Punkt den k�rzesten senkrechten Abstand 
        /// zu den Linien, die sich aus der Verbindung s�mtlicher Punktpaare des
        /// Referenztracks ergeben, und f�llt damit eine Messergebnisliste. Die Messwerte
        /// werden statistisch ausgewertet, so dass sich anschlie�end Angaben etwa �ber den
        /// gr��ten und kleinsten Abstand, die Varianz, die Standardabweichung oder den
        /// Mittelwert machen lassen.
        /// @param ref Referenztrack
        /// @return Statistische Auswertung der Messwerte mit den kleinsten Abst�nden.
        /// Wenn NULL, hat entweder dieser oder der Referenztrack zu wenig Punkte (<3),
        /// weshalb ein Vergleich nicht m�glich ist.
        /// @see Stochastics
        Stochastics* compareTo(Track* const ref) const;

        /// Alle Trackpunkte als "behalten" markieren.
        void keepAll(void);

        /// Alle Trackpunkte als "verwerfen" markieren.
        void keepNone(void);

        /// @return true, wenn der Track Herzfrequenzdaten enth�lt.
        bool hasHeartrate(void) const;

        /// @return true, wenn der Track Geschwindigkeitsdaten enth�lt.
        bool hasSpeed(void) const;

        /// @return true, wenn der Track H�hendaten enth�lt.
        bool hasElevation(void) const;

        /// @return true, wenn der Track Entfernungsdaten enth�lt.
        bool hasDistance(void) const;

        /// @return true, wenn der Track Zeitstempel enth�lt.
        bool hasTimestamps(void) const;


    };

};

#endif // __TRACK_H_
