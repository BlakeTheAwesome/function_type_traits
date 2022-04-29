# function_type_traits
## About
This is a C++17 single-header library that allows you to extract type information from function types including member functions.

The information available is:
- Return Type
- Argument list
- Whether or not the function is variadic
- Noexcept specifier
- Associated class type (non-static member functions only)
- CV qualifiers (non-static member functions only)
- L-ref and R-ref qualifiers (non-static member functions only)

## Installation
1) Place the [function_type_traits.h](./function_type_traits/function_type_traits.h) file somewhere
1) `#include` the file

A [c++20 module](./function_type_traits/function_type_traits.ixx) has also been provided if you're using a compatible compiler and wish to ensure the `detail` namespace doesn't leak out anywhere.

## Usage
The `function_type_traits` namespace has two exposed struct templates - `fn_type_traits` and `fn_traits`. The former works on a function type, while the latter works with function pointers - it is provided to simplify calling code, but uses the same logic.

For example these yield identical types: `fn_type_traits<float(float, float)>` and `fn_traits<&fminf>`.

Instanciating these templates will yield a struct with the following fields:
- `return_type` - A type declaration containing the return type of the function
- `arg_types` - A type that is a `std::tuple<>` of the function argument types
- `arity` - A constexpr bool containing the number of function arguments
- `arg_t<N>` - A way to get a specific argument type. For example `fn_traits<&printf>::arg_t<0>` returns `const char*`
- `variadic` - A constexpr bool of whether the function is variadic (i.e. `printf()`)
- `noexcept_v` - A constexpr bool that matches the functions `noexcept` specifier
- `class_type` - For a non-static member function, this is the type of the associated class. Otherwise it is `void`
- `const_qualified` - A constexpr bool that is true for a member function marked const
- `volatile_qualified` - A constexpr bool that is true for a member function marked volatile
- `lref_qualified` - A constexpr bool that is true for an l-ref qualified member function
- `rref_qualified` - A constexpr bool that is true for an r-ref qualified member function
- `identity` - The type of the internal structure after pointers and such have been stripped away. This can be used to ensure type structures are only generated once per function signature. For example the following is true:
  `std::is_same_v<fn_traits<&fminf>::identity, fn_type_traits<decltype(&fminf)>::identity>`

## Examples
All the static assertions in this section will pass.

### Free Functions
```cpp
#include "function_type_traits.h"
#include <type_traits>
#include <tuple>
#include <cstdio>
#include <cmath>

// Test function with signature: float fminf(float, float)
using fminf_traits = fn_traits<&fminf>;
static_assert(std::is_same_v<fminf_traits::return_type, float>);
static_assert(std::is_same_v<fminf_traits::arg_types, std::tuple<float, float>>);
static_assert(std::is_same_v<printf_traits::arg_t<0>, float>);
static_assert(fminf_traits::arity == 2);
static_assert(fminf_traits::variadic == false);

// Test function with signature: int printf(const char*, ...)
using printf_traits = fn_traits<&printf>;
static_assert(std::is_same_v<printf_traits::return_type, int>);
static_assert(std::is_same_v<printf_traits::arg_types, std::tuple<const char*>>);
static_assert(std::is_same_v<printf_traits::arg_t<0>, const char*>);
static_assert(printf_traits::arity == 1);
static_assert(printf_traits::variadic == true);
```

### Member Functions
```cpp
#include "function_type_traits.h"
#include <type_traits>
#include <tuple>

struct cls {
    void ArgFn(char, int, double*) {}
    void ConstFn() const {}
    void LRefFn() & {}
    static void StaticFn() {}
};

using arg_fn_traits = fn_traits<&cls::ArgFn>;
using const_fn_traits = fn_traits<&cls::ConstFn>;
using lref_fn_traits = fn_traits<&cls::LRefFn>;
using static_fn_traits = fn_traits<&cls::StaticFn>;


// Extract class type
static_assert(std::is_same_v<arg_fn_traits::class_type, cls>);
static_assert(std::is_same_v<static_fn_traits::class_type, void>);

// Extract Function Arguments
static_assert(std::is_same_v<arg_fn_traits::arg_types, std::tuple<char, int, double*>>);
static_assert(std::is_same_v<arg_fn_traits::arg_t<0>, char>);
static_assert(std::is_same_v<arg_fn_traits::arg_t<1>, int>);
static_assert(std::is_same_v<arg_fn_traits::arg_t<2>, double*>);

// Extract const-qualifier
static_assert(arg_fn_traits::const_qualified == false);
static_assert(const_fn_traits::const_qualified == true);

// Extract lref-qualifier
static_assert(const_fn_traits::lref_qualified == false);
static_assert(lref_fn_traits::lref_qualified == true);
```


## Tested Compilers
This code has been tested with:
- `Visual Studio 17.1.6` with `/W4` alongside both `/std:c++17` and `/std:c++20`
- `Clang 14.0.0` with `-Wall -Wextra` alongside both `-std=c++17` and `-std=c++20`
- `GCC 11.3` with `-Wall -Wextra` alongside both `-std=c++17` and `-std=c++20`

## License
This is distributed under a Boost Software License, which is basically just an MIT license except it explicitly states that you don't need to include a notice in compiled code.