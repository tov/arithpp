# ++int: checked integers for C++

This is a one-file, header-only library, so to use it, all you have to do is 
download [xxint/xxint.h][header] and put it somewhere in your include path.

## Usage

The main class template is `Checked`. For any built-in integer-type `T`, a 
`Checked<T>` behaves like `T` but it throws an exception when a `T` would 
overflow (or if unsigned, wrap around).

For example, here is a function that computes the factorial of an `int`, 
throwing an `xxint::overflow_too_large` exception if type `int` is not large 
enough to hold the result:

```cpp
#include <xxint.h>

int factorial(int n)
{
    xxint::Checked<int> result = 1;
    
    for (int i = 1; i <= n; ++i)
        result *= i;
    }
    
    return result.get()
}
```

Further documentation may be found [here][docs].

[header]:
    https://github.com/tov/xxint/raw/master/xxint/xxint.h

[docs]:
    https://tov.github.io/xxint/
