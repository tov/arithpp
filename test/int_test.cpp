#include <arith++/arith++.h>
#include <catch/catch.hpp>
#include <sstream>

using arithpp::Wrapping;
using W = Wrapping<int>;

using arithpp::Checked;
using C = Checked<int>;

using arithpp::Saturating;
using S = Saturating<int>;

TEST_CASE("convert_exn")
{
    using arithpp::convert_exn;;

    short five = 5;
    CHECK(5 == convert_exn<char>(five));
    CHECK(5 == convert_exn<signed char>(five));
    CHECK(5 == convert_exn<unsigned char>(five));
    CHECK(5 == convert_exn<short>(five));
    CHECK(5 == convert_exn<unsigned short>(five));
    CHECK(5 == convert_exn<int>(five));
    CHECK(5 == convert_exn<unsigned int>(five));
    CHECK(5 == convert_exn<long>(five));
    CHECK(5 == convert_exn<unsigned long>(five));

    unsigned short ufive = 5;
    CHECK(5 == convert_exn<char>(ufive));
    CHECK(5 == convert_exn<signed char>(ufive));
    CHECK(5 == convert_exn<unsigned char>(ufive));
    CHECK(5 == convert_exn<short>(ufive));
    CHECK(5 == convert_exn<unsigned short>(ufive));
    CHECK(5 == convert_exn<int>(ufive));
    CHECK(5 == convert_exn<unsigned int>(ufive));
    CHECK(5 == convert_exn<long>(ufive));
    CHECK(5 == convert_exn<unsigned long>(ufive));

    short nfive = -5;
    CHECK(-5 == convert_exn<char>(nfive));
    CHECK(-5 == convert_exn<signed char>(nfive));
    CHECK_THROWS_AS(convert_exn<unsigned char>(nfive), std::overflow_error);
    CHECK(-5 == convert_exn<short>(nfive));
    CHECK_THROWS_AS(convert_exn<unsigned short>(nfive),
                std::overflow_error);
    CHECK(-5 == convert_exn<int>(nfive));
    CHECK_THROWS_AS(convert_exn<unsigned int>(nfive), std::overflow_error);
    CHECK(-5 == convert_exn<long>(nfive));
    CHECK_THROWS_AS(convert_exn<unsigned long>(nfive), std::overflow_error);

    CHECK(SCHAR_MAX == convert_exn<signed char>(short(SCHAR_MAX)));
    CHECK_THROWS_AS(convert_exn<signed char>(short(SCHAR_MAX) + 1),
                std::overflow_error);
    CHECK(SCHAR_MIN == convert_exn<signed char>(short(SCHAR_MIN)));
    CHECK_THROWS_AS(convert_exn<signed char>(short(SCHAR_MIN) - 1),
                std::overflow_error);
}

TEST_CASE("Checked_char")
{
    using CC = Checked<char>;
    CHECK(CC(27) == CC(14) + 13);
    CHECK_THROWS_AS(CC(CHAR_MAX) + 1, std::overflow_error);
}

TEST_CASE("convert_exn_2") {
    using arithpp::convert_exn;;

    // signed to unsigned, narrower, negative
    CHECK_THROWS_AS(convert_exn<unsigned char>(short(-1)), std::overflow_error);
    // signed to unsigned, narrower, non-negative
    CHECK(1 == convert_exn<unsigned char>(short(1)));
    CHECK(UCHAR_MAX == convert_exn<unsigned char>(short(UCHAR_MAX)));
    // signed to unsigned, narrower, too big
    CHECK_THROWS_AS(convert_exn<unsigned char>(short(UCHAR_MAX) + 1),
                std::overflow_error);
    // signed to unsigned, same width, negative
    CHECK_THROWS_AS(convert_exn<unsigned short>(short(-1)), std::overflow_error);
    // signed to unsigned, same width, non-negative
    CHECK(1 == convert_exn<unsigned short>(short(1)));
    // signed to unsigned, wider, negative
    CHECK_THROWS_AS(convert_exn<unsigned long>(short(-1)), std::overflow_error);
    // signed to unsigned, wider, non-negative
    CHECK(1 == convert_exn<unsigned long>(short(1)));

    // signed to signed, narrower, fits
    CHECK(1 == convert_exn<signed char>(short(1)));
    CHECK(SCHAR_MIN == convert_exn<signed char>(short(SCHAR_MIN)));
    CHECK(SCHAR_MAX == convert_exn<signed char>(short(SCHAR_MAX)));
    // signed to signed, narrower, too small
    CHECK_THROWS_AS(convert_exn<signed char>(short(SCHAR_MIN) - 1), std::overflow_error);
    // signed to signed, narrower, too big
    CHECK_THROWS_AS(convert_exn<signed char>(short(SCHAR_MAX) + 1), std::overflow_error);
    // signed to signed, same width
    CHECK(1 == convert_exn<short>(short(1)));
    // signed to signed, wider
    CHECK(1 == convert_exn<long>(short(1)));

    // unsigned to unsigned, narrower, fits
    CHECK(1 == convert_exn<unsigned char>((unsigned short)1));
    CHECK(UCHAR_MAX ==
                convert_exn<unsigned char>((unsigned short)UCHAR_MAX));
    // unsigned to unsigned, narrower, too big
    CHECK_THROWS_AS(convert_exn<unsigned char>((unsigned short)(UCHAR_MAX) + 1),
                std::overflow_error);
    // unsigned to unsigned, same width
    CHECK(1 == convert_exn<unsigned short>((unsigned short)1));
    // unsigned to unsigned, wider
    CHECK(1 == convert_exn<unsigned long>((unsigned short)1));

    // unsigned to signed, narrower, fits
    CHECK(1 == convert_exn<signed char>((unsigned short) 1));
    // unsigned to signed, narrower, doesn't fit
    CHECK_THROWS_AS(convert_exn<signed char>((unsigned short) SCHAR_MAX + 1),
                std::overflow_error);
    // unsigned to signed, same width, fits
    CHECK(1 == convert_exn<short>((unsigned short) 1));
    // unsigned to signed, same width, doesn't fit
    CHECK_THROWS_AS(convert_exn<short>(
            static_cast<unsigned short>(
                    std::numeric_limits<short>::max()) + 1),
                std::overflow_error);
    // unsigned to signed, wider
    CHECK(1 == convert_exn<long>((unsigned short) 1));
}

TEST_CASE("Widen") {
    using arithpp::convert_widen;

    short five = 5;
    CHECK(5 == convert_widen<short>(five));
    CHECK(5 == convert_widen<long>(five));

    unsigned short ufive = 5;
    CHECK(5 == convert_widen<unsigned short>(ufive));
    CHECK(5 == convert_widen<unsigned long>(ufive));
    CHECK(5 == convert_widen<long>(ufive));
}

TEST_CASE("Wrapping") {
    CHECK(5 == W(5).get());
    CHECK(W(INT_MIN + 4) == W(INT_MAX) + W(5));
}

TEST_CASE("Widening_conversions") {
    Wrapping<short> sfive_w = 5;
    Wrapping<long> lfive_w = sfive_w;

    Checked<short> sfive_c = 5;
    Checked<long> lfive_c = sfive_c;
}

TEST_CASE("Convert_method") {
    using CS = Checked<short>;
    using CL = Checked<long>;
    CHECK(CL(5) == CS(5).convert<long>());
    CHECK(CL(std::numeric_limits<short>::min()) ==
                CS(std::numeric_limits<short>::min()).convert<long>());
    CHECK(CL(std::numeric_limits<short>::max()) ==
                CS(std::numeric_limits<short>::max()).convert<long>());
    CHECK(CS(std::numeric_limits<short>::max()) ==
                CL(std::numeric_limits<short>::max()).convert<short>());
    CHECK(CS(std::numeric_limits<short>::min()) ==
                CL(std::numeric_limits<short>::min()).convert<short>());
    CHECK_THROWS_AS((CL(std::numeric_limits<short>::max()) + 1).convert<short>(),
                std::overflow_error);
    CHECK_THROWS_AS((CL(std::numeric_limits<short>::min()) - 1).convert<short>(),
                std::overflow_error);
}

TEST_CASE("Checked_comparisons")
{
    using CU = Checked<unsigned int>;

    CHECK(C(5) == C(5));
    CHECK(CU(5) == C(5));
    CHECK(CU(5) != C(-5));
    CHECK(CU(UINT_MAX) != C(-1));
    CHECK(UINT_MAX == -1);

    CHECK(C(5) < C(6));
    CHECK(!(C(5) < C(5)));
    CHECK(!(C(6) < C(5)));
    CHECK(C(-1) < CU(1));
    CHECK(intmax_t(-1) > uintmax_t(1));
    CHECK(Checked<intmax_t>(-1) < Checked<uintmax_t>(1));
}

TEST_CASE("Checked_construction") {
    CHECK(5 == C(5).get());
}

TEST_CASE("Checked_negate") {
    CHECK(C(-5) == -C(5));
    CHECK(C(-INT_MAX) == -C(INT_MAX));
    CHECK(C(INT_MAX) == -C(-INT_MAX));
    CHECK_THROWS_AS(-C(INT_MIN), std::overflow_error);
}

TEST_CASE("Saturating_negate") {
    CHECK(S(-5) == -S(5));
    CHECK(S(-INT_MAX) == -S(INT_MAX));
    CHECK(S(INT_MAX) == -S(INT_MIN));
}

TEST_CASE("Checked_abs") {
    CHECK(3 == C(3).abs());
    CHECK(3 == C(-3).abs());
    CHECK(INT_MAX == C(INT_MAX).abs());
    CHECK(INT_MAX == C(-INT_MAX).abs());
    CHECK(unsigned(INT_MAX) + 1 == C(INT_MIN).abs());
}

TEST_CASE("Checked_plus") {
    CHECK(C(8) == C(3) + C(5));
    CHECK(C(-1) == C(INT_MIN) + C(INT_MAX));
    CHECK_THROWS_AS(C(INT_MAX) + C(INT_MAX), std::overflow_error);
    CHECK_THROWS_AS(C(INT_MIN) + C(INT_MIN), std::overflow_error);
    CHECK_THROWS_AS(C(INT_MAX) + C(1), std::overflow_error);
    CHECK_THROWS_AS(C(1) + C(INT_MAX), std::overflow_error);
    CHECK_THROWS_AS(C(INT_MAX - 5) + C(6), std::overflow_error);
    CHECK_THROWS_AS(C(INT_MIN) + C(-1), std::overflow_error);
}

TEST_CASE("Checked_unsigned") {
    using CU = arithpp::Checked<unsigned>;

    CHECK(CU(8) == CU(3) + CU(5));
    CHECK_THROWS_AS(CU(UINT_MAX) + CU(UINT_MAX), std::overflow_error);
    CHECK_THROWS_AS(CU(UINT_MAX) + CU(1), std::overflow_error);
    CHECK_THROWS_AS(CU(UINT_MAX - 5) + CU(6), std::overflow_error);

    CHECK_THROWS_AS(CU(0) - CU(1), std::overflow_error);
}

TEST_CASE("Saturating_plus") {
    CHECK(S(8) == S(3) + S(5));
    CHECK(S(-1) == S(INT_MIN) + S(INT_MAX));
    CHECK(S(INT_MAX) == S(INT_MAX) + S(INT_MAX));
    CHECK(S(INT_MIN) == S(INT_MIN) + S(INT_MIN));
    CHECK(S(INT_MAX) == S(INT_MAX) + S(1));
    CHECK(S(INT_MAX) == S(1) + S(INT_MAX));
    CHECK(S(INT_MAX) == S(INT_MAX - 5) + S(6));
    CHECK(S(INT_MIN) == S(INT_MIN) + S(-1));
}

TEST_CASE("Checked_minus") {
    CHECK(C(3) == C(8) - C(5));
    CHECK(C(-3) == C(5) - C(8));
    CHECK(C(-13) == C(-5) - C(8));
    CHECK(C(13) == C(8) - C(-5));
    CHECK(C(-13) == C(-8) - C(5));
    CHECK(C(13) == C(5) - C(-8));
    CHECK(C(3) == C(-5) - C(-8));
    CHECK(C(-3) == C(-8) - C(-5));
    CHECK(C(INT_MAX - 3) == C(INT_MAX) - C(3));
    CHECK(C(INT_MAX) == C(INT_MAX - 2) - C(-2));
    CHECK(C(INT_MIN) == C(INT_MIN + 3) - C(3));

    CHECK_THROWS_AS(C(INT_MAX) - C(-1), std::overflow_error);
    CHECK_THROWS_AS(C(INT_MAX - 2) - C(-3), std::overflow_error);
    CHECK_THROWS_AS(C(INT_MIN + 2) - C(3), std::overflow_error);
}

TEST_CASE("Checked_times") {
    CHECK(C(12) == C(3) * C(4));
    CHECK(C(-12) == C(-3) * C(4));
    CHECK(C(-12) == C(3) * C(-4));
    CHECK(C(12) == C(-3) * C(-4));

    CHECK(C(INT_MAX) == C(INT_MAX / 2) * 2 + INT_MAX % 2);
    CHECK(C(INT_MAX) == C(INT_MAX / 5) * 5 + INT_MAX % 5);
    CHECK_THROWS_AS(C(INT_MAX / 5) * 6, std::overflow_error);
    CHECK_THROWS_AS(C(INT_MAX / 5) * -6, std::overflow_error);
    CHECK_THROWS_AS(C(INT_MAX / -5) * 6, std::overflow_error);
    CHECK_THROWS_AS(C(INT_MAX / -5) * -6, std::overflow_error);
}

TEST_CASE("Saturating_times") {
    CHECK(S(12) == S(3) * S(4));
    CHECK(S(-12) == S(-3) * S(4));
    CHECK(S(-12) == S(3) * S(-4));
    CHECK(S(12) == S(-3) * S(-4));

    CHECK(S(INT_MAX) == S(INT_MAX / 2) * 2 + INT_MAX % 2);
    CHECK(S(INT_MAX) == S(INT_MAX / 5) * 5 + INT_MAX % 5);
    CHECK(S(INT_MAX) == S(INT_MAX / 5) * 6);
    CHECK(S(INT_MIN) == S(INT_MAX / 5) * -6);
    CHECK(S(INT_MIN) == S(INT_MAX / -5) * 6);
    CHECK(S(INT_MAX) == S(INT_MAX / -5) * -6);
}

TEST_CASE("Checked_left_shift") {
    CHECK(C(1) == C(1) << 0);
    CHECK(C(2) == C(1) << 1);
    CHECK(C(4) == C(1) << 2);
    CHECK_THROWS_AS(C(INT_MAX) << 1, std::overflow_error);
    CHECK(C(INT_MAX ^ 0x7) == C(INT_MAX) >> 3 << 3);
    CHECK_THROWS_AS(C(INT_MAX) >> 3 << 4, std::overflow_error);
}

TEST_CASE("Wrapping_print") {
    Wrapping<int> x(-1);
    std::ostringstream os;

    os << x;
    CHECK("-1" == os.str());
}
