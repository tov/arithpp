#pragma once

#include <xxint.hxx>
#include <iostream>
#include <stdexcept>

namespace rational {

// Represents a rational number, that is, a fraction.
class Rational
{
public:
    // Constructs the rational representing 0
    Rational();

    // Constructs the rational representing `n`
    Rational(long n);

    // Constructs the rational representing `n/d`; throws
    // std::overflow_error if `d` is 0.
    Rational(long n, long d);

    // Note: Only three arithmetic operations are members, and the rest
    // are free functions declared below. The reason is that these three
    // operations access the representation directly, but the other
    // arithmetic operations work via getters and constructors.

    // Negation -r
    Rational operator-() const;

    // Reciprocal 1/r
    Rational reciprocal() const;

    // Multiplication.
    Rational operator*(Rational) const;

    // Addition.
    Rational operator+(Rational) const;

    // Gets the numerator
    long numerator() const { return num_.get(); }

    // Gets the denominator
    long denominator() const { return den_.get(); }

private:
    using repr_t = xxint::Checked<long>;
    repr_t num_, den_;
    // invariants:
    //   den > 0
    //   gcd(num, den) == 1
};

bool operator==(Rational, Rational);
bool operator!=(Rational, Rational);
bool operator<(Rational, Rational);
bool operator<=(Rational, Rational);
bool operator>(Rational, Rational);
bool operator>=(Rational, Rational);

std::ostream& operator<<(std::ostream&, Rational);
double to_double(Rational);

Rational operator-(Rational, Rational);
Rational operator/(Rational, Rational);

Rational& operator+=(Rational&, Rational);
Rational& operator-=(Rational&, Rational);
Rational& operator*=(Rational&, Rational);
Rational& operator/=(Rational&, Rational);

Rational& operator++(Rational&);
Rational& operator--(Rational&);
Rational operator++(Rational&, int);
Rational operator--(Rational&, int);

}
