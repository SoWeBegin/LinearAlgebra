# Overloaded operators
## operators/= and operators%
1) ```cpp
   template<Concepts::underlying_vector_type T2>
   constexpr vector& operator/=(T2 lambda);
   ```
2) ```cpp
   template<Concepts::underlying_vector_type T2>
   friend constexpr vector operator/(vector lhs, T2 lambda);
   ```
3) ```cpp
   template<Concepts::underlying_vector_type T2>
   constexpr vector& operator%=(T2 lambda);
   ```
4) ```cpp
   template<Concepts::underlying_vector_type T2>
   friend constexpr vector operator%(vector lhs, T2 lambda);
   ```
<br>

1) Divides each element of the vector by `lambda`, and stores the result in the current instance `this`.
2) Same as 1), but returns a new vector where the results are stored. The new vector has same type and size of `lhs`.
3) Performs the modulo of `lambda` on each element, storing the result in the current instance `this`.
4) Same as 3). but returns a new vector where the results are stored. The new vector has same type and size of `lhs`.

In all cases, an assert is performed to ensure that `lambda` is different than zero.<br>
*Note*: For 3) and 4), both `lambda` and the current vector must have integral types. That also means that you cannot call this function on a vector of `std::complex` objects.
<hr><br>

## Examples
```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<std::complex<double>> c{ {{2,4},{6,7},{3,3}} };
	MathLbr::vector<std::complex<double>> d{ {{5,5},{9,8},{9,9}} };
	c /= 3;
	d / 4;
	//d %= 3; // Disallowed
	MathLbr::vector<int> a{ {2,3,4} };
	a %= 2;
}
```
