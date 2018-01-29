#include "arith++/arith++.h"
#include <climits>

int main()
{
    int a = INT_MAX;
    int b = arithpp::safe_plus(a, 1);
}