# Projection

1) ```cpp
   constexpr vector& vector_projection_from(const vector& other);
   ```
2) ```cpp
   template<typename T, std::size_t Size>
   constexpr vector<T, Size> projection(vector<T, Size> first, const vector<T, Size>& other);
   ```
3) ```cpp
   template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT>
   constexpr vector& vector_projection_from(const vector& other) 
   ```
4) ```cpp
   template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT, typename T, std::size_t Size>
   constexpr vector<T, Size> projection(vector<T, Size> first, const vector<T, Size>& other)
   ```
  
<br>

1) Projects `other` on the current instance `this`, storing the result in `this`. An assert is performed to ensure that both sizes are equal. A further assert is performed to ensure that inner_product() does not equal 0 (since it is used as a denominator to calculate the projection).
If asserts are disabled and the sizes are not equal, the function still attempts to calculate the projection, which results in undefined behavior.
As it currently stands, this function is only defined for non-complex vectors. That is, using `std::complex` as the underlying vector's type will disallow it.
2) Same as 1), but projects `first` on `other`, resulting in a new vector with the same type `T` and `Size`. All the other rules explained in point 1) stand: internally, this functions calls 1).
3) Same as 1), but works for complex vector. As usual, a template argument can be passed in order to choose the wished definition of the complex dot product. If no 
argument is passed, the default complex dot product used is ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT.
4) Same as 3), but projects `first` on `other`, resulting in a new vector with the same type `T` and `Size`. All other rules explained in 1) stand: internally, this function calls 3).

 
 ## Examples
```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<std::complex<double>> c{ { {0,1},{1,1},{1,0} } };
	MathLbr::vector<std::complex<double>> d{ {{0,0},{1 / sqrt(3),0},{-1 / sqrt(3),1 / sqrt(3)}} };

	MathLbr::vector<std::complex<double>> complex_projection = MathLbr::Vector::projection(c, d);
	c.vector_projection_from(d);

	MathLbr::vector<int> a{ {23,3,3,4} };
	MathLbr::vector<int> b{ {5,5,0,2} };
	MathLbr::vector<int> normal_projection = MathLbr::Vector::projection(a, b);
	a.vector_projection_from(b);
}
```
