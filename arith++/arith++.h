#pragma once

#include <limits>
#include <stdexcept>

namespace arithpp {

template<class T>
struct Make_unsigned;

#define DECLARE_MAKE_UNSIGNED(S, U) \
    template <> struct Make_unsigned<S> { using type = U; }

DECLARE_MAKE_UNSIGNED(char, unsigned char);
DECLARE_MAKE_UNSIGNED(signed char, unsigned char);
DECLARE_MAKE_UNSIGNED(short, unsigned short);
DECLARE_MAKE_UNSIGNED(int, unsigned int);
DECLARE_MAKE_UNSIGNED(long, unsigned long);
DECLARE_MAKE_UNSIGNED(long long, unsigned long long);

template<class T>
using make_unsigned_t = typename Make_unsigned<T>::type;

template<class T>
inline T safe_negate(T a)
{
    if (a == std::numeric_limits<T>::min())
        throw std::overflow_error("safe_negate overflowed positive");

    return -a;
}

template<class T>
inline make_unsigned_t<T> safe_abs(T a)
{
    if (a == std::numeric_limits<T>::min()) {
        return make_unsigned_t<T>(std::numeric_limits<T>::max()) + 1;
    } else if (a < 0) {
        return -a;
    } else {
        return a;
    }
}

template<class T>
inline T safe_plus(T a, T b)
{
    // Bad cases are:
    //  *  a + b > T_MAX  (can only happen if both are positive)
    //  -  a + b < T_MIN  (can only happen if both are negative)
    if (b >= 0) {
        // -  a + b > T_MAX  ===  a > T_MAX - b
        // The latter cannot overflow of b is non-negative.
        if (a > std::numeric_limits<T>::max() - b) {
            throw std::overflow_error("safe_plus overflowed positive");
        }
        // -  a + b < T_MIN  ===  a < T_MIN - b
        // The latter cannot overflow if b is negative.
    } else {
        if (a < std::numeric_limits<T>::min() - b) {
            throw std::overflow_error("safe_plus overflowed negative");
        }
    }

    return a + b;
}

template<class T>
inline T safe_minus(T a, T b)
{
    // Bad cases are:
    //  -  a - b > T_MAX  (can only happen if a > 0 and b < 0)
    //  -  a - b < T_MIN  (can only happen if a < 0 and b > 0)
    if (a >= 0) {
        //  -  a - b > T_MAX  ===  b < a - T_MAX
        // The latter cannot overflow if a is non-negative.
        if (b < a - std::numeric_limits<T>::max()) {
            throw std::overflow_error("safe_plus overflowed positive");
        }
    } else {
        //  -  a - b < T_MIN  ===  b > a - T_MIN
        if (b > a - std::numeric_limits<T>::min()) {
            throw std::overflow_error("safe_plus overflowed negative");
        }
    }

    return a - b;
}

template<class T>
inline T safe_times(T a, T b)
{
    // Bad cases are:
    //  -  a * b > T_MAX  (can only happen if signs are same)
    //  -  a * b < T_MIN  (can only happen if signs are different)
    if (b > 0) {
        //  - a * b > T_MAX  ===   a > T_MAX / b
        // Can do this safely if b is positive.
        if (a > std::numeric_limits<T>::max() / b) {
            throw std::overflow_error("safe_times overflowed positive");
        }

        //  - a * b < T_MIN  ===   a < T_MIN / b
        // Can do this safely if b is positive.
        if (a < std::numeric_limits<T>::min() / b) {
            throw std::overflow_error("safe_times overflowed negative");
        }
    } else if (a > 0) {
        // By symmetry:
        if (b > std::numeric_limits<T>::max() / a) {
            throw std::overflow_error("safe_times overflowed positive");
        }

        if (b < std::numeric_limits<T>::min() / a) {
            throw std::overflow_error("safe_times overflowed negative");
        }
    } else if (a < 0 && b < 0) {
        if (safe_abs(a) > std::numeric_limits<T>::max() / safe_abs(b)) {
            throw std::overflow_error("safe_times overflowed positive");
        }
    }

    return a * b;
}

template<class T>
T safe_divides(T a, T b)
{
    if (a == std::numeric_limits<T>::min() && b == -1) {
        throw std::overflow_error("safe_divides overflowed positive");
    }

    return a / b;
}

}
