# Cross Product
1) ```cpp
   template<typename T2>
   constexpr vector& cross_product(const vector<T2, Size>& rhs);
   ```
2) ```cpp
   template<typename T, typename T2, std::size_t Size1>
   constexpr vector<T, Size1> cross_product(vector<T, Size1> lhs, const vector<T2, Size1>& rhs);
   ```
<br>

1) Computes the cross product of the current instance and `rhs`, storing the result in `this`.<br>
2) Same as 1), but the result is stored in a new `vector`, which has the same size and type of `lhs`.<br>

The cross product is defined for both arithmetic and complex types. For the latter, a normal cross product is performed on the conjugates of the complex elements.
<br>In any case, the types of the elements of both vectors must be the same (this is to avoid mistakes such as trying to obtain the cross product between a complex vector and a non complex vector).

# Inner Product (dot product)
1) ```cpp
   template<Concepts::underlying_vector_type T, std::size_t Size>
   constexpr T inner_product(const vector<T, Size>& lhs, const vector<T, Size>& other)
   ```
<br>

1) Computes the dot product between `lhs` and `other`. The result that is returned has the type `T`, which is the type of the passed vectors. To avoid mistakes such as trying to obtain the dot product between complex and non complex vectors,
both underlying vector types must be the same.
If `std::complex` is used as the underlying type, then the inner product performs the following formula: $∑ conj(a) * b$, with `conj(lhs[i]) == conj(ai)` and `other[i] == bi`.
<hr><br>

# Examples
```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<std::complex<double>> c{ {{2,4},{6,7},{3,3}} };
	MathLbr::vector<std::complex<double>> d{ {{5,5},{9,8},{9,9}} };
	std::cout << "inner product: " << MathLbr::Vector::inner_product(c, d) << std::endl;

	std::cout << "cross product: " << MathLbr::Vector::cross_product(c, d);
	c.cross_product(d);
}
```
