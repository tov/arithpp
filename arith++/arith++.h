#pragma once

#include <iostream>
#include <limits>
#include <stdexcept>

namespace arithpp {

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

template <class From, class To, class P = Throwing_policy<To>,
          class Enable = void>
struct Convert;

//template <class From, class To, class P>
//struct Convert<From, To, P, std::enable_if_t<std::is_signed<From>::value &&
//                                             std::is_unsigned<To>::value &&
//                                             sizeof(To) >= sizeof(From)>>
//{
//
//};

//template <class To, class From, class P = Throwing_policy<To>, Enable=void>
//To checked_convert(From from);
//
//// From is signed, To is unsigned, To is same size or larger.
//template <class To, class From, class P>
//To checked_convert(
//        std::enable_if_t<
//                std::is_signed<From>::value &&
//                std::is_unsigned<To>::value &&
//                sizeof(To) >= sizeof(From),
//                From> from)
//{
//    if (from < 0) return P::too_small("checked_convert");
//
//    return static_cast<To>(from);
//}
//
//// From is signed, To is unsigned, To is smaller.
//template <class To, class From, class P>
//To checked_convert(
//        std::enable_if_t<
//                std::is_signed<From>::value &&
//                std::is_unsigned<To>::value &&
//                sizeof(To) < sizeof(From),
//                From> from)
//{
//    if (from < 0) return P::too_small("checked_convert");
//    if (from > static_cast<From>(std::numeric_limits<To>::max()))
//        return P::too_large("checked_convert");
//
//    return static_cast<To>(from);
//}

// Checked is currently specialized based on signedness.
template <class T, class P = Throwing_policy<T>, class Enable = void>
class Checked;

template <class T, class P>
class Checked<T, P, std::enable_if_t<std::is_signed<T>::value>>
{
private:
    using unsigned_t = std::make_unsigned_t<T>;

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
    Checked(T value = T())
    {
        value_ = value;
    }

    T get() const {
        return value_;
    }

    Checked operator-() const
    {
        if (std::is_signed<T>::value && value_ == min_())
            return P::too_big("Checked::operator-()");
        else
            return -value_;
    }

    unsigned_t abs() const
    {
        if (std::is_signed<T>::value) {
            if (value_ == min_()) {
                return unsigned_t(std::numeric_limits<T>::max()) + 1;
            } else if (value_ < 0) {
                return unsigned_t(-value_);
            } else {
                return unsigned_t(value_);
            }
        } else {
            return value_;
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

    Checked operator~() const
    {
        return ~value_;
    }

    Checked& operator+=(Checked other) {
        return *this = *this + other;
    }

    Checked& operator-=(Checked other) {
        return *this = *this - other;
    }

    Checked& operator*=(Checked other) {
        return *this = *this * other;
    }

    Checked& operator/=(Checked other) {
        return *this = *this / other;
    }

    Checked& operator%=(Checked other) {
        return *this = *this % other;
    }

    Checked& operator&=(Checked other) {
        return *this = *this & other;
    }

    Checked& operator|=(Checked other) {
        return *this = *this | other;
    }

    Checked& operator^=(Checked other) {
        return *this = *this | other;
    }

    Checked& operator<<=(u_int8_t other) {
        return *this = *this << other;
    }

    Checked& operator>>=(u_int8_t other) {
        return *this = *this >> other;
    }

    bool operator==(Checked other) const
    {
        return get() == other.get();
    }

    bool operator!=(Checked other) const
    {
        return get() != other.get();
    }

    bool operator<(Checked other) const
    {
        return get() < other.get();
    }

    bool operator>(Checked other) const
    {
        return get() > other.get();
    }

    bool operator<=(Checked other) const
    {
        return get() <= other.get();
    }

    bool operator>=(Checked other) const
    {
        return get() >= other.get();
    }

    Checked& operator++()
    {
        return *this += 1;
    }

    Checked& operator--()
    {
        return *this -= 1;
    }

    Checked& operator++(int)
    {
        Checked old = *this;
        ++*this;
        return old;
    }

    Checked& operator--(int)
    {
        Checked old = *this;
        --*this;
        return old;
    }
};

template <class T, class P>
class Checked<T, P, std::enable_if_t<std::is_unsigned<T>::value>>
{
private:
    T value_;

    static T zero_()
    {
        return T(0);
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
        if (value_ == zero_())
            return value_;
        else
            return P::too_small("Checked::operator-()");
    }

    T abs() const
    {
        return value_;
    }

    Checked operator+(Checked other) const
    {
        if (value_ > max_() - other.value_)
            return P::too_big("Checked::operator+");

        return value_ + other.value_;
    }

    Checked operator-(Checked other) const
    {
        if (other.value_ > value_)
            return P::too_small("Checked::operator-");

        return value_ - other.value_;
    }

    // This is slow right now, because it does a division. There are better
    // ways of doing it, depending on the size of T.
    Checked operator*(Checked other) const
    {
        if (other.value_ != 0) {
            if (value_ > max_() / other.value_)
                return P::too_big("Checked::operator*");
        }

        return value_ * other.value_;
    }

    Checked operator/(Checked other) const
    {
        return value_ / other.value_;
    }

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

    Checked operator~() const
    {
        return ~value_;
    }

    Checked& operator+=(Checked other) {
        return *this = *this + other;
    }

    Checked& operator-=(Checked other) {
        return *this = *this - other;
    }

    Checked& operator*=(Checked other) {
        return *this = *this * other;
    }

    Checked& operator/=(Checked other) {
        return *this = *this / other;
    }

    Checked& operator%=(Checked other) {
        return *this = *this % other;
    }

    Checked& operator&=(Checked other) {
        return *this = *this & other;
    }

    Checked& operator|=(Checked other) {
        return *this = *this | other;
    }

    Checked& operator^=(Checked other) {
        return *this = *this | other;
    }

    Checked& operator<<=(u_int8_t other) {
        return *this = *this << other;
    }

    Checked& operator>>=(u_int8_t other) {
        return *this = *this >> other;
    }

    bool operator==(Checked other) const
    {
        return get() == other.get();
    }

    bool operator!=(Checked other) const
    {
        return get() != other.get();
    }

    bool operator<(Checked other) const
    {
        return get() < other.get();
    }

    bool operator>(Checked other) const
    {
        return get() > other.get();
    }

    bool operator<=(Checked other) const
    {
        return get() <= other.get();
    }

    bool operator>=(Checked other) const
    {
        return get() >= other.get();
    }

    Checked& operator++()
    {
        return *this += 1;
    }

    Checked& operator--()
    {
        return *this -= 1;
    }

    Checked& operator++(int)
    {
        Checked old = *this;
        ++*this;
        return old;
    }

    Checked& operator--(int)
    {
        Checked old = *this;
        --*this;
        return old;
    }
};

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
    using unsigned_t = std::make_unsigned_t<T>;
    unsigned_t value_;

    explicit Wrapping(unsigned_t value) : value_(value)
    { }

public:
    Wrapping(T value) : value_{unsigned_t(value)}
    { }

    T get() const { return T(value_); }

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
            return unsigned_t(-get());
        } else {
            return unsigned_t(get());
        }
    }

    Wrapping operator+(Wrapping other) const
    {
        return value_ + other.value_;
    }

    Wrapping operator-(Wrapping other) const
    {
        return value_ - other.value_;
    }

    Wrapping operator*(Wrapping other) const
    {
        return value_ * other.value_;
    }

    Wrapping operator/(Wrapping other) const
    {
        return value_ / other.value_;
    }

    Wrapping operator%(Wrapping other) const
    {
        return value_ % other.value_;
    }

    Wrapping operator&(Wrapping other) const
    {
        return value_ & other.value_;
    }

    Wrapping operator|(Wrapping other) const
    {
        return value_ | other.value_;
    }

    Wrapping operator^(Wrapping other) const
    {
        return value_ ^ other.value_;
    }

    Wrapping operator<<(u_int8_t other) const
    {
        return value_ << other;
    }

    Wrapping operator>>(u_int8_t other) const
    {
        return get() >> other;
    }

    Wrapping operator~() const
    {
        return Wrapping(~value_);
    }

    Wrapping& operator+=(Wrapping other)
    {
        return *this = *this + other;
    }

    Wrapping& operator-=(Wrapping other) {
        return *this = *this - other;
    }

    Wrapping& operator*=(Wrapping other)
    {
        return *this = *this * other;
    }

    Wrapping& operator/=(Wrapping other)
    {
        return *this = *this / other;
    }

    Wrapping& operator%=(Wrapping other)
    {
        return *this = *this % other;
    }

    Wrapping& operator&=(Wrapping other)
    {
        return *this = *this & other;
    }

    Wrapping& operator|=(Wrapping other)
    {
        return *this = *this | other;
    }

    Wrapping& operator^=(Wrapping other)
    {
        return *this = *this ^ other;
    }

    Wrapping& operator<<=(u_int8_t other)
    {
        return *this = *this << other;
    }

    Wrapping& operator>>=(u_int8_t other)
    {
        return *this = *this >> other;
    }

    bool operator==(Wrapping other) const
    {
        return get() == other.get();
    }

    bool operator!=(Wrapping other) const
    {
        return get() != other.get();
    }

    bool operator<(Wrapping other) const
    {
        return get() < other.get();
    }

    bool operator>(Wrapping other) const
    {
        return get() > other.get();
    }

    bool operator<=(Wrapping other) const
    {
        return get() <= other.get();
    }

    bool operator>=(Wrapping other) const
    {
        return get() >= other.get();
    }

    Wrapping& operator++()
    {
        return *this += 1;
    }

    Wrapping& operator--()
    {
        return *this -= 1;
    }

    Wrapping& operator++(int)
    {
        Wrapping old = *this;
        ++*this;
        return old;
    }

    Wrapping& operator--(int)
    {
        Wrapping old = *this;
        --*this;
        return old;
    }
};

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
