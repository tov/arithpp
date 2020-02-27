#include <xxint.hxx>
#include <catch.hxx>
#include <cstdint>

using namespace xxint::detail;

TEST_CASE("is_as_wide_as")
{
    CHECK(      is_as_wide_as<u_int16_t, u_int16_t>());
    CHECK_FALSE(is_as_wide_as<  int16_t, u_int16_t>());
    CHECK_FALSE(is_as_wide_as<u_int16_t,   int16_t>());
    CHECK(      is_as_wide_as<  int16_t,   int16_t>());

    CHECK(      is_as_wide_as<u_int32_t, u_int16_t>());
    CHECK_FALSE(is_as_wide_as<u_int16_t, u_int32_t>());
    CHECK(      is_as_wide_as<  int32_t, u_int16_t>());
    CHECK_FALSE(is_as_wide_as<  int16_t, u_int32_t>());
    CHECK_FALSE(is_as_wide_as<u_int32_t,   int16_t>());
    CHECK_FALSE(is_as_wide_as<u_int16_t,   int32_t>());
    CHECK(      is_as_wide_as<  int32_t,   int16_t>());
    CHECK_FALSE(is_as_wide_as<  int16_t,   int32_t>());

    CHECK(      is_as_wide_as<u_int64_t, u_int16_t>());
    CHECK_FALSE(is_as_wide_as<u_int16_t, u_int64_t>());
    CHECK(      is_as_wide_as<  int64_t, u_int16_t>());
    CHECK_FALSE(is_as_wide_as<  int16_t, u_int64_t>());
    CHECK_FALSE(is_as_wide_as<u_int64_t,   int16_t>());
    CHECK_FALSE(is_as_wide_as<u_int16_t,   int64_t>());
    CHECK(      is_as_wide_as<  int64_t,   int16_t>());
    CHECK_FALSE(is_as_wide_as<  int16_t,   int64_t>());
}

TEST_CASE("goes_lower_than")
{
    CHECK_FALSE(goes_lower_than<u_int16_t, u_int16_t>());
    CHECK(      goes_lower_than<  int16_t, u_int16_t>());
    CHECK_FALSE(goes_lower_than<u_int16_t,   int16_t>());
    CHECK_FALSE(goes_lower_than<  int16_t,   int16_t>());

    CHECK_FALSE(goes_lower_than<u_int32_t, u_int16_t>());
    CHECK_FALSE(goes_lower_than<u_int16_t, u_int32_t>());
    CHECK(      goes_lower_than<  int32_t, u_int16_t>());
    CHECK(      goes_lower_than<  int16_t, u_int32_t>());
    CHECK_FALSE(goes_lower_than<u_int32_t,   int16_t>());
    CHECK_FALSE(goes_lower_than<u_int16_t,   int32_t>());
    CHECK(      goes_lower_than<  int32_t,   int16_t>());
    CHECK_FALSE(goes_lower_than<  int16_t,   int32_t>());

    CHECK_FALSE(goes_lower_than<u_int64_t, u_int16_t>());
    CHECK_FALSE(goes_lower_than<u_int16_t, u_int64_t>());
    CHECK(      goes_lower_than<  int64_t, u_int16_t>());
    CHECK(      goes_lower_than<  int16_t, u_int64_t>());
    CHECK_FALSE(goes_lower_than<u_int64_t,   int16_t>());
    CHECK_FALSE(goes_lower_than<u_int16_t,   int64_t>());
    CHECK(      goes_lower_than<  int64_t,   int16_t>());
    CHECK_FALSE(goes_lower_than<  int16_t,   int64_t>());
}

TEST_CASE("goes_higher_than")
{
    CHECK_FALSE(goes_higher_than<u_int16_t, u_int16_t>());
    CHECK_FALSE(goes_higher_than<  int16_t, u_int16_t>());
    CHECK(      goes_higher_than<u_int16_t,   int16_t>());
    CHECK_FALSE(goes_higher_than<  int16_t,   int16_t>());

    CHECK(      goes_higher_than<u_int32_t, u_int16_t>());
    CHECK_FALSE(goes_higher_than<u_int16_t, u_int32_t>());
    CHECK(      goes_higher_than<  int32_t, u_int16_t>());
    CHECK_FALSE(goes_higher_than<  int16_t, u_int32_t>());
    CHECK(      goes_higher_than<u_int32_t,   int16_t>());
    CHECK_FALSE(goes_higher_than<u_int16_t,   int32_t>());
    CHECK(      goes_higher_than<  int32_t,   int16_t>());
    CHECK_FALSE(goes_higher_than<  int16_t,   int32_t>());

    CHECK(      goes_higher_than<u_int64_t, u_int16_t>());
    CHECK_FALSE(goes_higher_than<u_int16_t, u_int64_t>());
    CHECK(      goes_higher_than<  int64_t, u_int16_t>());
    CHECK_FALSE(goes_higher_than<  int16_t, u_int64_t>());
    CHECK(      goes_higher_than<u_int64_t,   int16_t>());
    CHECK_FALSE(goes_higher_than<u_int16_t,   int64_t>());
    CHECK(      goes_higher_than<  int64_t,   int16_t>());
    CHECK_FALSE(goes_higher_than<  int16_t,   int64_t>());
}

