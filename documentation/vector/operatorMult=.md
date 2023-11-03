# Overloaded operators
## operators *

1) ```cpp
   template<Concepts::underlying_vector_type T2>
   constexpr vector& operator*=(T2 lambda);
   ```
2) ```cpp
   template<Concepts::underlying_vector_type T2>
   friend constexpr vector operator*(vector lhs, T2 lambda);
   ```
3) ```cpp
   template<Concepts::underlying_vector_type T2>
   friend constexpr vector operator*(T2 lambda, vector lhs);
   ```
<br>

1) Multiplies each element of the current vector with the value of `lambda`, effectively mutating the current instance `this`. 
2) Same as 1), but does not mutate the container: instead, a new `vector` is returned (which as the same size and type of `lhs`).
3) Same as 2), but with the arguments in reverse order.

In all cases, a compiler warning (or error, depending on your settings) is issued in case a vector of `unsigned` types is multiplied with a `lambda` of any type.
<br>*Note*: This overloaded operator does **not** perform dot or cross products. Instead, it performs normal *scalar* multiplications on each element.
<hr>

## Examples
```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<int> a2{ {1,2,3,4,5} };
	MathLbr::vector<int, 2> b2{ {1,2} };
	a2 *= 4;
	b2 * 4;
	4 * a2;
	MathLbr::vector<std::size_t> a3{ {2,3,2} };
	MathLbr::vector<std::size_t> b3{ {5,5} };
	a3 * b3; // Doesn't make sense and is thus disallowed
}
```
  
