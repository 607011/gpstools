// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __STOCHASTICS_H_
#define __STOCHASTICS_H_

#include <cstdlib>
#include <vector>

namespace GPS {

    /// Die Klasse Stochastics stellt einfache Routinen zur statistischen Auswertung von Messwerten bereit.
    class Stochastics {
    private:
        std::vector<double> _Values;
        double _Sum;
        double _SumSquared;
        double _Average;
        double _Median;
        double _Variance;
        double _StandardDeviation;
        double _Min;
        double _Max;

    public:
        /// Konstruktor.
        Stochastics(void)
        { /* ... */ }

        /// Einen Messwerte zur Liste mit den Messungen hinzufügen.
        inline void add(double val)
        { 
            _Values.push_back(val);
        }

        /// Die Summe der Messwerte zurückgeben.
        inline double sum(void)const  { return _Sum; }

        /// Die Summe der quadrierten Messwerte zurückgeben.
        inline double sumSquared(void) const { return _SumSquared; }

        /// Den arithmetischen Mittelwert über die Messwerte zurückgeben.
        inline double average(void) const { return _Average; }

        /// Die Varianz der Messwerte zurückgeben.
        inline double variance(void) const { return _Variance; }

        /// Die Standardabweichung der Messwerte zurückgeben.
        inline double standardDeviation(void) const { return _StandardDeviation; }

        /// Den Median der Messwerte zurückgeben.
        inline double median(void) const { return _Median; }

        /// Den Wert des kleinsten Messwerts zurückgeben.
        inline double min(void) const { return _Min; }

        /// Den Wert des größten Messwerts zurückgeben.
        inline double max(void) const { return _Max; }

        /// Die Anzahl der Messwerte zurückgeben.
        inline size_t N(void) const { return _Values.size(); }

        /// Die Liste der Messwerte zurückgeben.
        inline const std::vector<double>& values(void) { return _Values; }

        /// Statistische Berechnungen über die Messwerte durchführen.
        void calculate(void);
    };

};

#endif //  __STOCHASTICS_H_
