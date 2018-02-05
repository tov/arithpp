#include "arith++/arith++.h"
#include <gmpxx.h>
#include <rapidcheck.h>
#include <cstdint>

using namespace arithpp;
using namespace std;

using Z = mpz_class;

bool operator<(const Z& a, const Z& b)
{
    return mpz_cmp(a.get_mpz_t(), b.get_mpz_t()) < 0;
}

Z operator<<(const Z& a, unsigned long b)
{
    Z result;
    Z base(2);
    mpz_pow_ui(result.get_mpz_t(), base.get_mpz_t(), b);
    return a * result;
}

template <class T>
struct Check
{
    using CT = Checked<T>;

    Z max;
    Z min;

    Check() : max(numeric_limits<T>::max())
            , min(numeric_limits<T>::min())
    { }

    template <class Thunk>
    bool check_against(const Z& mc, Thunk thunk)
    {
        if (max < mc) {
            try {
                thunk();
                return false;
            } catch (overflow_too_large& e) {
                return true;
            }
        }

        if (mc < min) {
            try {
                thunk();
                return false;
            } catch (overflow_too_small& e) {
                return true;
            }
        }

        CT c(thunk());
        if (is_signed<T>()) {
            return c.get() == mc.get_si();
        } else {
            return c.get() == mc.get_ui();
        }
    }

    template <class Op>
    bool unop(T a, Op operation)
    {
        Z ma(a);
        Z mc(operation(ma));
        return check_against(mc, [&]() { return operation(CT(a)); });
    }

    template <class Op>
    bool binop(T a, T b, Op operation)
    {
        Z ma(a);
        Z mb(b);
        Z mc(operation(ma, mb));
        return check_against(mc, [&]() { return operation(CT(a), CT(b)); });
    }

     bool lshiftop(T a, u_int8_t b)
     {
         Z ma(a);
         Z mc(ma << b);
         return check_against(mc, [=]() { return CT(a) << b; });
     }
};

template <template <class> class F>
void apply_to_types()
{
    F<char>::go();
    F<unsigned char>::go();
    F<signed char>::go();
    F<short>::go();
    F<unsigned short>::go();
    F<int>::go();
    F<unsigned int>::go();
    F<long>::go();
    F<unsigned long>::go();
}

template <class T>
struct Check_conversions
{
    template <class U>
    struct Loop
    {
        static void go()
        {
            T a_t;
            U a_u;
            string t_s = typeid(a_t).name();
            string u_s = typeid(a_u).name();
            string description = "conversion from " + t_s + " to " + u_s;

            Z min_u(numeric_limits<U>::min());
            Z max_u(numeric_limits<U>::max());

            rc::check(description,
                      [&](T a) {
                          Z ma(a);

                          if (ma < min_u) {
                              try {
                                  Checked<U> c(a);
                                  return false;
                              } catch (overflow_too_small& e) {
                                  return true;
                              }
                          } else if (max_u < ma) {
                              try {
                                  Checked<U> c(a);
                                  return false;
                              } catch (overflow_too_large& e) {
                                  return true;
                              }
                          } else {
                              Checked<U> c(a);
                              return c.get() == a;
                          }
                      });
        }
    };

    static void go()
    {
        apply_to_types<Loop>();
    }
};

template <class T>
struct Check_operations
{
    static void go()
    {
        T a_t;
        string t_s = typeid(a_t).name();
        string param = "(" + t_s + ")";
        string params = "(" + t_s + ", " + t_s + ")";

        Check<T> check;

        rc::check("operator+" + params,
                  [&](T a, T b) {
                      RC_ASSERT(check.binop(a, b, [](auto a, auto b) {
                          return a + b;
                      }));
                      return true;
                  });
        rc::check("operator-" + params,
                  [&](T a, T b) {
                      RC_ASSERT(check.binop(a, b, [](auto a, auto b) {
                          return a - b;
                      }));
                      return true;
                  });
        rc::check("operator*" + params,
                  [&](T a, T b) {
                      RC_ASSERT(check.binop(a, b, [](auto a, auto b) {
                          return a * b;
                      }));
                      return true;
                  });
        rc::check("operator/" + params,
                  [&](T a, T b) {
                      RC_PRE(b != 0);
                      RC_ASSERT(check.binop(a, b, [](auto a, auto b) {
                          return a / b;
                      }));
                      return true;
                  });
        rc::check("operator-" + param,
                  [&](T a) {
                      RC_ASSERT(check.unop(a, [](auto a) {
                          return -a;
                      }));
                      return true;
                  });
        rc::check("operator<<(" + t_s + ", u_int8_t)",
                  [&](T a, unsigned char b) {
                      RC_PRE(a >= 0);
                      RC_ASSERT(check.lshiftop(a, b));
                      return true;
                  });
    }
};

int main()
{
//    apply_to_types<Check_operations>();
    apply_to_types<Check_conversions>();

//    Checked<signed char> x(128);
}
