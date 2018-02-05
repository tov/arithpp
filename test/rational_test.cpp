#include "Rational.h"
#include <catch/catch.hpp>
#include <stdexcept>

using R = rational::Rational;

TEST_CASE("rational multiplication")
{
    CHECK(R(1, 2) == R(2, 3) * R(3, 4));
    CHECK(R(LONG_MAX) == R(LONG_MAX, 3) * R(3));
}

TEST_CASE("rational multiplication overflow")
{
    CHECK_THROWS_AS(R(LONG_MAX, 3) * R(5), std::overflow_error);
}
