#include <int++/int++.h>
#include <iostream>

using namespace intpp;
using namespace std;

using C = Saturating<long>;

C operation(C a, C b)
{
    return a * b;
}
