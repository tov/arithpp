#pragma once

#include <iostream>
#include <limits>
#include <stdexcept>

namespace arithpp {

template<class T>
struct Make_unsigned;

#define SPECIALIZE_MAKE_UNSIGNED(S, U) \
    template <> struct Make_unsigned<S> { using type = U; }

SPECIALIZE_MAKE_UNSIGNED(char, unsigned char);
SPECIALIZE_MAKE_UNSIGNED(signed char, unsigned char);
SPECIALIZE_MAKE_UNSIGNED(short, unsigned short);
SPECIALIZE_MAKE_UNSIGNED(int, unsigned int);
SPECIALIZE_MAKE_UNSIGNED(long, unsigned long);
SPECIALIZE_MAKE_UNSIGNED(long long, unsigned long long);

template<class T>
using make_unsigned_t = typename Make_unsigned<T>::type;

template <class T>
struct Saturating_policy
{
    static T too_big(const char*)
    {
        return std::numeric_limits<T>::max();
    }

    static T too_small(const char*)
    {
        return std::numeric_limits<T>::min();
    }
};

template <class T, class E = std::overflow_error>
struct Throwing_policy
{
    static T too_big(const char* who)
    {
        throw E(who);
    }

    static T too_small(const char* who)
    {
        throw E(who);
    }
};

template <class T, class P = Throwing_policy<T>>
class Checked
{
private:
    using unsigned_t = make_unsigned_t<T>;
    T value_;

    static T min_()
    {
        return std::numeric_limits<T>::min();
    }

    static T max_()
    {
        return std::numeric_limits<T>::max();
    }

public:
    Checked(T value) : value_(value)
    { }

    T get() const { return value_ ; }

    Checked operator-() const
    {
        if (value_ == min_())
            return P::too_big("Checked::operator-()");
        else
            return -value_;
    }

    unsigned_t abs() const
    {
        if (value_ == min_()) {
            return unsigned_t(std::numeric_limits<T>::max()) + 1;
        } else if (value_ < 0) {
            return unsigned_t(-value_);
        } else {
            return unsigned_t(value_);
        }
    }

    Checked operator+(Checked other) const
    {
        if (other.value_ >= 0) {
            if (value_ > max_() - other.value_)
                return P::too_big("Checked::operator+");
        } else {
            if (value_ < min_() - other.value_)
                return P::too_small("Checked::operator+");
        }

        return value_ + other.value_;
    }

    Checked operator-(Checked other) const
    {
        if (value_ >= 0) {
            if (other.value_ < value_ - max_())
                return P::too_big("Checked::operator-");
        } else {
            if (other.value_ > value_ - min_())
                return P::too_small("Checked::operator-");
        }

        return value_ - other.value_;
    }

    // This is slow right now, because it does a division. There are better
    // ways of doing it, depending on the size of T.
    Checked operator*(Checked other) const
    {
        if (other.value_ != 0) {
            if (abs() > unsigned_t(max_()) / other.abs()) {
                if ((value_ > 0 && other.value_ > 0) ||
                        (value_ < 0 && other.value_ < 0))
                    return P::too_big("Checked::operator*");
                else
                    return P::too_small("Checked::operator*");
            }
        }

        return value_ * other.value_;
    }

    Checked operator/(Checked other) const
    {
        if (value_ == min_() && other.value_ == -1)
            return P::too_big("Checked::operator/");

        if (other.value_ == 0) {
            // In saturating mode, 0 / 0 is T_MAX.
            if (value_ >= 0)
                return P::too_big("Checked::operator/");
            else
                return P::too_small("Checked::operator/");
        }

        return value_ / other.value_;
    }

    // Should this do some kind of checking?
    Checked operator%(Checked other) const
    {
        return value_ % other.value_;
    }

    Checked operator&(Checked other) const
    {
        return value_ & other.value_;
    }

    Checked operator|(Checked other) const
    {
        return value_ | other.value_;
    }

    Checked operator^(Checked other) const
    {
        return value_ ^ other.value_;
    }

    Checked operator~() const
    {
        return ~value_;
    }

    Checked operator<<(u_int8_t other) const
    {
        T mask = max_() - (max_() >> 1);
        T result = value_;

        // This is very inefficient.
        while (other > 0) {
            if (result & mask)
                return P::too_big("Checked::operator<<");

            result <<= 1;
            --other;
        }

        return result;
    }

    Checked operator>>(u_int8_t other) const
    {
        return value_ >> other;
    }
};

template <class T, class P>
bool operator==(Checked<T, P> a, Checked<T, P> b)
{
    return a.get() == b.get();
}

template <class T, class P>
bool operator!=(Checked<T, P> a, Checked<T, P> b)
{
    return a.get() != b.get();
}

template <class T, class P>
bool operator<(Checked<T, P> a, Checked<T, P> b)
{
    return a.get() < b.get();
}

template <class T, class P>
bool operator>(Checked<T, P> a, Checked<T, P> b)
{
    return a.get() > b.get();
}

template <class T, class P>
bool operator<=(Checked<T, P> a, Checked<T, P> b)
{
    return a.get() <= b.get();
}

template <class T, class P>
bool operator>=(Checked<T, P> a, Checked<T, P> b)
{
    return a.get() >= b.get();
}

template <class T, class P>
std::ostream& operator<<(std::ostream& o, Checked<T, P> a)
{
    return o << a.get();
}

template <class T, class P>
std::istream& operator>>(std::istream& i, Checked<T, P>& a) {
    T temp;
    i >> temp;
    a = Checked<T, P>(temp);
    return i;
}

template <class T>
using Saturating = Checked<T, Saturating_policy<T>>;

template <class T>
class Wrapping
{
private:
    using unsigned_t = make_unsigned_t<T>;
    unsigned_t value_;

    explicit Wrapping(unsigned_t value) : value_(value)
    { }

public:
    Wrapping(T value) : value_(static_cast<unsigned_t>(value))
    { }

    T get() const { return static_cast<T>(value_); }

    Wrapping operator-() const {
        if (get() == std::numeric_limits<T>::min())
            return Wrapping(std::numeric_limits<T>::min());
        else
            return Wrapping(-get());
    }

    unsigned_t abs() const
    {
        if (get() == std::numeric_limits<T>::min()) {
            return unsigned_t(std::numeric_limits<T>::max()) + 1;
        } else if (get() < 0) {
            return static_cast<unsigned_t>(-get());
        } else {
            return static_cast<unsigned_t>(get());
        }
    }

    Wrapping operator+(Wrapping other) const {
        return Wrapping(value_ + other.value_);
    }

    Wrapping operator-(Wrapping other) const {
        return Wrapping(value_ - other.value_);
    }

    Wrapping operator*(Wrapping other) const {
        return Wrapping(value_ * other.value_);
    }

    Wrapping operator/(Wrapping other) const {
        return Wrapping(value_ / other.value_);
    }

    Wrapping operator%(Wrapping other) const {
        return Wrapping(value_ % other.value_);
    }

    Wrapping operator&(Wrapping other) const {
        return Wrapping(value_ & other.value_);
    }

    Wrapping operator|(Wrapping other) const {
        return Wrapping(value_ | other.value_);
    }

    Wrapping operator^(Wrapping other) const {
        return Wrapping(value_ ^ other.value_);
    }

    Wrapping operator<<(u_int8_t other) const {
        return Wrapping(value_ << other);
    }

    Wrapping operator>>(u_int8_t other) const {
        return Wrapping(get() >> other);
    }

    Wrapping operator~() const {
        return Wrapping(~value_);
    }

    Wrapping& operator+=(Wrapping other) {
        value_ += other.value_;
        return *this;
    }

    Wrapping& operator-=(Wrapping other) {
        value_ -= other.value_;
        return *this;
    }

    Wrapping& operator*=(Wrapping other) {
        value_ *= other.value_;
        return *this;
    }

    Wrapping& operator/=(Wrapping other) {
        value_ /= other.value_;
        return *this;
    }

    Wrapping& operator%=(Wrapping other) {
        value_ %= other.value_;
        return *this;
    }

    Wrapping& operator&=(Wrapping other) {
        value_ &= other.value_;
        return *this;
    }

    Wrapping& operator|=(Wrapping other) {
        value_ |= other.value_;
        return *this;
    }

    Wrapping& operator^=(Wrapping other) {
        value_ ^= other.value_;
        return *this;
    }

    Wrapping& operator<<=(Wrapping other) {
        value_ <<= other.value_;
        return *this;
    }

    Wrapping& operator>>=(Wrapping other) {
        return *this = Wrapping(get() >> other);
    }
};

template <class T>
bool operator==(Wrapping<T> a, Wrapping<T> b)
{
    return a.get() == b.get();
}

template <class T>
bool operator!=(Wrapping<T> a, Wrapping<T> b)
{
    return a.get() != b.get();
}

template <class T>
bool operator<(Wrapping<T> a, Wrapping<T> b)
{
    return a.get() < b.get();
}

template <class T>
bool operator>(Wrapping<T> a, Wrapping<T> b)
{
    return a.get() > b.get();
}

template <class T>
bool operator<=(Wrapping<T> a, Wrapping<T> b)
{
    return a.get() <= b.get();
}

template <class T>
bool operator>=(Wrapping<T> a, Wrapping<T> b)
{
    return a.get() >= b.get();
}

template <class T>
std::ostream& operator<<(std::ostream& o, Wrapping<T> a)
{
    return o << a.get();
}

template <class T>
std::istream& operator>>(std::istream& i, Wrapping<T>& a) {
    T temp;
    i >> temp;
    a = Wrapping<T>(temp);
    return i;
}

}
