#pragma once

#include <iostream>
#include <limits>
#include <stdexcept>

namespace arithpp {

// Make sure we have two's complement numbers, because Wrapping<T> depends on
// conversion to unsigned and back:
static_assert(static_cast<unsigned int>(-3) == UINT_MAX - 2,
              "Two's complement check");
static_assert(static_cast<int>(UINT_MAX - 2) == -3,
              "Two's complement check");

struct overflow_too_large : std::overflow_error
{
    using overflow_error::overflow_error;
};

struct overflow_too_small : std::overflow_error
{
    using overflow_error::overflow_error;
};

struct overflow_div_zero : std::overflow_error
{
    using overflow_error::overflow_error;
};

template <class T>
struct Saturating_policy
{
    static T too_large(const char*)
    {
        return std::numeric_limits<T>::max();
    }

    static T too_small(const char*)
    {
        return std::numeric_limits<T>::min();
    }

    static T div_zero(const char* who)
    {
        throw overflow_div_zero(who);
    }
};

template <class T>
struct Throwing_policy
{
    static T too_large(const char* who)
    {
        throw overflow_too_large(who);
    }

    static T too_small(const char* who)
    {
        throw overflow_too_small(who);
    }

    static T div_zero(const char* who)
    {
        throw overflow_div_zero(who);
    }
};

template <class To,
          class From,
          template <class> class Policy = Throwing_policy,
          class Enable = void>
struct Convert;

template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<std::is_signed<From>::value &&
                         std::is_unsigned<To>::value &&
                         sizeof(From) <= sizeof(To)>>
{
    static To convert(From from)
    {
        if (from < 0) return Policy<To>::too_small("Convert");
        return static_cast<To>(from);
    }
};

template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<std::is_signed<From>::value &&
                         std::is_unsigned<To>::value &&
                         sizeof(To) < sizeof(From)>>
{
    static To convert(From from)
    {
        if (from < 0) return Policy<To>::too_small("Convert");
        if (from > static_cast<From>(std::numeric_limits<To>::max()))
            return Policy<To>::too_large("Convert");
        return static_cast<To>(from);
    }
};

template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<std::is_signed<From>::value == std::is_signed<To>::value &&
                         sizeof(From) <= sizeof(To)>>
{
    static To convert(From from)
    {
        return static_cast<To>(from);
    }

    static To widen(From from)
    {
        return static_cast<To>(from);
    }
};

template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<std::is_signed<From>::value == std::is_signed<To>::value &&
                         sizeof(To) < sizeof(From)>>
{
    static To convert(From from)
    {
        if (from < static_cast<From>(std::numeric_limits<To>::min()))
            return Policy<To>::too_small("Convert");
        if (from > static_cast<From>(std::numeric_limits<To>::max()))
            return Policy<To>::too_large("Convert");
        return static_cast<To>(from);
    }
};

template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<std::is_unsigned<From>::value &&
                         std::is_signed<To>::value &&
                         sizeof(From) < sizeof(To)>>
{
    static To convert(From from)
    {
        return static_cast<To>(from);
    }

    static To widen(From from)
    {
        return static_cast<To>(from);
    }
};

template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<std::is_unsigned<From>::value &&
                         std::is_signed<To>::value &&
                         sizeof(To) <= sizeof(From)>>
{
    static To convert(From from)
    {
        if (from > static_cast<From>(std::numeric_limits<To>::max()))
            return Policy<To>::too_large("Convert");
        return static_cast<To>(from);
    }
};

template <class To, class From>
To convert_exn(From from)
{
    return Convert<To, From, Throwing_policy>::convert(from);
};

template <class To, class From>
To convert_sat(From from)
{
    return Convert<To, From, Saturating_policy>::convert(from);
};

template <class To, class From>
To convert_widen(From from)
{
    return Convert<To, From, Throwing_policy>::widen(from);
};

// Checked is currently specialized based on signedness.
template <class T,
          template<class> class P = Throwing_policy,
          class Enable = void>
class Checked;

template <class T, template<class> class P>
class Checked<T, P, std::enable_if_t<std::is_signed<T>::value>>
{
private:
    using unsigned_t = std::make_unsigned_t<T>;
    using policy_t   = P<T>;

    T value_;

    static T min_()
    {
        return std::numeric_limits<T>::min();
    }

    static T max_()
    {
        return std::numeric_limits<T>::max();
    }

    template <class U, template <class> class Q, class F>
    friend class Checked;

public:
    Checked(T value = T()) : value_(value)
    { }

    template <class U>
    Checked(U value) : value_(Convert<T, U, P>::convert(value))
    { }

    template <class U, template<class> class Q>
    Checked(Checked<U, Q> other) : value_(Convert<T, U, P>::widen(other.value_))
    { }

    T get() const
    {
        return value_;
    }

    template <class U, template <class> class Q = P>
    Checked<U, Q> convert() const
    {
        return Checked<U, Q>(Convert<U, T, Q>::convert(value_));
    }

    Checked operator-() const
    {
        if (std::is_signed<T>::value && value_ == min_())
            return policy_t::too_large("Checked::operator-()");
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
#if __has_builtin(__builtin_add_overflow)
        Checked result;
        if (__builtin_add_overflow(value_, other.value_, &result.value_)) {
            if (value_ >= 0)
                return policy_t::too_large("Checked::operator+(Checked)");
            else
                return policy_t::too_small("Checked::operator+(Checked)");
        } else {
            return result;
        }
#else
        if (value_ >= 0) {
            if (other.value_ > max_() - value_)
                return policy_t::too_large("Checked::operator+(Checked)");
        } else {
            if (other.value_ < min_() - value_)
                return policy_t::too_small("Checked::operator+(Checked)");
        }

        return value_ + other.value_;
#endif
    }

    Checked operator-(Checked other) const
    {
#if __has_builtin(__builtin_sub_overflow)
        Checked result;
        if (__builtin_sub_overflow(value_, other.value_, &result.value_)) {
            if (value_ >= 0)
                return policy_t::too_large("Checked::operator-(Checked)");
            else
                return policy_t::too_small("Checked::operator-(Checked)");
        } else {
            return result;
        }
#else
        if (value_ >= 0) {
            if (other.value_ < value_ - max_())
                return policy_t::too_large("Checked::operator-(Checked)");
        } else {
            if (other.value_ > value_ - min_())
                return policy_t::too_small("Checked::operator-(Checked)");
        }

        return value_ - other.value_;
#endif
    }

    Checked operator*(Checked other) const
    {
        auto overflow = [=]() {
            if ((value_ > 0 && other.value_ > 0) ||
                (value_ < 0 && other.value_ < 0))
                return policy_t::too_large("Checked::operator*(Checked)");
            else
                return policy_t::too_small("Checked::operator*(Checked)");
        };

#if __has_builtin(__builtin_mul_overflow)
        Checked result;
        if (__builtin_mul_overflow(value_, other.value_, &result.value_)) {
            return overflow();
        } else {
            return result;
        }
#else
        // This is slow right now, because it does a division. There are better
        // ways of doing it, depending on the size of T.
        if (other.value_ != 0) {
            if (abs() > unsigned_t(max_()) / other.abs()) {
                return overflow();
            }
        }

        return value_ * other.value_;
#endif
    }

    Checked operator/(Checked other) const
    {
        if (value_ == min_() && other.value_ == -1)
            return policy_t::too_large("Checked::operator/(Checked)");

        if (other.value_ == 0) {
            return policy_t::div_zero("Checked::operator/(Checked)");
        }

        return value_ / other.value_;
    }

    // Should this do some kind of checking?
    Checked operator%(Checked other) const
    {
        if (other.value_ == 0) {
            return policy_t::div_zero("Checked::operator%(Checked)");
        }

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
                return policy_t::too_large("Checked::operator<<(u_int8_t)");

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

    Checked& operator+=(Checked other)
    {
        return *this = *this + other;
    }

    Checked& operator-=(Checked other)
    {
        return *this = *this - other;
    }

    Checked& operator*=(Checked other)
    {
        return *this = *this * other;
    }

    Checked& operator/=(Checked other)
    {
        return *this = *this / other;
    }

    Checked& operator%=(Checked other)
    {
        return *this = *this % other;
    }

    Checked& operator&=(Checked other)
    {
        return *this = *this & other;
    }

    Checked& operator|=(Checked other)
    {
        return *this = *this | other;
    }

    Checked& operator^=(Checked other)
    {
        return *this = *this | other;
    }

    Checked& operator<<=(u_int8_t other)
    {
        return *this = *this << other;
    }

    Checked& operator>>=(u_int8_t other)
    {
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

template <class T, template <class> class P>
class Checked<T, P, std::enable_if_t<std::is_unsigned<T>::value>>
{
private:
    using policy_t   = P<T>;

    T value_;

    static T zero_()
    {
        return T(0);
    }

    static T max_()
    {
        return std::numeric_limits<T>::max();
    }

    template <class U, template <class> class Q, class F>
    friend class Checked;

public:
    Checked(T value = T()) : value_(value)
    { }

    template <class U>
    Checked(U value) : value_(Convert<T, U, P>::convert(value))
    { }

    template <class U, template <class> class Q>
    Checked(Checked<U, Q> other) : value_(Convert<T, U, P>::widen(other.value_))
    { }

    T get() const
    {
        return value_;
    }

    template <class U, template <class> class Q = P>
    Checked<U, Q> convert() const
    {
        return Checked<U, Q>(Convert<U, T, Q>::convert(value_));
    }

    Checked operator-() const
    {
        if (value_ == zero_())
            return value_;
        else
            return policy_t::too_small("Checked::operator-()");
    }

    T abs() const
    {
        return value_;
    }

    Checked operator+(Checked other) const
    {
#if __has_builtin(__builtin_add_overflow)
        Checked result;
        if (__builtin_add_overflow(value_, other.value_, &result.value_)) {
            return policy_t::too_large("Checked::operator+(Checked)");
        } else {
            return result;
        }
#else
        if (value_ > max_() - other.value_)
            return policy_t::too_large("Checked::operator+(Checked)");

        return value_ + other.value_;
#endif
    }

    Checked operator-(Checked other) const
    {
        if (other.value_ > value_)
            return policy_t::too_small("Checked::operator-(Checked)");

        return value_ - other.value_;
    }

    Checked operator*(Checked other) const
    {
#if __has_builtin(__builtin_mul_overflow)
        Checked result;
        if (__builtin_mul_overflow(value_, other.value_, &result.value_)) {
            return policy_t::too_large("Checked::operator*(Checked)");
        } else {
            return result;
        }
#else
        // This is slow right now, because it does a division. There are better
        // ways of doing it, depending on the size of T.
        if (other.value_ != 0) {
            if (value_ > max_() / other.value_)
                return policy_t::too_large("Checked::operator*(Checked)");
        }

        return value_ * other.value_;
#endif
    }

    Checked operator/(Checked other) const
    {
        if (other.value_ == 0)
            return policy_t::div_zero("Checked::operator/(Checked)");

        return value_ / other.value_;
    }

    Checked operator%(Checked other) const
    {
        if (other.value_ == 0)
            return policy_t::div_zero("Checked::operator%(Checked)");

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
                return policy_t::too_large("Checked::operator<<(u_int8_t)");

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

    Checked& operator+=(Checked other)
    {
        return *this = *this + other;
    }

    Checked& operator-=(Checked other)
    {
        return *this = *this - other;
    }

    Checked& operator*=(Checked other)
    {
        return *this = *this * other;
    }

    Checked& operator/=(Checked other)
    {
        return *this = *this / other;
    }

    Checked& operator%=(Checked other)
    {
        return *this = *this % other;
    }

    Checked& operator&=(Checked other)
    {
        return *this = *this & other;
    }

    Checked& operator|=(Checked other)
    {
        return *this = *this | other;
    }

    Checked& operator^=(Checked other)
    {
        return *this = *this | other;
    }

    Checked& operator<<=(u_int8_t other)
    {
        return *this = *this << other;
    }

    Checked& operator>>=(u_int8_t other)
    {
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

template <class T, template <class> class P>
std::ostream& operator<<(std::ostream& o, Checked<T, P> a)
{
    return o << a.get();
}

template <class T, template <class> class P>
std::istream& operator>>(std::istream& i, Checked<T, P>& a)
{
    T temp;
    i >> temp;
    a = Checked<T, P>(temp);
    return i;
}

template <class T>
using Saturating = Checked<T, Saturating_policy>;

template <class T>
class Wrapping
{
private:
    using unsigned_t = std::make_unsigned_t<T>;
    unsigned_t value_;

    template <class U>
    friend class Wrapping;

public:
    Wrapping(T value = T()) : value_{unsigned_t(value)}
    { }

    template <class U>
    Wrapping(U value) : value_{unsigned_t(value)}
    { }

    template <class U>
    Wrapping(Wrapping<U> other) : Wrapping(convert_widen<T>(other.value_))
    { }

    T get() const
    {
        return T(value_);
    }

    template <class U>
    Wrapping<U> convert() const
    {
        return Wrapping<U>(static_cast<std::make_unsigned_t<U>>(value_));
    }

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
        return Wrapping(value_ + other.value_);
    }

    Wrapping operator-(Wrapping other) const
    {
        return Wrapping(value_ - other.value_);
    }

    Wrapping operator*(Wrapping other) const
    {
        return Wrapping(value_ * other.value_);
    }

    Wrapping operator/(Wrapping other) const
    {
        if (other.value_ == 0)
            throw overflow_div_zero("Wrapping::operator/(Wrapping)");

        return Wrapping(value_ / other.value_);
    }

    Wrapping operator%(Wrapping other) const
    {
        if (other.value_ == 0)
            throw overflow_div_zero("Wrapping::operator%(Wrapping)");

        return Wrapping(value_ % other.value_);
    }

    Wrapping operator&(Wrapping other) const
    {
        return Wrapping(value_ & other.value_);
    }

    Wrapping operator|(Wrapping other) const
    {
        return Wrapping(value_ | other.value_);
    }

    Wrapping operator^(Wrapping other) const
    {
        return Wrapping(value_ ^ other.value_);
    }

    Wrapping operator<<(u_int8_t other) const
    {
        return Wrapping(value_ << other);
    }

    Wrapping operator>>(u_int8_t other) const
    {
        return Wrapping(get() >> other);
    }

    Wrapping operator~() const
    {
        return Wrapping(~value_);
    }

    Wrapping& operator+=(Wrapping other)
    {
        return *this = *this + other;
    }

    Wrapping& operator-=(Wrapping other)
    {
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
std::istream& operator>>(std::istream& i, Wrapping<T>& a)
{
    T temp;
    i >> temp;
    a = Wrapping<T>(temp);
    return i;
}

}
