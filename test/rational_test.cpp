#include "Rational.h"
#include <UnitTest++/UnitTest++.h>
#include <stdexcept>

using R = rational::Rational;

TEST(RationalMultiplication)
{
    CHECK_EQUAL(R(1, 2), R(2, 3) * R(3, 4));
    CHECK_EQUAL(R(LONG_MAX), R(LONG_MAX, 3) * R(3));
}

TEST(RationalMultiplicationOverflow)
{
    CHECK_THROW(R(LONG_MAX, 3) * R(5), std::overflow_error);
}
