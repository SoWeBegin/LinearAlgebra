# Overloaded operators
## operators- [substraction]

1. ```cpp
   template<typename T2, size_type Size2>
   constexpr vector& operator-=(const vector& rhs);
   ```
2. ```cpp
   template<Concepts::underlying_vector_type T2>
   constexpr vector& operator-=(T2 val);
   ```
3. ```cpp
   template<typename T2, size_type Size2>
   friend constexpr vector operator-(vector lhs, const vector& rhs);
   ```
4. ```cpp
   template<Concepts::underlying_vector_type T2>
   friend constexpr vector operator-(vector lhs, T2 val);
   ```
<br>

1) Substracts the contents of `rhs` from the current instance `this` in an element-wise manner. This operator overload effectively mutates the current instance.
The type of the elements of `rhs` must be convertible to the current's instance type `T`.
If the sizes are different, the smallest one is chosen. In this case, only the elements in the range of that size are substracted.
2) Substracts the value `val` from all elements in the current instance, effectively mutating it.
3) Same as 1), but does not mutate the current instance: it instead returns a new vector. The resulting vector has effectively the same size and type of the first vector parameter `lhs`.
4) Same as 2), but does not mutate the current instance: it instead returns a new vector, which has the same size and type of `lhs`.

In all cases, performing a `- or -=` on `unsigned` types such as `std::size_t` will result in a compiler warning (or error depending on your settings) to be issued.
<hr><br>

## Examples
```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<int> a2{ {1,2,3,4,5} };
	MathLbr::vector<int, 2> b2{ {1,2} };
	a2 -= 4;
	b2 - 4;
	
	MathLbr::vector<std::size_t> a3{ {2,3,2} };
	MathLbr::vector<std::size_t> b3{ {5,5} };
	a3 - b3; // warning (or compile error) issued: substracting  unsigned types is risky!
}
```
