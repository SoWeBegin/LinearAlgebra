# Cross Product
1) ```cpp
   constexpr vector& cross_product(const vector& rhs);
   ```
2) ```cpp
   template<typename T, std::size_t Size1>
   constexpr vector<T, Size1> cross_product(vector<T, Size1> lhs, const vector<T, Size1>& rhs);
   ```
3) ```cpp
   template<Concepts::arithmetic_char_const_excluded T, Concepts::arithmetic_char_const_excluded T2>
   constexpr auto complex_cross_product(const std::complex<T>& lhs, const std::complex<T2> rhs);
   ```
<br>

1) Computes the cross product of the current instance and `rhs`, storing the result in `this`.<br>
2) Same as 1), but the result is stored in a new `vector`, which has the same size and type of `lhs`.<br>
3) This function performs the cross product between two complex numbers. https://proofwiki.org/wiki/Definition:Vector_Cross_Product/Complex<br>

The cross product is defined for both arithmetic and complex types. For the latter, a normal cross product is performed, but for each resulting coordinate its conjugate is taken instead.
<br>In any case, the types of the elements of both vectors must be the same (this is to avoid mistakes such as trying to obtain the cross product between a complex vector and a non complex vector).<br>
**Note**: The cross product between two vectors is *only* defined for 3D vectors, that is, the size of the vector must be equal to 3.
<br> - For vectors whose underlying container is `std::array`, a static_assert is performed to ensure that both size are equal to 3.
<br> - For vectors whose underlying container is `std::vector`, an assert is performed. If the assert is disabled and the sizes are not 3, the function still attempts to perform the cross product, and undefined behavior occurs.<br>

# Inner Product (dot product)
1) ```cpp
   template<Concepts::underlying_vector_type T, std::size_t Size>
   constexpr T inner_product(const vector<T, Size>& lhs, const vector<T, Size>& other)
   ```
2) ```cpp
   template<ComplexInnerProduct definition_type, Concepts::underlying_vector_type T, std::size_t Size>
   constexpr T inner_product(const vector<T, Size>& lhs, const vector<T, Size>& other);
   ```
3) ```cpp
   template<Concepts::arithmetic_char_const_excluded T, Concepts::arithmetic_char_const_excluded T2>
   constexpr auto complex_inner_product(const std::complex<T>& lhs, const std::complex<T2>& rhs);
   ```
<br>

1) Computes the dot product between `lhs` and `other`. The result that is returned has the type `T`, which is the type of the passed vectors. To avoid mistakes such as trying to obtain the dot product between complex and non complex vectors,
both underlying vector types must be the same.
2) Same as 1), but works on complex vectors, that is, `std::complex` is used as the underlying type.
In this case, the complex dot product might have multiple, different definitions. By passing the correct template argument to this function, you are allowed to choose the convention that is used for the complex dot product between vectors `a` and `b`: 
- $a\cdot b=\sum_i \overline {a_i} b_i$ -> Template argument to pass is `MathLbr::ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT`
- $a\cdot b=\sum_i a_i \overline {b_i}$ -> Template argument to pass is `MathLbr::ComplexInnerProduct::ANTILINEAR_SECOND_ARGUMENT`
<br>**If no template argument is passed, option 1 is chosen (which is the most common).**
3) Performs the dot product between two complex numbers. https://proofwiki.org/wiki/Definition:Dot_Product/Complex

For functions 1) and 2), in case the underlying container is `std::vector`, an assert is performed to ensure that the sizes of the vectors match. If the asserts are disabled, the function attempts to calculate the dot products anyway, resulting in undefined behavior.
Otherwise, if the underlying container is `std::array` and the sizes differ, then this function cannot be called.

# Triple Products (Scalar and Vectorial)
1) ```cpp
   template<typename T, std::size_t Size>
   constexpr T scalar_triple_product(const vector<T, Size>& first, const vector<T, Size>& other2, const vector<T, Size>& other3);
   ```
2) ```cpp  
   template<typename T, std::size_t Size>
   constexpr vector<T, Size> vector_triple_product(const vector<T, Size>& first, const vector<T, Size>& other2, const vector<T, Size>& other3);
   ```
<br>

1) Performs the scalar triple product, and returns the result. All the sizes must be equal. For an underlying container `std::array`, this is ensured at compile time, while for an underlying container `std::vector`, an assert is performed to ensure that all sizes are equal. <br>
If a complex type is used as the vector's type, then a template argument related to the wished dot product definition can be passed (see: dot product above). If no template argument is passed, the default parameter is `ComplexInnerProduct::ANTILINEAR_FIRST,ARGUMENT`, which is the most common.
The scalar triple product is performed such as:

$other3\cdot (first X other2)$, where `X` is the cross operation.

2) Performs the vector triple product, returning the result in a new vector which has the same type and size as the passed vectors. All sizes must be equal 3, with the same rules as in 1). Unlike 1), the call for non-complex and complex types is identical (meaning that no special template argument regarding the wished dot product definition can be passed, since the cross product is the same for both anti linear in the first argument and antilinear in the second argument).
The vector triple product is performed such as:

$other3 X (first X other2)$, where `X` is the cross operation.

In all cases, if the underlying container is `std::vector` and the asserts are disabled, and vectors with different sizes are passed, the functions will attempt to perform the triple product anyway, resulting in undefined behavior.
<hr><br>


# Examples
```cpp
#include "vector.h"
#include <iostream>

int main() {
	// Normal cross and inner products
	MathLbr::vector<int> a{ {2,3,4} };
	MathLbr::vector<int> b{ {3,4,2} };
	MathLbr::vector<int> res = MathLbr::Vector::cross_product(a, b);
	auto res2 = MathLbr::Vector::inner_product(a, b);
	a.cross_product(b); // in place

	// Complex inner product between two complex vectors
	MathLbr::vector<std::complex<double>> c{ {{2,4},{6,7},{3,3}} };
	MathLbr::vector<std::complex<double>> d{ {{5,5},{9,8},{9,9}} };
	std::complex<double> res3 = MathLbr::Vector::inner_product(c, d); // Uses MathLbr::ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT by default

	// Inner product between two complex numbers
	std::complex<double> cmp{ 1,2 };
	std::complex<double> cmp2{ 3,3 };
	auto result = MathLbr::Vector::complex_inner_product(cmp, cmp2);
	auto result2 = MathLbr::Vector::complex_cross_product(cmp, cmp2);

	// Triple products
	MathLbr::vector<int> c1{ {5,5,5} };
	MathLbr::vector<std::complex<double>> e{ {3,4,2} };

	int result3 = MathLbr::Vector::scalar_triple_product(a, b, c1);
	auto result4 = MathLbr::Vector::vector_triple_product(c, d, e);
}
```
