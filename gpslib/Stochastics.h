// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
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

        /// Einen Messwerte zur Liste mit den Messungen hinzuf�gen.
        inline void add(double val)
        { 
            _Values.push_back(val);
        }

        /// Die Summe der Messwerte zur�ckgeben.
        inline double sum(void)const  { return _Sum; }

        /// Die Summe der quadrierten Messwerte zur�ckgeben.
        inline double sumSquared(void) const { return _SumSquared; }

        /// Den arithmetischen Mittelwert �ber die Messwerte zur�ckgeben.
        inline double average(void) const { return _Average; }

        /// Die Varianz der Messwerte zur�ckgeben.
        inline double variance(void) const { return _Variance; }

        /// Die Standardabweichung der Messwerte zur�ckgeben.
        inline double standardDeviation(void) const { return _StandardDeviation; }

        /// Den Median der Messwerte zur�ckgeben.
        inline double median(void) const { return _Median; }

        /// Den Wert des kleinsten Messwerts zur�ckgeben.
        inline double minimum(void) const { return _Min; }

        /// Den Wert des gr��ten Messwerts zur�ckgeben.
        inline double maximum(void) const { return _Max; }

        /// Die Anzahl der Messwerte zur�ckgeben.
        inline size_t N(void) const { return _Values.size(); }

        /// Die Liste der Messwerte zur�ckgeben.
        inline const std::vector<double>& values(void) { return _Values; }

        /// Statistische Berechnungen �ber die Messwerte durchf�hren.
        void calculate(void);
    };

};

#endif //  __STOCHASTICS_H_
