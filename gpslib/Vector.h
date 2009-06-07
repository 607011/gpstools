// $Id$
// Copyright (c) 2008-2009 Oliver Lau <oliver@von-und-fuer-lau.de>
// Alle Rechte vorbehalten.

#ifndef __VECTOR_H_
#define __VECTOR_H_

namespace GPS
{
    class Point;

    /// Die Klasse Vector repräsentiert einen dreidimensionalen Vektor.
    class Vector {
    private:
        /// u
        double _U;

        /// v
        double _V;

        /// w
        double _W;

    public:
        /// Konstruktor.
        Vector(double u = 0.0, double v = 0.0, double w = 0.0) : _U(u), _V(v), _W(w)
        { /* ... */ }

        /// Konstruktor.
        Vector(const Point&, const Point&);

        /// Skalarprodukt aus zwei Vektoren bilden.
        friend double operator*(const Vector&, const Vector&);

        /// Einen Vektor von einem anderen abziehen.
        friend Vector operator-(const Vector&, const Vector&);

        /// Zwei Vektoren addieren.
        friend Vector operator+(const Vector&, const Vector&);
        
        /// Vektor durch Gleitkommazahl teilen.
        friend Vector operator/(const Vector&, double);

        /// Vektor mit Gleitkommazahl multiplizieren.
        friend Vector operator*(const Vector&, double);
        friend Vector operator*(double, const Vector&);

        /// u zurückgeben.
        /// @return u
        inline double u(void) const { return _U; }

        /// v zurückgeben.
        /// @return v
        inline double v(void) const { return _V; }

        /// w zurückgeben.
        /// @return w
        inline double w(void) const { return _W; }

        /// Länge des Vektor als doppeltgenaue Gleitkommazahl zurückgeben.
        /// @return Länge
        inline operator double() { return length(); }

        /// Länge des Vektor als Ganzzahl zurückgeben.
        /// @return Länge
        inline operator int() { return (int) length(); }

        /// Länge des Vektors zurückgeben.
        /// @return Länge
        double length(void) const;

        /// Normalisierten Vektor zurückgeben.
        /// @return normalisierter Vektor
        Vector normalized(void) const;

        /// Prüfen, ob alle Komponenten des Vektor auf 0 stehen.
        /// @return true, wenn Vektor gleich Null
        inline bool isNull(void) const 
        {
            return (_U == 0.0) && (_V == 0.0) && (_W == 0.0);
        }
    };

};

#endif // __VECTOR_H_
