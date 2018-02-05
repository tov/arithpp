#include "arith++/arith++.h"
#include <catch/catch.hpp>
#include <gmpxx.h>
#include <stdexcept>

using namespace arithpp;

TEST_CASE("Gmp_works")
{
    mpz_class a(5);
    mpz_class b(4);
    mpz_class c(a + b);
    CHECK(9 == c);
}

bool operator<(const mpz_class& a, const mpz_class& b)
{
    return mpz_cmp(a.get_mpz_t(), b.get_mpz_t()) < 0;
}

template <class T>
bool check_add(T a, T b)
{
    using CT = Checked<T>;

    mpz_class ma(a);
    mpz_class mb(b);
    mpz_class mc(ma + mb);

    mpz_class max(std::numeric_limits<T>::max());
    mpz_class min(std::numeric_limits<T>::min());

    if (max < mc) {
        try {
            CT(a) + CT(b);
            return false;
        } catch (overflow_too_large& e) {
            return true;
        }
    }

    if (mc < min) {
        try {
            CT(a)  + CT(b);
            return false;
        } catch (overflow_too_small& e) {
            return true;
        }
    }

    CT c(a + b);
    if (std::is_signed<T>()) {
        return c.get() == mc.get_si();
    } else {
        return c.get() == mc.get_ui();
    }
}

TEST_CASE("Check_add")
{
    CHECK(check_add(5, 9));
    CHECK(check_add(5, INT_MAX));
    CHECK(check_add(INT_MIN, INT_MAX));
}
