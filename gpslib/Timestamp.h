// $Id: Timestamp.h c546ffd259f3 2008/08/29 15:38:11 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <ooliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __TIMESTAMP_H_
#define __TIMESTAMP_H_

#include <string>

namespace GPS {

    /// Typ für millisekundengenaue Zeitstempel.
    typedef unsigned long long timestamp_t;

    /// Die Klasse Timestamp verwaltet millisekundengenaue Zeitstempel.
    class Timestamp
    {
    private:
        /// Zeitstempel in Millisekunden
        timestamp_t ms;

    public:
        /// Konstruktor.
        Timestamp(void);

        /// Konstruktor.
        /// @param str Zeitstempel als String. 
        /// Erlaubte Formate:
        ///        YYYY-MM-DDThh:mm:ssZ
        ///        z.B. 2008-06-07T06:00:48Z
        ///
        ///        YYYY-MM-DDThh:mm:ssTZD
        ///        e.g. 1997-07-16T19:20:30+01:00
        ///
        ///        YYYY-MM-DDThh:mm:ss.sTZD
        ///        e.g. 1997-07-16T19:20:30.45+01:00
        ///
        ///        YYYY = four-digit year
        ///        MM   = two-digit month (01=January, etc.)
        ///        DD   = two-digit day of month (01 through 31)
        ///        hh   = two digits of hour (00 through 23) (am/pm NOT allowed)
        ///        mm   = two digits of minute (00 through 59)
        ///        ss   = two digits of second (00 through 59)
        ///        s    = one or more digits representing a decimal fraction of a second
        ///        TZD  = time zone designator (Z or +hh:mm or -hh:mm)
        ///
        /// @see http://www.w3.org/TR/NOTE-datetime
        Timestamp(const char* str);

        /// Konstruktor.
        /// Dieser Konstruktor erfüllt dieselbe Aufgabe wie Timestamp(const char*),
        /// @param str Zeitstempel als String. 
        /// @see Timestamp(const char* str)
        Timestamp(const std::string& str);

        /// Konstruktor.
        Timestamp(int year, int month, int day, int hours, int minutes, int seconds, int offsethours = 0, int offsetminutes = 0);

        /// Konstruktor.
        /// @param msSinceEpoch Verstrichene Millisekunden seit dem 01.01.1970 00:00 UTC
        Timestamp(timestamp_t msSinceEpoch);

        /// Zeitstempel addieren.
        /// @param ts Zeitstempel, der addiert werden soll.
        Timestamp& operator+=(const Timestamp& ts);

        /// Zeitstempel addieren.
        /// @param ms Millisekunden, die addiert werden sollen.
        Timestamp& operator+=(timestamp_t ms);

        /// Zeitstempel subtrahieren.
        /// @param ts Zeitstempel, der abgezogen werden soll.
        Timestamp& operator-=(const Timestamp& ts);

        /// Zeitstempel subtrahieren.
        /// @param ms Millisekunden, die abgezogen werden sollen.
        Timestamp& operator-=(timestamp_t ms);

        /// Zeitstempel auf Gleichheit prüfen.
        /// @return true, wenn Zeitstempel identisch sind.
        friend bool operator==(const Timestamp&, const Timestamp&);

        /// Zeitstempel auf Ungleichheit prüfen.
        /// @return true, wenn Zeitstempel voneinander abweichen.
        friend bool operator!=(const Timestamp&, const Timestamp&);

        /// Zeitstempel vergleichen.
        /// @return true, wenn der zweite Zeitstempel kleiner oder gleich dem ersten ist.
        friend bool operator>=(const Timestamp&, const Timestamp&);

        /// Zeitstempel vergleichen.
        /// @return true, wenn der zweite Zeitstempel größer als der erste ist.
        friend bool operator<=(const Timestamp&, const Timestamp&);

        /// Zeitstempel vergleichen.
        /// @return true, wenn der zweite Zeitstempel kleiner als der erste ist.
        friend bool operator>(const Timestamp&, const Timestamp&);

        /// Zeitstempel vergleichen.
        /// @return true, wenn der erste Zeitstempel kleiner als der zweite ist.
        friend bool operator<(const Timestamp&, const Timestamp&);

        /// Differenz zweier Zeitstempel berechnen.
        /// @return Differenz
        friend const Timestamp operator-(const Timestamp&, const Timestamp&);

        /// Differenz zweier Zeitstempel berechnen.
        /// @return Differenz
        friend const Timestamp operator-(const Timestamp&, timestamp_t);

        /// Summe zweier Zeitstempel berechnen.
        /// @return Summe
        friend const Timestamp operator+(const Timestamp&, const Timestamp&);

        /// Summe zweier Zeitstempel berechnen.
        /// @return Summe
        friend const Timestamp operator+(const Timestamp&, timestamp_t);

        /// Zeitstempel (in Sekunden) in Stream ausgeben.
        friend std::ostream& operator<<(std::ostream&, const Timestamp&);

        /// true zurückgeben, wenn Zeitstempel gleich 0
        bool isNull(void) const { return ms == 0; }

        /// Zeitstempel in Sekunden zurückgeben.
        /// @return Wert des Zeitstempels in Sekunden
        time_t seconds(void) const;

        /// Zeitstempel in Millisekunden zurückgeben.
        /// @return Wert des Zeitstempels in Millisekunden
        timestamp_t milliseconds(void) const;

        /// Zeitstempel vis strftime() als String formatieren und zurückgeben.
        /// Wenn kein Format-String angegeben wird, verwendet toString()
        /// "%Y-%m-%dT%H:%M:%SZ".
        /// @param fmt Format-String
        /// @see strftime
        /// @return String-Repräsentation des Zeitstempels.
        std::string toString(const char* fmt = NULL) const;

        /// Datum und Uhrzeit in Zeitstempel (Millisekunden) wandeln.
        /// @param year Jahr
        /// @param month Monat
        /// @param day Tag des Monats
        /// @param hours Stunden des Tages
        /// @param minutes Minuten der Stunde
        /// @param seconds Sekunden der Minute
        /// @param offsethours Stunden der Zeitzone
        /// @param offsetminutes Minuten der Zeitzone
        /// @return Zeitstempel (Millisekunden)
        static timestamp_t toMs(int year, int month, int day,
            int hours, int minutes, int seconds = 0,
            int offsethours = 0, int offsetminutes = 0);

        /// Den aktuellen Zeitstempel (Datum/Uhrzeit) als Timestamp zurückgeben.
        /// @return Datum/Uhrzeit als Zeitstempel
        static Timestamp current(void);

        /// Ein Zeit-Offset im Format "[+-]hh:mm:ss" in Sekunden wandeln.
        /// @param offset Zeit-Offset im Format "[+-]hh:mm:ss"
        /// @return Sekunden, denen das Offset entspricht.
        static time_t offsetToSeconds(std::string offset);

    };

};

#endif // __TIMESTAMP_H_
