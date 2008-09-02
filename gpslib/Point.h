// $Id$
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __POINT_H_
#define __POINT_H_

#include <string>

namespace GPS {

    class Vector;
    class Line;

    /// Die Klasse Point repräsentiert einen Punkt in einem dreidimensionalen
    /// kartesischen Koordinatensystem.
    class Point {
    protected:
        /// x-Wert
        double _X;

        /// y-Wert
        double _Y;

        /// z-Wert
        double _Z;

    public:
        /// Konstruktor.
        Point(void) : _X(0), _Y(0), _Z(0)
        { /* ... */ }

        /// Konstruktor.
        /// @param x
        /// @param y
        /// @param z
        Point(double x, double y, double z = 0.0) : _X(x), _Y(y), _Z(z)
        { /* ... */ }

        /// Einen Punkt von einem anderen subtrahieren.
        /// Das Ergebnis der Subtraktion ist ein Vektor.
        /// @param p1 Punkt, von dem p2 abgezogen werden soll
        /// @param p2 Punkt, der von p1 abgezogen werden soll
        /// @return Vektor, der die Differenz zwischen den beiden Punkten repräsentiert.
        friend Vector operator-(const Point& p1, const Point& p2);

        friend Point operator-(const Point& p, const Vector& v);
        friend Point operator+(const Point& p, const Vector& v);
        friend Point operator*(const Point& p, double c);
        friend Point operator*(double c, const Point& p);
        friend Point operator/(const Point& p, double c);
        friend Point operator+(const Point& p1, const Point& p2);
        friend bool operator==(const Point& p1, const Point& p2);
        friend bool operator!=(const Point& p1, const Point& p2);

        Point& operator-=(const Vector& v);
        Point& operator+=(const Vector& v);
        Point& operator*=(double c);
        Point& operator/=(double c);

        /// Den senkrechten Abstand des Punktes zu der gedachten Geraden
        /// durch zwei andere Punkte berechnen.
        /// @param p1 Erster Punkt, durch den die gedachte Gerade verläuft.
        /// @param p2 Zweiter Punkt, durch den die gedachte Gerade verläuft.
        /// @return Senkrechter Abstand
        double perpendicularDistanceToLine(const Point& p1, const Point& p2) const;

        /// Den senkrechten Abstand des Punktes zu einer Geraden berechnen.
        /// @param line Gerade
        /// @return Senkrechter Abstand
        double perpendicularDistanceToLine(const Line& line) const;

        /// Den senkrechten Abstand des Punktes zu der gedachten Strecken
        /// zwischen zwei andere Punkten berechnen.
        /// @param p1 Erster Punkt, durch den die gedachte Strecke verläuft.
        /// @param p2 Zweiter Punkt, durch den die gedachte Strecke verläuft.
        /// @return Senkrechter Abstand
        double perpendicularDistanceToSegment(const Point& p1, const Point& p2) const;

        /// Den senkrechten Abstand des Punktes zu einer Strecke berechnen.
        /// @param line Strecke
        /// @return Senkrechter Abstand
        double perpendicularDistanceToSegment(const Line& line) const;

        inline bool isNull(void) const { return _X == 0 && _Y == 0 && _Z == 0; }

        /// X-Wert setzen.
        /// @param x X-Wert
        inline void setX(double x) { _X = x; }

        /// Y-Wert setzen.
        /// @param y Y-Wert
        inline void setY(double y) { _Y = y; }

        /// Z-Wert setzen.
        /// @param z Z-Wert
        inline void setZ(double z) { _Z = z; }

        /// X-Wert zurückgeben.
        /// @return X-Wert
        /// @see _Z
        inline double x(void) const { return _X; }

        /// Y-Wert zurückgeben.
        /// @return Y-Wert
        /// @see _Y
        inline double y(void) const { return _Y; }

        /// Z-Wert zurückgeben.
        /// @return Z-Wert
        /// @see _Z
        inline double z(void) const { return _Z; }

        /// Den Punkt in einen String wandeln.
        inline operator std::string() { return toString(); }

        /// Den Punkt in einen String wandeln.
        virtual std::string toString(void) const;

    };

};

#endif // __POINT_H_
