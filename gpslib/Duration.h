// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __DURATION_H_
#define __DURATION_H_

#include "Timestamp.h"

namespace GPS {

    class Timestamp;

    class Duration {

    private:
        /// Zeitstempel in Millisekunden
        timestamp_t ms;

    public:
        /// Konstruktor.
        Duration(void) : ms(0) { }

        /// Konstruktor.
        /// @param ms Millisekunden
        Duration(timestamp_t _MS) : ms(_MS) { };

        /// Konstruktor.
        /// @param str Dauer als String. 
        /// Erlaubte Formate:
        ///        h:mm:ss
        ///        z.B. 6:00:48
        Duration(const char* str);

        /// Konstruktor.
        /// Dieser Konstruktor erfüllt dieselbe Aufgabe wie Duration(const char*),
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

        /// Zeitdauern auf Gleichheit prüfen.
        /// @return true, wenn Zeitdauern identisch sind.
        friend bool operator==(const Duration&, const Duration&);

        /// Zeitdauern auf Ungleichheit prüfen.
        /// @return true, wenn Zeitdauern voneinander abweichen.
        friend bool operator!=(const Duration&, const Duration&);

        /// Zeitdauern vergleichen.
        /// @return true, wenn die zweite Zeitdauer kleiner oder gleich der ersten ist.
        friend bool operator>=(const Duration&, const Duration&);

        /// Zeitdauern vergleichen.
        /// @return true, wenn die zweite Zeitdauer größer als die erste ist.
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

        /// true zurückgeben, wenn Zeitdauer gleich 0
        inline bool isNull(void) const { return ms == 0; }

        /// Zeitdauer in Sekunden zurückgeben.
        /// @return Wert der Zeitdauer in Sekunden
        inline time_t seconds(void) const { return ms / 1000ULL; }

        /// Zeitdauer in Millisekunden zurückgeben.
        /// @return Wert der Zeitdauer in Millisekunden
        inline timestamp_t milliseconds(void) const { return ms; }

        /// Zeitdauer vis strftime() als String formatieren und zurückgeben.
        /// Wenn kein Format-String angegeben wird, verwendet toString() "%H:%M:%S".
        /// @param fmt Format-String
        /// @return String-Repräsentation der Zeitdauer.
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
