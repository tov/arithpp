#include "arith++/arith++.h"
#include <UnitTest++/UnitTest++.h>
#include <cstdint>

using namespace arithpp::internal;

TEST(Is_as_wide_as)
{
    CHECK( (is_as_wide_as<u_int16_t, u_int16_t>()));
    CHECK(!(is_as_wide_as<  int16_t, u_int16_t>()));
    CHECK(!(is_as_wide_as<u_int16_t,   int16_t>()));
    CHECK( (is_as_wide_as<  int16_t,   int16_t>()));

    CHECK( (is_as_wide_as<u_int32_t, u_int16_t>()));
    CHECK(!(is_as_wide_as<u_int16_t, u_int32_t>()));
    CHECK( (is_as_wide_as<  int32_t, u_int16_t>()));
    CHECK(!(is_as_wide_as<  int16_t, u_int32_t>()));
    CHECK(!(is_as_wide_as<u_int32_t,   int16_t>()));
    CHECK(!(is_as_wide_as<u_int16_t,   int32_t>()));
    CHECK( (is_as_wide_as<  int32_t,   int16_t>()));
    CHECK(!(is_as_wide_as<  int16_t,   int32_t>()));

    CHECK( (is_as_wide_as<u_int64_t, u_int16_t>()));
    CHECK(!(is_as_wide_as<u_int16_t, u_int64_t>()));
    CHECK( (is_as_wide_as<  int64_t, u_int16_t>()));
    CHECK(!(is_as_wide_as<  int16_t, u_int64_t>()));
    CHECK(!(is_as_wide_as<u_int64_t,   int16_t>()));
    CHECK(!(is_as_wide_as<u_int16_t,   int64_t>()));
    CHECK( (is_as_wide_as<  int64_t,   int16_t>()));
    CHECK(!(is_as_wide_as<  int16_t,   int64_t>()));
}

TEST(Goes_lower_than)
{
    CHECK(!(goes_lower_than<u_int16_t, u_int16_t>()));
    CHECK( (goes_lower_than<  int16_t, u_int16_t>()));
    CHECK(!(goes_lower_than<u_int16_t,   int16_t>()));
    CHECK(!(goes_lower_than<  int16_t,   int16_t>()));

    CHECK(!(goes_lower_than<u_int32_t, u_int16_t>()));
    CHECK(!(goes_lower_than<u_int16_t, u_int32_t>()));
    CHECK( (goes_lower_than<  int32_t, u_int16_t>()));
    CHECK( (goes_lower_than<  int16_t, u_int32_t>()));
    CHECK(!(goes_lower_than<u_int32_t,   int16_t>()));
    CHECK(!(goes_lower_than<u_int16_t,   int32_t>()));
    CHECK( (goes_lower_than<  int32_t,   int16_t>()));
    CHECK(!(goes_lower_than<  int16_t,   int32_t>()));

    CHECK(!(goes_lower_than<u_int64_t, u_int16_t>()));
    CHECK(!(goes_lower_than<u_int16_t, u_int64_t>()));
    CHECK( (goes_lower_than<  int64_t, u_int16_t>()));
    CHECK( (goes_lower_than<  int16_t, u_int64_t>()));
    CHECK(!(goes_lower_than<u_int64_t,   int16_t>()));
    CHECK(!(goes_lower_than<u_int16_t,   int64_t>()));
    CHECK( (goes_lower_than<  int64_t,   int16_t>()));
    CHECK(!(goes_lower_than<  int16_t,   int64_t>()));
}

TEST(Goes_higher_than)
{
    CHECK(!(goes_higher_than<u_int16_t, u_int16_t>()));
    CHECK(!(goes_higher_than<  int16_t, u_int16_t>()));
    CHECK( (goes_higher_than<u_int16_t,   int16_t>()));
    CHECK(!(goes_higher_than<  int16_t,   int16_t>()));

    CHECK( (goes_higher_than<u_int32_t, u_int16_t>()));
    CHECK(!(goes_higher_than<u_int16_t, u_int32_t>()));
    CHECK( (goes_higher_than<  int32_t, u_int16_t>()));
    CHECK(!(goes_higher_than<  int16_t, u_int32_t>()));
    CHECK( (goes_higher_than<u_int32_t,   int16_t>()));
    CHECK(!(goes_higher_than<u_int16_t,   int32_t>()));
    CHECK( (goes_higher_than<  int32_t,   int16_t>()));
    CHECK(!(goes_higher_than<  int16_t,   int32_t>()));

    CHECK( (goes_higher_than<u_int64_t, u_int16_t>()));
    CHECK(!(goes_higher_than<u_int16_t, u_int64_t>()));
    CHECK( (goes_higher_than<  int64_t, u_int16_t>()));
    CHECK(!(goes_higher_than<  int16_t, u_int64_t>()));
    CHECK( (goes_higher_than<u_int64_t,   int16_t>()));
    CHECK(!(goes_higher_than<u_int16_t,   int64_t>()));
    CHECK( (goes_higher_than<  int64_t,   int16_t>()));
    CHECK(!(goes_higher_than<  int16_t,   int64_t>()));
}

