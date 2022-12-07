# MathLbr::vector Introduction
```cpp
template<
  Concepts::underlying_vector_type T, 
  std::size_t Size = dynamic_extent
> class vector;
```
`vector` is a header-only class that represents a mathematical vector, and its elements are stored continuously.
This class implements general vectorial operations and predicates.
Internally, elements are held inside a `std::array<T, Size>` if the `Size` template argument is provided by the user, otherwise they are held inside `std::vector<T>`.
Most member functions of this class are `constexpr` (see specific references).

```cpp
#include "vector.h"
int main() {
  MathLbr::vector<double> a{{2,3,4}}; // OK, internal container is std::vector since no template size was passed
  MathLbr::vector<int, 3> b{{2,4,3}}; // OK, internal container is std::array since a template size was passed
}
```

## Template parameters
**T**: The type of the elements. Currently, T must be only of the following:
  - Arithmetic types, char excluded <a href="https://en.cppreference.com/w/c/language/arithmetic_types">(cppreference: arithmetic types)</a>
  - `std::complex<T>` <a href="https://en.cppreference.com/w/cpp/numeric/complex">(cppreference: std::complex)</a>.</br>
*Note*: Some operations are not supported for `std::complex<T>` (either because they're still under development, or because they make no sense on such a type).

Support for user defined arithmetic type might be added in the future.

**Size**: Represents how many elements there are inside the vector.</br>
Passing a size through a template argument means that `MathLbr::vector` will internally use `std::array<T, Size>` to store its elements.</br>
Omitting this argument means that `MathLbr::vector` will internally use `std::vector<T>` to store its elements.</br>
*Note*: Some constructors might need different parameters, depending on what container is used to store the elements internally. (see specific references)

## Aliases
The vector class contains the usual, standard aliases:
```cpp
		using size_type = std::size_t;
		using value_type = T;
		using pointer_type = T*;
		using reference_type = T&;
		using const_pointer_type = const T*;
		using const_reference_type = const T&;
```
Additional aliases are:
```cpp
using underlying_container = std::conditional_t<Concepts::dynamic_extent_enabled<Size, dynamic_extent>, std::vector<T>, std::array<T, Size>>;
template<typename _T>
using complex_internal_value_type = typename Concepts::is_complex<_T>::value_type;
```
with 
- `underlying_container`: alias of the currently used underlying container (either `std::array` or `std::vector`),
- `complex_internal_value_type`: useful for the library itself, contains the underlying type of a passed `std::complex`.
For all the other details, see the member function page.
