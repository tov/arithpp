#include "arith++/arith++.h"
#include "UnitTest++/UnitTest++.h"

using W = arithpp::Wrapping<int>;
using C = arithpp::Checked<int>;
using S = arithpp::Saturating<int>;

int main()
{
    return UnitTest::RunAllTests();
}

TEST(convert_exn)
{
    using arithpp::convert_exn;;

    short five = 5;
    CHECK_EQUAL(5, convert_exn<char>(five));
    CHECK_EQUAL(5, convert_exn<signed char>(five));
    CHECK_EQUAL(5, convert_exn<unsigned char>(five));
    CHECK_EQUAL(5, convert_exn<short>(five));
    CHECK_EQUAL(5, convert_exn<unsigned short>(five));
    CHECK_EQUAL(5, convert_exn<int>(five));
    CHECK_EQUAL(5, convert_exn<unsigned int>(five));
    CHECK_EQUAL(5, convert_exn<long>(five));
    CHECK_EQUAL(5, convert_exn<unsigned long>(five));

    unsigned short ufive = 5;
    CHECK_EQUAL(5, convert_exn<char>(ufive));
    CHECK_EQUAL(5, convert_exn<signed char>(ufive));
    CHECK_EQUAL(5, convert_exn<unsigned char>(ufive));
    CHECK_EQUAL(5, convert_exn<short>(ufive));
    CHECK_EQUAL(5, convert_exn<unsigned short>(ufive));
    CHECK_EQUAL(5, convert_exn<int>(ufive));
    CHECK_EQUAL(5, convert_exn<unsigned int>(ufive));
    CHECK_EQUAL(5, convert_exn<long>(ufive));
    CHECK_EQUAL(5, convert_exn<unsigned long>(ufive));

    short nfive = -5;
    CHECK_EQUAL(-5, convert_exn<char>(nfive));
    CHECK_EQUAL(-5, convert_exn<signed char>(nfive));
    CHECK_THROW(convert_exn<unsigned char>(nfive), std::overflow_error);
    CHECK_EQUAL(-5, convert_exn<short>(nfive));
    CHECK_THROW(convert_exn<unsigned short>(nfive),
                std::overflow_error);
    CHECK_EQUAL(-5, convert_exn<int>(nfive));
    CHECK_THROW(convert_exn<unsigned int>(nfive), std::overflow_error);
    CHECK_EQUAL(-5, convert_exn<long>(nfive));
    CHECK_THROW(convert_exn<unsigned long>(nfive), std::overflow_error);

    CHECK_EQUAL(SCHAR_MAX, convert_exn<signed char>(short(SCHAR_MAX)));
    CHECK_THROW(convert_exn<signed char>(short(SCHAR_MAX) + 1),
                std::overflow_error);
    CHECK_EQUAL(SCHAR_MIN, convert_exn<signed char>(short(SCHAR_MIN)));
    CHECK_THROW(convert_exn<signed char>(short(SCHAR_MIN) - 1),
                std::overflow_error);
}

TEST(convert_exn_2) {
    using arithpp::convert_exn;;

    // signed to unsigned, narrower, negative
    CHECK_THROW(convert_exn<unsigned char>(short(-1)), std::overflow_error);
    // signed to unsigned, narrower, non-negative
    CHECK_EQUAL(1, convert_exn<unsigned char>(short(1)));
    CHECK_EQUAL(UCHAR_MAX, convert_exn<unsigned char>(short(UCHAR_MAX)));
    // signed to unsigned, narrower, too big
    CHECK_THROW(convert_exn<unsigned char>(short(UCHAR_MAX) + 1),
                std::overflow_error);
    // signed to unsigned, same width, negative
    CHECK_THROW(convert_exn<unsigned short>(short(-1)), std::overflow_error);
    // signed to unsigned, same width, non-negative
    CHECK_EQUAL(1, convert_exn<unsigned short>(short(1)));
    // signed to unsigned, wider, negative
    CHECK_THROW(convert_exn<unsigned long>(short(-1)), std::overflow_error);
    // signed to unsigned, wider, non-negative
    CHECK_EQUAL(1, convert_exn<unsigned long>(short(1)));

    // signed to signed, narrower, fits
    CHECK_EQUAL(1, convert_exn<signed char>(short(1)));
    CHECK_EQUAL(SCHAR_MIN, convert_exn<signed char>(short(SCHAR_MIN)));
    CHECK_EQUAL(SCHAR_MAX, convert_exn<signed char>(short(SCHAR_MAX)));
    // signed to signed, narrower, too small
    CHECK_THROW(convert_exn<signed char>(short(SCHAR_MIN) - 1), std::overflow_error);
    // signed to signed, narrower, too big
    CHECK_THROW(convert_exn<signed char>(short(SCHAR_MAX) + 1), std::overflow_error);
    // signed to signed, same width
    CHECK_EQUAL(1, convert_exn<short>(short(1)));
    // signed to signed, wider
    CHECK_EQUAL(1, convert_exn<long>(short(1)));

    // unsigned to unsigned, narrower, fits
    CHECK_EQUAL(1, convert_exn<unsigned char>((unsigned short)1));
    CHECK_EQUAL(UCHAR_MAX,
                convert_exn<unsigned char>((unsigned short)UCHAR_MAX));
    // unsigned to unsigned, narrower, too big
    CHECK_THROW(convert_exn<unsigned char>((unsigned short)(UCHAR_MAX) + 1),
                std::overflow_error);
    // unsigned to unsigned, same width
    CHECK_EQUAL(1, convert_exn<unsigned short>((unsigned short)1));
    // unsigned to unsigned, wider
    CHECK_EQUAL(1, convert_exn<unsigned long>((unsigned short)1));

    // unsigned to signed, narrower, fits
    CHECK_EQUAL(1, convert_exn<signed char>((unsigned short) 1));
    // unsigned to signed, narrower, doesn't fit
    CHECK_THROW(convert_exn<signed char>((unsigned short) SCHAR_MAX + 1),
                std::overflow_error);
    // unsigned to signed, same width, fits
    CHECK_EQUAL(1, convert_exn<signed short>((unsigned short) 1));
    // unsigned to signed, same width, doesn't fit
    CHECK_THROW(convert_exn<signed short>(
            static_cast<unsigned short>(
                    std::numeric_limits<signed short>::max()) + 1),
                std::overflow_error);
    // unsigned to signed, wider
    CHECK_EQUAL(1, convert_exn<signed long>((unsigned short) 1));
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

TEST(Checked_unsigned) {
    using CU = arithpp::Checked<unsigned>;

    CHECK_EQUAL(CU(8), CU(3) + CU(5));
    CHECK_THROW(CU(UINT_MAX) + CU(UINT_MAX), std::overflow_error);
    CHECK_THROW(CU(UINT_MAX) + CU(1), std::overflow_error);
    CHECK_THROW(CU(UINT_MAX - 5) + CU(6), std::overflow_error);

    CHECK_THROW(CU(0) - CU(1), std::overflow_error);
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

