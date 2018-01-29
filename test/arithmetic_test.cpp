#include "arith++/arith++.h"
#include "UnitTest++/UnitTest++.h"

using W = arithpp::Wrapping<int>;
using C = arithpp::Checked<int>;
using S = arithpp::Saturating<int>;

int main()
{
    return UnitTest::RunAllTests();
}

TEST(Wrapping) {
    CHECK_EQUAL(5, W(5).get());
    CHECK_EQUAL(W(INT_MIN + 4), W(INT_MAX) + W(5));
}

TEST(Checked_construction) {
    CHECK_EQUAL(5, C(5).get());
}

TEST(Checked_negate) {
    CHECK_EQUAL(C(-5), -C(5));
    CHECK_EQUAL(C(-INT_MAX), -C(INT_MAX));
    CHECK_EQUAL(C(INT_MAX), -C(-INT_MAX));
    CHECK_THROW(-C(INT_MIN), std::overflow_error);
}

TEST(Saturating_negate) {
    CHECK_EQUAL(S(-5), -S(5));
    CHECK_EQUAL(S(-INT_MAX), -S(INT_MAX));
    CHECK_EQUAL(S(INT_MAX), -S(INT_MIN));
}

TEST(Checked_abs) {
    CHECK_EQUAL(3, C(3).abs());
    CHECK_EQUAL(3, C(-3).abs());
    CHECK_EQUAL(INT_MAX, C(INT_MAX).abs());
    CHECK_EQUAL(INT_MAX, C(-INT_MAX).abs());
    CHECK_EQUAL(unsigned(INT_MAX) + 1, C(INT_MIN).abs());
}

TEST(Checked_plus) {
    CHECK_EQUAL(C(8), C(3) + C(5));
    CHECK_EQUAL(C(-1), C(INT_MIN) + C(INT_MAX));
    CHECK_THROW(C(INT_MAX) + C(INT_MAX), std::overflow_error);
    CHECK_THROW(C(INT_MIN) + C(INT_MIN), std::overflow_error);
    CHECK_THROW(C(INT_MAX) + C(1), std::overflow_error);
    CHECK_THROW(C(1) + C(INT_MAX), std::overflow_error);
    CHECK_THROW(C(INT_MAX - 5) + C(6), std::overflow_error);
    CHECK_THROW(C(INT_MIN) + C(-1), std::overflow_error);
}

TEST(Saturating_plus) {
    CHECK_EQUAL(S(8), S(3) + S(5));
    CHECK_EQUAL(S(-1), S(INT_MIN) + S(INT_MAX));
    CHECK_EQUAL(S(INT_MAX), S(INT_MAX) + S(INT_MAX));
    CHECK_EQUAL(S(INT_MIN), S(INT_MIN) + S(INT_MIN));
    CHECK_EQUAL(S(INT_MAX), S(INT_MAX) + S(1));
    CHECK_EQUAL(S(INT_MAX), S(1) + S(INT_MAX));
    CHECK_EQUAL(S(INT_MAX), S(INT_MAX - 5) + S(6));
    CHECK_EQUAL(S(INT_MIN), S(INT_MIN) + S(-1));
}

TEST(Checked_minus) {
    CHECK_EQUAL(C(3), C(8) - C(5));
    CHECK_EQUAL(C(-3), C(5) - C(8));
    CHECK_EQUAL(C(-13), C(-5) - C(8));
    CHECK_EQUAL(C(13), C(8) - C(-5));
    CHECK_EQUAL(C(-13), C(-8) - C(5));
    CHECK_EQUAL(C(13), C(5) - C(-8));
    CHECK_EQUAL(C(3), C(-5) - C(-8));
    CHECK_EQUAL(C(-3), C(-8) - C(-5));
    CHECK_EQUAL(C(INT_MAX - 3), C(INT_MAX) - C(3));
    CHECK_EQUAL(C(INT_MAX), C(INT_MAX - 2) - C(-2));
    CHECK_EQUAL(C(INT_MIN), C(INT_MIN + 3) - C(3));

    CHECK_THROW(C(INT_MAX) - C(-1), std::overflow_error);
    CHECK_THROW(C(INT_MAX - 2) - C(-3), std::overflow_error);
    CHECK_THROW(C(INT_MIN + 2) - C(3), std::overflow_error);
}

TEST(Checked_times) {
    CHECK_EQUAL(C(12), C(3) * C(4));
    CHECK_EQUAL(C(-12), C(-3) * C(4));
    CHECK_EQUAL(C(-12), C(3) * C(-4));
    CHECK_EQUAL(C(12), C(-3) * C(-4));

    CHECK_EQUAL(C(INT_MAX), C(INT_MAX / 2) * 2 + INT_MAX % 2);
    CHECK_EQUAL(C(INT_MAX), C(INT_MAX / 5) * 5 + INT_MAX % 5);
    CHECK_THROW(C(INT_MAX / 5) * 6, std::overflow_error);
    CHECK_THROW(C(INT_MAX / 5) * -6, std::overflow_error);
    CHECK_THROW(C(INT_MAX / -5) * 6, std::overflow_error);
    CHECK_THROW(C(INT_MAX / -5) * -6, std::overflow_error);
}

TEST(Saturating_times) {
    CHECK_EQUAL(S(12), S(3) * S(4));
    CHECK_EQUAL(S(-12), S(-3) * S(4));
    CHECK_EQUAL(S(-12), S(3) * S(-4));
    CHECK_EQUAL(S(12), S(-3) * S(-4));

    CHECK_EQUAL(S(INT_MAX), S(INT_MAX / 2) * 2 + INT_MAX % 2);
    CHECK_EQUAL(S(INT_MAX), S(INT_MAX / 5) * 5 + INT_MAX % 5);
    CHECK_EQUAL(S(INT_MAX), S(INT_MAX / 5) * 6);
    CHECK_EQUAL(S(INT_MIN), S(INT_MAX / 5) * -6);
    CHECK_EQUAL(S(INT_MIN), S(INT_MAX / -5) * 6);
    CHECK_EQUAL(S(INT_MAX), S(INT_MAX / -5) * -6);
}

TEST(Checked_left_shift) {
    CHECK_EQUAL(C(1), C(1) << 0);
    CHECK_EQUAL(C(2), C(1) << 1);
    CHECK_EQUAL(C(4), C(1) << 2);
    CHECK_THROW(C(INT_MAX) << 1, std::overflow_error);
}
