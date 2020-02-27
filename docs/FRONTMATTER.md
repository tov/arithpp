# ++int: checked integers for C++

## Get it

The latest version is available [on GitHub](https://github.com/tov/xxint).

## Use it

The main class template is `xxint::Checked`. For any built-in integer-type
`T`, an `xxint::Checked<T>` behaves like `T` but it throws an exception
when the result of an operation would not fit in type `T`.

For example, here is a function that computes the factorial of an `int`,
throwing an `xxint::overflow_too_large` exception if type `int` is not large
enough to hold the result:

```cpp
#include <int++.h>

using namespace xxint;

int factorial(int n)
{
    Checked<int> result = 1;

    for (int i = 1; i <= n; ++i)
        result *= i;
    }

    return result.get()
}
```

If we want `factorial` to saturate and return `INT_MAX` instead of
overflowing, we can replace `Checked<int>` with
`Checked<int, policy::saturating>`. (There's a type alias, so we can
also write `Saturating<int>` instead for this type.)

The library also provides checked conversions and mathematically-correct
mixed-sign comparisons between `Checked` types.
See the [`xxint`](namespacexxint.html) namespace, which contains
everything this library defines.

