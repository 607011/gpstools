// $Id: Value.h bd86a16444b7 2008/08/27 15:09:21 Oliver Lau <ola@ct.heise.de> $
// Copyright (c) 2008 Oliver Lau <oliver@ersatzworld.net>
// Alle Rechte vorbehalten.

#ifndef __VALUE_H_
#define __VALUE_H_

#include <cassert>
#include <iostream>

namespace GPS {

    /// Die Template-Klasse Value beherbergt vornehmlich skalare Werte
    /// wie int, long, float oder double und zusätzlich die
    /// Eigenschaft, ob der Wert definiert ist oder nicht.
    template<typename T> class Value
    {
    private:
        /// Nur wenn diese Eigenschaft true ist, befindet sich in der
        /// Membervariablen _Value ein gültiger Wert.
        /// @see _Value
        bool _Defined;

        /// Der skalare Wert, den diese Template-Klasse verwaltet.
        T _Value;

    public:
        /// Konstruktor.
        inline Value<T>(void) : _Defined(false)
        { /* ... */ }

        /// Konstruktor.
        inline Value<T>(T value) : _Defined(true)
        {
            _Value = value;
        }

        /// Gibt true zurück, wenn der skalare Wert in der Membervariablen
        /// _Value gültig ist.
        /// @see _Value
        /// @see _Defined
        inline bool defined(void) const { return _Defined; }

        /// Der Aufruf dieser Methode invalidiert den gespeicherten Wert.
        inline void undef(void) { _Defined = false; }

        /// Die Membervariable _Value mit einem Wert initialisieren.
        inline Value<T>& operator=(T const v)
        {
            _Defined = true;
            _Value = v;
            return *this;
        }

        /// Die Membervariable _Value mit einem Wert initialisieren.
        inline Value<T>& operator=(const Value<T>& v)
        {
            _Defined = v.defined();
            _Value = v.value();
            return *this;
        }

        /// Einen Wert auf die Membervariable _Value addieren.
        inline Value<T>& operator+=(const Value<T>& v)
        {
            assert(_Defined);
            _Value += v.value();
            return *this;
        }

        /// Einen Wert von der Membervariable _Value subtrahieren.
        inline Value<T>& operator-=(const Value<T>& v)
        {
            assert(_Defined);
            _Value -= v.value();
            return *this;
        }

        /// Die Membervariable _Value mit einem anderen Wert multiplizieren.
        inline Value<T>& operator*=(const Value<T>& v)
        {
            assert(_Defined);
            _Value *= v.value();
            return *this;
        }

        /// Die Membervariable _Value durch einen anderen Wert teilen.
        inline Value<T>& operator/=(const Value<T>& v)
        {
            assert(_Defined);
            _Value /= v.value();
            return *this;
        }

        /// Die Membervariable _Value inkrementieren.
        inline Value<T>& operator++()
        {
            assert(_Defined);
            _Value += 1;
            return *this;
        }

        /// Die Membervariable _Value dekrementieren.
        inline Value<T>& operator--()
        {
            assert(_Defined);
            _Value -= 1;
            return *this;
        }	

        /// Die Membervariable _Value inkrementieren.
        inline const Value<T> operator++(int)
        {
            assert(_Defined);
            Value<T> old(*this);
            *this += 1;
            return old;
        }

        /// Die Membervariable _Value dekrementieren.
        inline const Value<T> operator--(int)
        {
            assert(_Defined);
            Value<T> old(*this);
            *this -= 1;
            return old;
        }

        /// Die Membervariable _Value setzen.
        /// @param value
        inline void set(T value)
        {
            _Defined = true;
            _Value = value;
        }

        /// Die Membervariable _Value zurückgeben.
        inline T value(void) const
        {
            // assert(_Defined);
            return _Value;
        }
    };


    /// Den Wert der Membervariablen _Value in einen Stream schreiben.
    /// @param os Der Stream, in den der Wert geschrieben werden soll.
    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Value<T>& v)
    {
        if (os.good())
            os << v.value();
        return os;
    }


    /// Die Membervariablen _Value mit einem aus einem Stream gelesenen
    /// Wert setzen.
    /// @param is Der Stream, aus dem der Wert gelesen werden soll.
    template<typename T>
    std::istream& operator>>(std::istream& is, Value<T>& v)
    {
        T value;
        if (is.good()) {
            is >> value;
            v = value;
        }
        return is;
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert kleiner ist als der rechte.
    template<typename T>
    bool operator<(const Value<T>& a, const Value<T>& b)
    {
        return a.value() < b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert kleiner ist als der rechte.
    template<typename T>
    bool operator<(const Value<T>& a, T b)
    {
        return a.value() < b;
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert kleiner ist als der rechte.
    template<typename T>
    bool operator<(T a, const Value<T>& b)
    {
        return a < b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert groesser ist als der rechte.
    template<typename T>
    bool operator>(const Value<T>& a, const Value<T>& b)
    {
        return a.value() > b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert groesser ist als der rechte.
    template<typename T>
    bool operator>(const Value<T>& a, T b)
    {
        return a.value() > b;
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert groesser ist als der rechte.
    template<typename T>
    bool operator>(T a, const Value<T>& b)
    {
        return a > b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert kleiner ist als der rechte
    /// oder identisch dazu
    template<typename T>
    bool operator<=(const Value<T>& a, const Value<T>& b)
    {
        return a.value() <= b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert kleiner ist als der rechte
    /// oder identisch dazu
    template<typename T>
    bool operator<=(const Value<T>& a, T b)
    {
        return a.value() <= b;
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert kleiner ist als der rechte
    /// oder identisch dazu
    template<typename T>
    bool operator<=(T a, const Value<T>& b)
    {
        return a <= b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert groesser ist als der rechte
    /// oder identisch dazu
    template<typename T>
    bool operator>=(const Value<T>& a, const Value<T>& b)
    {
        return a.value() >= b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert groesser ist als der rechte
    /// oder identisch dazu
    template<typename T>
    bool operator>=(const Value<T>& a, T b)
    {
        return a.value() >= b;
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn der linke Wert groesser ist als der rechte
    /// oder identisch dazu
    template<typename T>
    bool operator>=(T a, const Value<T>& b)
    {
        return a >= b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn beide Werte gleich sind.
    template<typename T>
    bool operator==(const Value<T>& a, const Value<T>& b)
    {
        return a.value() == b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn beide Werte gleich sind.
    template<typename T>
    bool operator==(const Value<T>& a, T b)
    {
        return a.value() == b;
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn beide Werte gleich sind.
    template<typename T>
    bool operator==(T a, const Value<T>& b)
    {
        return a == b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn die beiden Werte sich unterscheiden.
    template<typename T>
    bool operator!=(const Value<T>& a, const Value<T>& b)
    {
        return a.value() != b.value();
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn die beiden Werte sich unterscheiden.
    template<typename T>
    bool operator!=(const Value<T>& a, T b)
    {
        return a.value() != b;
    }

    /// Zwei Werte miteinander vergleichen.
    /// @return true, wenn die beiden Werte sich unterscheiden.
    template<typename T>
    bool operator!=(T a, const Value<T>& b)
    {
        return a != b.value();
    }

    /// Summe zweier Werte bilden.
    template<typename T>
    Value<T> operator+(const Value<T>& a, const Value<T>& b)
    { 
        Value<T> tmp(a);
        tmp += b;
        return tmp;
    }

    /// Summe zweier Werte bilden.
    template<typename T>
    Value<T> operator+(const Value<T>& a, T b)
    { 
        Value<T> tmp(a);
        tmp += b;
        return tmp;
    }

    /// Summe zweier Werte bilden.
    template<typename T>
    Value<T> operator+(T a, const Value<T>&  b)
    { 
        Value<T> tmp(a);
        tmp += b;
        return tmp;
    }

    /// Differenz zweier Werte bilden.
    template<typename T>
    Value<T> operator-(const Value<T>& a, const Value<T>& b)
    { 
        Value<T> tmp(a);
        tmp -= b;
        return tmp;
    }

    /// Differenz zweier Werte bilden.
    template<typename T>
    Value<T> operator-(const Value<T>& a, T b)
    { 
        Value<T> tmp(a);
        tmp -= b;
        return tmp;
    }

    /// Differenz zweier Werte bilden.
    template<typename T>
    Value<T> operator-(T a, const Value<T>& b)
    { 
        Value<T> tmp(a);
        tmp -= b;
        return tmp;
    }

    /// Produkt zweier Werte bilden.
    template<typename T>
    Value<T> operator*(const Value<T>& a, const Value<T>& b)
    { 
        Value<T> tmp(a);
        tmp *= b;
        return tmp;
    }

    /// Produkt zweier Werte bilden.
    template<typename T>
    Value<T> operator*(const Value<T>& a, T b)
    { 
        Value<T> tmp(a);
        tmp *= b;
        return tmp;
    }

    /// Produkt zweier Werte bilden.
    template<typename T>
    Value<T> operator*(T a, const Value<T>& b)
    { 
        Value<T> tmp(a);
        tmp *= b;
        return tmp;
    }

    /// Quotient zweier Werte bilden.
    template<typename T>
    Value<T> operator/(const Value<T>& a, const Value<T>& b)
    { 
        Value<T> tmp(a);
        tmp /= b;
        return tmp;
    }

    /// Quotient zweier Werte bilden.
    template<typename T>
    Value<T> operator/(const Value<T>& a, T b)
    { 
        Value<T> tmp(a);
        tmp /= b;
        return tmp;
    }

    /// Quotient zweier Werte bilden.
    template<typename T>
    Value<T> operator/(T a, const Value<T>& b)
    { 
        Value<T> tmp(a);
        tmp /= b;
        return tmp;
    }

    /// Value<char>
    typedef Value<char> CharValue;

    /// Value<unsigned char>
    typedef Value<unsigned char> UnsignedCharValue;

    /// Value<int>
    typedef Value<int> IntValue;

    /// Value<unsigned int>
    typedef Value<unsigned int> UnsignedIntValue;

    /// Value<long>
    typedef Value<long> LongValue;

    /// Value<long long>
    typedef Value<long long> LongLongValue;

    /// Value<unsigned long>
    typedef Value<unsigned long> UnsignedLongValue;

    /// Value<unsigned long long>
    typedef Value<unsigned long long> UnsignedLongLongValue;

    /// Value<float>
    typedef Value<float> FloatValue;

    /// Value<double>
    typedef Value<double> DoubleValue;

    /// Value<long double>
    typedef Value<long double> LongDoubleValue;

};

#endif // __VALUE_H_
