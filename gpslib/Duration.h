// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#ifndef __DURATION_H_
#define __DURATION_H_

#include "Timestamp.h"

#ifdef WIN32
#include <Windows.h>
#endif

namespace GPS {

    class Timestamp;

    /// Dauer einer Handlung.
    /// Aufl�sung der Zeitmessung in Millisekunden.
    class Duration {

    private:
        /// Zeitstempel in Millisekunden
        timestamp_t ms;

        /// Zeitstempel des Startzeitpunkts.
#ifdef WIN32
        LARGE_INTEGER t0;
        LARGE_INTEGER freq;
#else
        Timestamp t0;
#endif

    public:
        /// Konstruktor.
        Duration(void) : ms(0) { }

        /// Konstruktor.
        /// @param _MS Millisekunden
        Duration(timestamp_t _MS) : ms(_MS) { }

        /// Konstruktor.
        /// @param _S Sekunden
        Duration(time_t _S) : ms(_S * 1000) { }

        /// Konstruktor.
        /// @param str Dauer als String. 
        /// Erlaubte Formate:
        ///        h:mm:ss
        ///        z.B. 6:00:48
        Duration(const char* str);

        /// Konstruktor.
        /// Dieser Konstruktor erf�llt dieselbe Aufgabe wie Duration(const char*),
        /// @param str Dauer als String. 
        /// @see Duration(const char* str)
        Duration(const std::string& str)
        {
            *this = str.c_str();
        }

        /// Konstruktor.
        Duration(int days, int hours, int minutes, int seconds, int milliseconds = 0);

        /// Konstruktor.
        Duration(int hours, int minutes, int seconds);

        /// Starten der Zeitmessung.
        /// @see stop
        void start(void);

        /// Stoppen der Zeitmessung.    
        /// @see start
        void stop(void);

        /// Zeitdauern addieren.
        /// @param ts Zeitstempel, der addiert werden soll.
        Duration& operator+=(const Duration& ts);

        /// Zeitdauern addieren.
        /// @param ms Millisekunden, die addiert werden sollen.
        Duration& operator+=(timestamp_t ms);

        /// Zeitdauern subtrahieren.
        /// @param ts Zeitdauer, die abgezogen werden soll.
        Duration& operator-=(const Duration& ts);

        /// Zeitdauern subtrahieren.
        /// @param ms Millisekunden, die abgezogen werden sollen.
        Duration& operator-=(timestamp_t ms);

        /// Zeitdauern auf Gleichheit pr�fen.
        /// @return true, wenn Zeitdauern identisch sind.
        friend bool operator==(const Duration&, const Duration&);

        /// Zeitdauern auf Ungleichheit pr�fen.
        /// @return true, wenn Zeitdauern voneinander abweichen.
        friend bool operator!=(const Duration&, const Duration&);

        /// Zeitdauern vergleichen.
        /// @return true, wenn die zweite Zeitdauer kleiner oder gleich der ersten ist.
        friend bool operator>=(const Duration&, const Duration&);

        /// Zeitdauern vergleichen.
        /// @return true, wenn die zweite Zeitdauer gr��er als die erste ist.
        friend bool operator<=(const Duration&, const Duration&);

        /// Zeitdauern vergleichen.
        /// @return true, wenn die zweite Zeitdauer kleiner als die erste ist.
        friend bool operator>(const Duration&, const Duration&);

        /// Zeitstempel vergleichen.
        /// @return true, wenn die erste Zeitdauer kleiner als die zweite ist.
        friend bool operator<(const Duration&, const Duration&);

        /// Differenz zweier Zeitdauern berechnen.
        /// @return Differenz
        friend const Duration operator-(const Duration&, const Duration&);

        /// Differenz zweier Zeitdauern berechnen.
        /// @return Differenz
        friend const Duration operator-(const Duration&, timestamp_t);

        /// Summe zweier Zeitdauern berechnen.
        /// @return Summe
        friend const Duration operator+(const Duration&, const Duration&);

        /// Summe zweier Zeitdauern berechnen.
        /// @return Summe
        friend const Duration operator+(const Duration&, timestamp_t);

        /// Zeitdauer (in Sekunden) in Stream ausgeben.
        friend std::ostream& operator<<(std::ostream&, const Duration&);

        /// true zur�ckgeben, wenn Zeitdauer gleich 0
        inline bool isNull(void) const { return ms == 0; }

        /// Zeitdauer in Sekunden zur�ckgeben.
        /// @return Wert der Zeitdauer in Sekunden
        inline time_t seconds(void) const { return ms / 1000ULL; }

        /// Zeitdauer in Millisekunden zur�ckgeben.
        /// @return Wert der Zeitdauer in Millisekunden
        inline timestamp_t milliseconds(void) const { return ms; }

        /// Zeitdauer vis strftime() als String formatieren und zur�ckgeben.
        /// Wenn kein Format-String angegeben wird, verwendet toString() "%H:%M:%S".
        /// @param fmt Format-String
        /// @return String-Repr�sentation der Zeitdauer.
        std::string toString(const char* fmt = NULL) const;

        /// Datum und Uhrzeit in Zeitstempel (Millisekunden) wandeln.
        /// @param days Tage
        /// @param hours Stunde
        /// @param minutes Minuten
        /// @param seconds Sekunden
        /// @param milliseconds Millisekunden
        /// @return Zeitdauer (Millisekunden)
        static timestamp_t toMs(int days, int hours, int minutes, int seconds, int milliseconds = 0);
    };

};

#endif // __DURATION_H_
