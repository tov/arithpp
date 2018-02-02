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

/*
 * EXCEPTIONS
 */

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

/*
 * POLICIES
 */

// Saturates on overflow, throws on divide-by-zero.
template<class T>
struct Saturating_policy
{
    static constexpr bool is_wrapping = false;

    static constexpr T too_large(const char*)
    {
        return std::numeric_limits<T>::max();
    }

    static T constexpr too_small(const char*)
    {
        return std::numeric_limits<T>::min();
    }

    static T constexpr div_zero(const char* who)
    {
        throw overflow_div_zero(who);
    }
};

// Throws on overflow or divide-by-zero.
template<class T>
struct Throwing_policy
{
    static constexpr bool is_wrapping = false;

    static T constexpr too_large(const char* who)
    {
        throw overflow_too_large(who);
    }

    static T constexpr too_small(const char* who)
    {
        throw overflow_too_small(who);
    }

    static T constexpr div_zero(const char* who)
    {
        throw overflow_div_zero(who);
    }
};

// Wraps instead of overflowing, throws on divide-by-zero
template<class T>
struct Wrapping_policy
{
    static constexpr bool is_wrapping = true;

    static T constexpr div_zero(const char* who)
    {
        throw overflow_div_zero(who);
    }
};

/*
 * INTERNAL DEFINITIONS
 * Includes type size calculations and comparisons.
 */

namespace internal {

// Is type `A` wide enough to hold ever value of type `B`?
template<class A, class B>
constexpr bool is_as_wide_as()
{
    if (std::is_signed<B>::value == std::is_signed<A>::value)
        return sizeof(B) <= sizeof(A);

    if (std::is_unsigned<B>::value && std::is_signed<A>::value)
        return sizeof(B) < sizeof(A);

    return false;
};

// Does type `A` include values lower than `B_MIN`?
template<class A, class B>
constexpr bool goes_lower_than()
{
    if (std::is_unsigned<A>::value) return false;

    if (std::is_unsigned<B>::value) return true;

    return sizeof(A) > sizeof(B);
};

// Does type `A` include values higher than `B_MAX`?
template<class A, class B>
constexpr bool goes_higher_than()
{
    if (sizeof(A) == sizeof(B))
        return std::is_unsigned<A>::value && std::is_signed<B>::value;

    return sizeof(A) > sizeof(B);
};

// Gets the minimum value of type `T` in type `Repr`.
// PRECONDITION: !goes_lower_than<T, Repr>()
template<class T, class Repr>
constexpr Repr
min_as()
{
    return static_cast<Repr>(std::numeric_limits<T>::min());
};

// Gets the maximum value of type `T` in type `Repr`.
// PRECONDITION: !goes_higher_than<T, Repr>()
template<class T, class Repr>
constexpr Repr
max_as()
{
    return static_cast<Repr>(std::numeric_limits<T>::max());
};

// Is `from` too small to fit in type `To`?
template<class To, class From>
constexpr bool is_too_small_for(From from)
{
    if (goes_lower_than<From, To>())
        return from < min_as<To, From>();
    else
        return false;
}

// Is `from` too large to fit in type `To`?
template<class To, class From>
constexpr bool is_too_large_for(From from)
{
    if (goes_higher_than<From, To>())
        return from > max_as<To, From>();
    else
        return false;
}

template<class T>
constexpr bool same_sign(T a, T b)
{
    return (a ^ b) >= 0;
}

} // end internal

/*
 * CONVERSIONS
 */

// We specialize class Convert for the `To` and `From` types and the `Policy`.
template <class To,
          class from,
          template <class> class Policy = Throwing_policy,
          class Enable = void>
struct Convert;

// Widening conversions are non-lossy.
template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<internal::is_as_wide_as<To, From>()>>
{
    static constexpr To convert(From from)
    {
        return static_cast<To>(from);
    }

    // Only for widening conversions do we provide the widen function.
    static constexpr To widen(From from)
    {
        return static_cast<To>(from);
    }
};

// Non-wrapping conversion where the value might be too low.
template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<internal::goes_lower_than<From, To>()
                         && !internal::goes_higher_than<From, To>()
                         && !Policy<To>::is_wrapping>>
{
    static constexpr To convert(From from)
    {
        if (internal::is_too_small_for<To>(from))
            return Policy<To>::too_small("Convert");
        return static_cast<To>(from);
    }
};

// Non-wrapping conversion where the value might be too low or too high.
template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<internal::goes_lower_than<From, To>()
                         && internal::goes_higher_than<From, To>()
                         && !Policy<To>::is_wrapping>>
{
    static constexpr To convert(From from)
    {
        if (internal::is_too_small_for<To>(from))
            return Policy<To>::too_small("Convert");
        if (internal::is_too_large_for<To>(from))
            return Policy<To>::too_small("Convert");
        return static_cast<To>(from);
    }
};

// Non-wrapping conversion where the value might be too high.
template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<!internal::goes_lower_than<From, To>()
                         && internal::goes_higher_than<From, To>()
                         && !Policy<To>::is_wrapping>>
{
    static constexpr To convert(From from)
    {
        if (internal::is_too_large_for<To>(from))
            return Policy<To>::too_small("Convert");
        return static_cast<To>(from);
    }
};

// Wrapping, non-widening conversion.
template <class To, class From, template <class> class Policy>
struct Convert<To, From, Policy,
        std::enable_if_t<Policy<To>::is_wrapping &&
                         !internal::is_as_wide_as<To, From>()>>
{
    using UFrom = std::make_unsigned_t<From>;
    using UTo   = std::make_unsigned_t<From>;

    static constexpr To convert(From from)
    {
        return static_cast<To>(static_cast<UTo>(static_cast<UFrom>(from)));
    }
};

// Convenience function for converting using Throwing_policy.
template <class To, class From>
constexpr To convert_exn(From from)
{
    return Convert<To, From, Throwing_policy>::convert(from);
};

// Convenience function for converting using Saturating_policy.
template <class To, class From>
constexpr To convert_sat(From from)
{
    return Convert<To, From, Saturating_policy>::convert(from);
};

// Convenience function for widening conversions.
template <class To, class From>
constexpr To convert_widen(From from)
{
    return Convert<To, From, Throwing_policy>::widen(from);
};

/*
 * CHECKED INTEGERS
 */

// Checked<T, P> specifies an integer type T and a policy P. It is specialized based
// on signedness and wrapping.
template <class T,
          template<class> class P = Throwing_policy,
          class Enable = void>
class Checked;

// Non-wrapping, signed integers.
template <class T, template<class> class P>
class Checked<T, P,
        std::enable_if_t<std::is_signed<T>::value && !P<T>::is_wrapping>>
{
private:
    using unsigned_t = std::make_unsigned_t<T>;
    using policy_t   = P<T>;

    T value_;

    static constexpr T T_MIN_ = std::numeric_limits<T>::min();
    static constexpr T T_MAX_ = std::numeric_limits<T>::max();

    template <typename U>
    static constexpr Checked rebuild_(U value)
    {
        return static_cast<T>(value);
    }

    template <class U, template <class> class Q, class F>
    friend class Checked;

public:
    constexpr Checked(T value = T()) : value_(value)
    { }

    template <class U>
    constexpr Checked(U value) : value_(Convert<T, U, P>::convert(value))
    { }

    template <class U, template<class> class Q>
    constexpr Checked(Checked<U, Q> other) : value_(Convert<T, U, P>::widen(other.value_))
    { }

    constexpr T get() const
    {
        return value_;
    }

    template <class U, template <class> class Q = P>
    constexpr Checked<U, Q> convert() const
    {
        return Checked<U, Q>(Convert<U, T, P>::convert(get()));
    }

    constexpr Checked operator-() const
    {
        if (std::is_signed<T>::value && value_ == T_MIN_)
            return policy_t::too_large("Checked::operator-()");
        else
            return rebuild_(-value_);
    }

    constexpr unsigned_t abs() const
    {
        if (value_ == T_MIN_) {
            return unsigned_t(std::numeric_limits<T>::max()) + 1;
        } else if (value_ < 0) {
            return unsigned_t(-value_);
        } else {
            return unsigned_t(value_);
        }
    }

    constexpr Checked operator+(Checked other) const
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
            if (other.value_ > T_MAX_ - value_)
                return policy_t::too_large("Checked::operator+(Checked)");
        } else {
            if (other.value_ < T_MIN_ - value_)
                return policy_t::too_small("Checked::operator+(Checked)");
        }

        return rebuild_(value_ + other.value_);
#endif
    }

    constexpr Checked operator-(Checked other) const
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
            if (other.value_ < value_ - T_MAX_)
                return policy_t::too_large("Checked::operator-(Checked)");
        } else {
            if (other.value_ > value_ - T_MIN_)
                return policy_t::too_small("Checked::operator-(Checked)");
        }

        return rebuild_(value_ - other.value_);
#endif
    }

    constexpr Checked operator*(Checked other) const
    {
        auto overflow = [=]() {
            if (internal::same_sign(value_, other.value_))
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
            if (abs() > unsigned_t(T_MAX_) / other.abs()) {
                return overflow();
            }
        }

        return rebuild_(value_ * other.value_);
#endif
    }

    constexpr Checked operator/(Checked other) const
    {
        if (value_ == T_MIN_ && other.value_ == -1)
            return policy_t::too_large("Checked::operator/(Checked)");

        if (other.value_ == 0) {
            return policy_t::div_zero("Checked::operator/(Checked)");
        }

        return rebuild_(value_ / other.value_);
    }

    // Should this do some kind of checking?
    constexpr Checked operator%(Checked other) const
    {
        if (other.value_ == 0) {
            return policy_t::div_zero("Checked::operator%(Checked)");
        }

        return rebuild_(value_ % other.value_);
    }

    constexpr Checked operator&(Checked other) const
    {
        return rebuild_(value_ & other.value_);
    }

    constexpr Checked operator|(Checked other) const
    {
        return rebuild_(value_ | other.value_);
    }

    constexpr Checked operator^(Checked other) const
    {
        return rebuild_(value_ ^ other.value_);
    }

    constexpr Checked operator<<(u_int8_t other) const
    {
        if (T_MAX_ >> other < value_)
            return policy_t::too_large("Checked::operator<<(u_int8_t)");

        return rebuild_(value_ << other);
    }

    constexpr Checked operator>>(u_int8_t other) const
    {
        return rebuild_(value_ >> other);
    }

    constexpr Checked operator~() const
    {
        return rebuild_(~value_);
    }

    constexpr Checked& operator+=(Checked other)
    {
        return *this = *this + other;
    }

    constexpr Checked& operator-=(Checked other)
    {
        return *this = *this - other;
    }

    constexpr Checked& operator*=(Checked other)
    {
        return *this = *this * other;
    }

    constexpr Checked& operator/=(Checked other)
    {
        return *this = *this / other;
    }

    constexpr Checked& operator%=(Checked other)
    {
        return *this = *this % other;
    }

    constexpr Checked& operator&=(Checked other)
    {
        return *this = *this & other;
    }

    constexpr Checked& operator|=(Checked other)
    {
        return *this = *this | other;
    }

    constexpr Checked& operator^=(Checked other)
    {
        return *this = *this | other;
    }

    constexpr Checked& operator<<=(u_int8_t other)
    {
        return *this = *this << other;
    }

    constexpr Checked& operator>>=(u_int8_t other)
    {
        return *this = *this >> other;
    }

    constexpr Checked& operator++()
    {
        return *this += 1;
    }

    constexpr Checked& operator--()
    {
        return *this -= 1;
    }

    constexpr Checked& operator++(int)
    {
        Checked old = *this;
        ++*this;
        return old;
    }

    constexpr Checked& operator--(int)
    {
        Checked old = *this;
        --*this;
        return old;
    }
};

// Non-wrapping, unsigned integers.
template <class T, template <class> class P>
class Checked<T, P,
        std::enable_if_t<std::is_unsigned<T>::value && !P<T>::is_wrapping>>
{
private:
    using policy_t   = P<T>;

    T value_;

    static constexpr T T_MAX_ = std::numeric_limits<T>::max();

    template <typename U>
    static constexpr Checked rebuild_(U value)
    {
        return static_cast<T>(value);
    }

    template <class U, template <class> class Q, class F>
    friend class Checked;

public:
    constexpr Checked(T value = T()) : value_(value)
    { }

    template <class U>
    constexpr Checked(U value) : value_(Convert<T, U, P>::convert(value))
    { }

    template <class U, template <class> class Q>
    constexpr Checked(Checked<U, Q> other) : value_(Convert<T, U, P>::widen(other.value_))
    { }

    constexpr T get() const
    {
        return value_;
    }

    template <class U, template <class> class Q = P>
    constexpr Checked<U, Q> convert() const
    {
        return Checked<U, Q>(Convert<U, T, P>::convert(get()));
    }

    constexpr Checked operator-() const
    {
        if (value_ == T(0))
            return rebuild_(value_);
        else
            return policy_t::too_small("Checked::operator-()");
    }

    constexpr T abs() const
    {
        return value_;
    }

    constexpr Checked operator+(Checked other) const
    {
#if __has_builtin(__builtin_add_overflow)
        Checked result;
        if (__builtin_add_overflow(value_, other.value_, &result.value_)) {
            return policy_t::too_large("Checked::operator+(Checked)");
        } else {
            return result;
        }
#else
        if (value_ > T_MAX_ - other.value_)
            return policy_t::too_large("Checked::operator+(Checked)");

        return rebuild_(value_ + other.value_);
#endif
    }

    constexpr Checked operator-(Checked other) const
    {
        if (other.value_ > value_)
            return policy_t::too_small("Checked::operator-(Checked)");

        return rebuild_(value_ - other.value_);
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
            if (value_ > T_MAX_ / other.value_)
                return policy_t::too_large("Checked::operator*(Checked)");
        }

        return rebuild_(value_ * other.value_);
#endif
    }

    constexpr Checked operator/(Checked other) const
    {
        if (other.value_ == 0)
            return policy_t::div_zero("Checked::operator/(Checked)");

        return rebuild_(value_ / other.value_);
    }

    constexpr Checked operator%(Checked other) const
    {
        if (other.value_ == 0)
            return policy_t::div_zero("Checked::operator%(Checked)");

        return rebuild_(value_ % other.value_);
    }

    constexpr Checked operator&(Checked other) const
    {
        return rebuild_(value_ & other.value_);
    }

    constexpr Checked operator|(Checked other) const
    {
        return rebuild_(value_ | other.value_);
    }

    constexpr Checked operator^(Checked other) const
    {
        return rebuild_(value_ ^ other.value_);
    }

    constexpr Checked operator<<(u_int8_t other) const
    {
        if (T_MAX_ >> other < value_)
            return policy_t::too_large("Checked::operator<<(u_int8_t)");

        return rebuild_(value_ << other);
    }

    constexpr Checked operator>>(u_int8_t other) const
    {
        return rebuild_(value_ >> other);
    }

    constexpr Checked operator~() const
    {
        return rebuild_(~value_);
    }

    constexpr Checked& operator+=(Checked other)
    {
        return *this = *this + other;
    }

    constexpr Checked& operator-=(Checked other)
    {
        return *this = *this - other;
    }

    constexpr Checked& operator*=(Checked other)
    {
        return *this = *this * other;
    }

    constexpr Checked& operator/=(Checked other)
    {
        return *this = *this / other;
    }

    constexpr Checked& operator%=(Checked other)
    {
        return *this = *this % other;
    }

    constexpr Checked& operator&=(Checked other)
    {
        return *this = *this & other;
    }

    constexpr Checked& operator|=(Checked other)
    {
        return *this = *this | other;
    }

    constexpr Checked& operator^=(Checked other)
    {
        return *this = *this | other;
    }

    constexpr Checked& operator<<=(u_int8_t other)
    {
        return *this = *this << other;
    }

    constexpr Checked& operator>>=(u_int8_t other)
    {
        return *this = *this >> other;
    }

    constexpr Checked& operator++()
    {
        return *this += 1;
    }

    constexpr Checked& operator--()
    {
        return *this -= 1;
    }

    constexpr Checked& operator++(int)
    {
        Checked old = *this;
        ++*this;
        return old;
    }

    constexpr Checked& operator--(int)
    {
        Checked old = *this;
        --*this;
        return old;
    }
};

// Wrapping integers (potentially signed)
template <class T, template <class> class P>
class Checked<T, P, std::enable_if_t<P<T>::is_wrapping>>
{
private:
    using policy_t = P<T>;
    using unsigned_t = std::make_unsigned_t<T>;
    unsigned_t value_;

    template <class U, template <class> class Q, class F>
    friend class Checked;

public:
    constexpr Checked(T value = T()) : value_{unsigned_t(value)}
    { }

    template <class U>
    constexpr Checked(U value) : value_{unsigned_t(value)}
    { }

    template <class U, template <class> class Q>
    constexpr Checked(Checked<U, Q> other)
            : Checked(convert_widen<T>(other.value_))
    { }

    constexpr T get() const
    {
        return static_cast<T>(value_);
    }

    template <class U, template <class> class Q = P>
    constexpr Checked<U, Q> convert() const
    {
        return Checked<U, Q>(Convert<U, T, P>::convert(get()));
    }

    constexpr Checked operator-() const {
        if (get() == std::numeric_limits<T>::min())
            return Checked(std::numeric_limits<T>::min());
        else
            return Checked(-get());
    }

    constexpr unsigned_t abs() const
    {
        if (get() == std::numeric_limits<T>::min()) {
            return unsigned_t(std::numeric_limits<T>::max()) + 1;
        } else if (get() < 0) {
            return unsigned_t(-get());
        } else {
            return unsigned_t(get());
        }
    }

    constexpr Checked operator+(Checked other) const
    {
        return Checked(value_ + other.value_);
    }

    constexpr Checked operator-(Checked other) const
    {
        return Checked(value_ - other.value_);
    }

    constexpr Checked operator*(Checked other) const
    {
        return Checked(value_ * other.value_);
    }

    constexpr Checked operator/(Checked other) const
    {
        if (other.value_ == 0)
            return policy_t::div_zero("Checked::operator/(Checked)");

        return Checked(value_ / other.value_);
    }

    constexpr Checked operator%(Checked other) const
    {
        if (other.value_ == 0)
            return policy_t::div_zero("Checked::operator/(Checked)");

        return Checked(value_ % other.value_);
    }

    constexpr Checked operator&(Checked other) const
    {
        return Checked(value_ & other.value_);
    }

    constexpr Checked operator|(Checked other) const
    {
        return Checked(value_ | other.value_);
    }

    constexpr Checked operator^(Checked other) const
    {
        return Checked(value_ ^ other.value_);
    }

    constexpr Checked operator<<(u_int8_t other) const
    {
        return Checked(value_ << other);
    }

    constexpr Checked operator>>(u_int8_t other) const
    {
        return Checked(get() >> other);
    }

    constexpr Checked operator~() const
    {
        return Checked(~value_);
    }

    constexpr Checked& operator+=(Checked other)
    {
        return *this = *this + other;
    }

    constexpr Checked& operator-=(Checked other)
    {
        return *this = *this - other;
    }

    constexpr Checked& operator*=(Checked other)
    {
        return *this = *this * other;
    }

    constexpr Checked& operator/=(Checked other)
    {
        return *this = *this / other;
    }

    constexpr Checked& operator%=(Checked other)
    {
        return *this = *this % other;
    }

    constexpr Checked& operator&=(Checked other)
    {
        return *this = *this & other;
    }

    constexpr Checked& operator|=(Checked other)
    {
        return *this = *this | other;
    }

    constexpr Checked& operator^=(Checked other)
    {
        return *this = *this ^ other;
    }

    constexpr Checked& operator<<=(u_int8_t other)
    {
        return *this = *this << other;
    }

    constexpr Checked& operator>>=(u_int8_t other)
    {
        return *this = *this >> other;
    }

    constexpr Checked& operator++()
    {
        return *this += 1;
    }

    constexpr Checked& operator--()
    {
        return *this -= 1;
    }

    constexpr Checked& operator++(int)
    {
        Checked old = *this;
        ++*this;
        return old;
    }

    constexpr Checked& operator--(int)
    {
        Checked old = *this;
        --*this;
        return old;
    }
};

template <class T>
using Saturating = Checked<T, Saturating_policy>;

template <class T>
using Wrapping = Checked<T, Wrapping_policy>;

/*
 * Checked integer comparisons and stream operations:
 */

template <class T, template <class> class P,
        class U, template <class> class Q>
constexpr bool operator==(Checked<T, P> a, Checked<U, Q> b)
{
    if (internal::is_too_large_for<U>(a.get()))
        return false;

    if (internal::is_too_small_for<U>(a.get()))
        return false;

    return static_cast<U>(a.get()) == b.get();
}

template <class T, template <class> class P,
        class U, template <class> class Q>
constexpr bool operator!=(Checked<T, P> a, Checked<U, Q> b)
{
    return !(a == b);
}

template <class T, template <class> class P,
        class U, template <class> class Q>
constexpr bool operator<(Checked<T, P> a, Checked<U, Q> b)
{
    if (internal::is_too_large_for<U>(a.get()))
        return false;

    if (internal::is_too_small_for<U>(a.get()))
        return true;

    return static_cast<U>(a.get()) < b.get();
}

template <class T, template <class> class P,
        class U, template <class> class Q>
constexpr bool operator<=(Checked<T, P> a, Checked<U, Q> b)
{
    return !(b < a);
}

template <class T, template <class> class P,
        class U, template <class> class Q>
constexpr bool operator>(Checked<T, P> a, Checked<U, Q> b)
{
    return b < a;
}

template <class T, template <class> class P,
        class U, template <class> class Q>
constexpr bool operator>=(Checked<T, P> a, Checked<U, Q> b)
{
    return !(a < b);
}

template <class T, template <class> class P, class U>
constexpr bool operator==(Checked<T, P> a, U b)
{
    return a == Checked<U, P>(b);
}
template <class T, template <class> class P, class U>
constexpr bool operator!=(Checked<T, P> a, U b)
{
    return a != Checked<U, P>(b);
}

template <class T, template <class> class P, class U>
constexpr bool operator<(Checked<T, P> a, U b)
{
    return a < Checked<U, P>(b);
}

template <class T, template <class> class P, class U>
constexpr bool operator<=(Checked<T, P> a, U b)
{
    return a <= Checked<U, P>(b);
}

template <class T, template <class> class P, class U>
constexpr bool operator>(Checked<T, P> a, U b)
{
    return a > Checked<U, P>(b);
}

template <class T, template <class> class P, class U>
constexpr bool operator>=(Checked<T, P> a, U b)
{
    return a >= Checked<U, P>(b);
}

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

}
