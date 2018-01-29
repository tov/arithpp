#include "arith++/arith++.h"
#include "UnitTest++/UnitTest++.h"

using namespace arithpp;

int main()
{
    return UnitTest::RunAllTests();
}

TEST(Safe_negate) {
    CHECK_EQUAL(8, safe_negate(-8));
    CHECK_EQUAL(-7, safe_negate(7));
    CHECK_EQUAL(-INT_MAX, safe_negate(INT_MAX));
    CHECK_EQUAL(INT_MAX, safe_negate(-INT_MAX));
    CHECK_THROW(safe_negate(INT_MIN), std::overflow_error);
}

TEST(Safe_abs) {
    CHECK_EQUAL(3, safe_abs(3));
    CHECK_EQUAL(3, safe_abs(-3));
    CHECK_EQUAL(INT_MAX, safe_abs(INT_MAX));
    CHECK_EQUAL(INT_MAX, safe_abs(-INT_MAX));
    CHECK_EQUAL(unsigned(INT_MAX) + 1, safe_abs(INT_MIN));
}

TEST(Safe_plus) {
    CHECK_EQUAL(8, safe_plus(3, 5));
    CHECK_EQUAL(-1, safe_plus(INT_MIN, INT_MAX));
    CHECK_THROW(safe_plus(INT_MAX, INT_MAX), std::overflow_error);
    CHECK_THROW(safe_plus(INT_MIN, INT_MIN), std::overflow_error);
    CHECK_THROW(safe_plus(INT_MAX, 1), std::overflow_error);
    CHECK_THROW(safe_plus(1, INT_MAX), std::overflow_error);
    CHECK_THROW(safe_plus(INT_MAX - 5, 6), std::overflow_error);
    CHECK_THROW(safe_plus(INT_MIN, -1), std::overflow_error);
}

TEST(Safe_minus) {
    CHECK_EQUAL(3, safe_minus(8, 5));
    CHECK_EQUAL(-3, safe_minus(5, 8));
    CHECK_EQUAL(-13, safe_minus(-5, 8));
    CHECK_EQUAL(13, safe_minus(8, -5));
    CHECK_EQUAL(-13, safe_minus(-8, 5));
    CHECK_EQUAL(13, safe_minus(5, -8));
    CHECK_EQUAL(3, safe_minus(-5, -8));
    CHECK_EQUAL(-3, safe_minus(-8, -5));
    CHECK_EQUAL(INT_MAX - 3, safe_minus(INT_MAX, 3));
    CHECK_EQUAL(INT_MAX, safe_minus(INT_MAX - 2, -2));
    CHECK_EQUAL(INT_MIN, safe_minus(INT_MIN + 3, 3));

    CHECK_THROW(safe_minus(INT_MAX, -1), std::overflow_error);
    CHECK_THROW(safe_minus(INT_MAX - 2, -3), std::overflow_error);
    CHECK_THROW(safe_minus(INT_MIN + 2, 3), std::overflow_error);
}

TEST(Safe_times) {
    CHECK_EQUAL(12, safe_times(3, 4));
    CHECK_EQUAL(-12, safe_times(-3, 4));
    CHECK_EQUAL(-12, safe_times(3, -4));
    CHECK_EQUAL(12, safe_times(-3, -4));

    CHECK_EQUAL(INT_MAX, safe_times(INT_MAX / 2, 2) + INT_MAX % 2);
    CHECK_EQUAL(INT_MAX, safe_times(INT_MAX / 5, 5) + INT_MAX % 5);
    CHECK_THROW(safe_times(INT_MAX / 5, 6), std::overflow_error);
    CHECK_THROW(safe_times(INT_MAX / 5, -6), std::overflow_error);
    CHECK_THROW(safe_times(INT_MAX / -5, 6), std::overflow_error);
    CHECK_THROW(safe_times(INT_MAX / -5, -6), std::overflow_error);
}

TEST(Safe_divides) {
    CHECK_EQUAL(3, safe_divides(12, 4));
    CHECK_EQUAL(-3, safe_divides(12, -4));
    CHECK_EQUAL(-3, safe_divides(-12, 4));
    CHECK_EQUAL(3, safe_divides(-12, -4));

    CHECK_THROW(safe_divides(INT_MIN, -1), std::overflow_error);
}
