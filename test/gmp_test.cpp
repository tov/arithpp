#include "arith++/arith++.h"
#include <gmpxx.h>
#include <rapidcheck.h>
#include <stdexcept>

using namespace arithpp;
using namespace std;

using Z = mpz_class;

bool operator<(const Z& a, const Z& b)
{
    return mpz_cmp(a.get_mpz_t(), b.get_mpz_t()) < 0;
}

template <class T, class Op>
bool check_binop(T a, T b, Op operation)
{
    using CT = Checked<T>;

    Z ma(a);
    Z mb(b);
    Z mc(operation(ma, mb));

    Z max(numeric_limits<T>::max());
    Z min(numeric_limits<T>::min());

    if (max < mc) {
        try {
            operation(CT(a), CT(b));
            return false;
        } catch (overflow_too_large& e) {
            return true;
        }
    }

    if (mc < min) {
        try {
            operation(CT(a), CT(b));
            return false;
        } catch (overflow_too_small& e) {
            return true;
        }
    }

    CT c(operation(a, b));
    if (is_signed<T>()) {
        return c.get() == mc.get_si();
    } else {
        return c.get() == mc.get_ui();
    }
}

template <class T>
void check_binops()
{
    T a_t;
    ostringstream params_s;
    params_s << "(" << typeid(a_t).name() << ", " << typeid(a_t).name() << ")";
    string params(params_s.str());

    rc::check("operator+" + params,
              [](T a, T b) {
                  RC_ASSERT(check_binop(a, b, [](auto a, auto b) {
                      return a + b;
                  }));
                  return true;
              });
    rc::check("operator-" + params,
              [](T a, T b) {
                  RC_ASSERT(check_binop(a, b, [](auto a, auto b) {
                      return a - b;
                  }));
                  return true;
              });
    rc::check("operator*" + params,
              [](T a, T b) {
                  RC_ASSERT(check_binop(a, b, [](auto a, auto b) {
                      return a * b;
                  }));
                  return true;
              });
    rc::check("operator/" + params,
              [](T a, T b) {
                  RC_PRE(b != 0);
                  RC_ASSERT(check_binop(a, b, [](auto a, auto b) {
                      return a / b;
                  }));
                  return true;
              });
}

int main()
{
    check_binops<char>();
    check_binops<unsigned char>();
    check_binops<signed char>();
    check_binops<short>();
    check_binops<unsigned short>();
    check_binops<int>();
    check_binops<unsigned int>();
    check_binops<long>();
    check_binops<unsigned long>();
}
