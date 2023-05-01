# MathLbr::vector::vector
1. ```cpp
   constexpr vector() = default;
   ```
2. ```cpp
   template<concepts::underlying_vector_type T2>
   constexpr explicit(not concepts::allow_implicit_conversions::value) vector(T2 value); 
   ```
3. ```cpp
   constexpr explicit(not concepts::allow_implicit_conversions::value) vector(size_type count);
   ```
4. ```cpp
   template<std::input_iterator InputIter>
   constexpr vector(InputIter first, InputIter last);
   ```
5. ```cpp
   template<size_type Sz>
   constexpr explicit vector(const value_type(&arr)[Sz]);
   ```
6. ```cpp
   constexpr explicit vector(complex_internal_value_type<T> lower, complex_internal_value_type<T> higher, size_type count = 1);
   ```
7. ```cpp
   explicit vector(value_type lower, value_type higher, size_type count = 1);
   ```
8. ```cpp
   template<typename T2, size_type Size2>
   constexpr vector(const vector<T2, Size2>& other);
   ```
9. ```cpp
   template<typename T2, size_type Size2>
   constexpr vector(vector<T2, Size2>&& other) noexcept (std::is_nothrow_move_constructible_v<T>)
   ```
9. ```cpp
   constexpr vector(const vector& other) = default;
   constexpr vector(vector&& other) noexcept = default;
   constexpr vector& operator=(vector&& other) noexcept = default;
   constexpr vector& operator=(const vector& other) = default;
   ```
<hr><br>
  
1) **Default constructor**. Constructs an empty vector, as in by using `= default`.
2) Contructs the container with each element initialized to `value`. This constructor effectively exists only if the underlying container is `std::array`, that is, if a size was passed as a second template parameter (such as `MathLbr::vector<int, 4>`).
The default behavior is that there must exist an implicit conversion sequence from T2 to T, where T is the type explicitly passed to the class template. If no such implicit conversion exists (that is, std::convertible_to<T2, T> fails) then a compilation error is thrown.
By default behavior, this constructor is implicitly marked explicit as well.
If ALLOW_IMPLICIT_CONVERSIONS is defined, then all narrowing conversions from T2 to T are disallowed, and this constructor is not marked explicit.
3) Constructs the container with `count` default-inserted instances of T (internally, calling `std::vector<T> myVector(count)`. This constructor effectively exists only if the underlying container is `std::vector`, that is, if no size was passed as a second template parameter (such as `MathLbr::vector<double> myVector`).
This constructor is explicit by default. If ALLOW_IMPLICIT_CONVERSIONS is defined then this constructor is not marked explicit.
4) Constructs the vector with the contents of the range [first, last].
   The default behavior is that there must be an implicit sequence from the internal type of the passed containers to T (where T is the type explicitly passed to the class template), and the constructor is marked explicit. If ALLOW_IMPLICIT_CONVERSIONS is defined then all narrowing conversions from the internal type of the passed containers to T are disallowed, and this constructor is not marked explicit. 
   For a MathLbr::vector whose size argument was passed to the class explicitly (such as MathLbr::vector<T, 3>):
   - an assert is performed to ensure that the size of the passed container ("OtherSize") is smaller than the size explicitly provided to MathLbr::vector, and 
   - If the size explicitly provided to MathLbr::vector is greater than "OtherSize" (that is, Size > OtherSize) then all remaining elements of MathLbr::vector are left uninitialized.
5) Constructs the vector with the contents of the passed array, similar to using a `std::initializer_list`. A pair of braces is necessary for this overload to be chosen, such as `MathLbr::vector<int> myVector{{1, 2, 3}}`. If no template size is passed, that is, if the underlying container is `std::array`, passing more values than what the vector can store (that is, total values passed > vector's actual Size) will result in a compile error.<br>
If the size of the passed array is smaller than the container's size, and if the underlying container used is `std::array`, the remaining elements are value-initialized such as `T{}`.
6) Constructs the vector with random real and imaginary values, where the values are in the range [lower, higher]. This constructor effectively exists only if `std::complex` is used as the type of the elements.<br>
   *Note*: The third parameter `count` is needed when initializing a vector whose underlying container is `std::vector`. This parameter is used to initialize the `std::vector` with `count` default values of the used type `T`: if no argument is passed, the default size of `std::vector` will be 1. If a `std::array` is used as the underlying container, this parameter is effectively ignored.
7) Same as 6), but this constructor effectively exists only if `std::complex` was not used as the type of the elements.
8) Constructs the vector with the contents of `other`. The type of the elements of `other` must be convertible to the type of the current vector `T`.<br>
   For vectors whose underlying container is `std::array`, an assert is performed to ensure that the size of `other` is smaller (or equal) to the size of the current vector. For vectors whose underlying container is `std::vector`, the current instance gets the same size as `other`. <br>
   If the underlying container is `std::array` and `other.size() < size()`, the remaining elements of the current instance are value-initialized such as `T{}`.
   Asserts might be disabled, see <a href="https://en.cppreference.com/w/cpp/error/assert">assert</a>.
9) Same as 9), but instead uses `std::move`. This constructor is currently useless, but it might become relevant if new types will be accepted in the future.
10) Copy constructor, move constructor, copy assignment and move assignment operators are all defaulted. These will be called *only* if the size and underlying type match.

## Examples
```cpp
#include "vector.h"
#include <complex>

void foo(const MathLbr::vector<int, 2>& v) {
	for (auto i : v) std::cout << i << ' ';
}

int main() {
	// Constructor (1)
	MathLbr::vector<int, 6> a1; // Empty vector of explicit size 6 whose underlying type is std::array
	MathLbr::vector<int> a11; // Empty vector whose underlying type is std::vector, since no explicit size was passed as the second template argument.


	// Constructor (2)
	MathLbr::vector<int, 2> v1(5); // All the elements are initialized to 5. 

	MathLbr::vector<int, 2> v2(5.5); // The default behavior is that 5.5 is implicitly converted to an int, and all elements are initialized to 5.
	// If ALLOW_IMPLICIT_CONVERSIONS is defined, then this fails to compile, because narrowing conversions are disallowed.

	foo(5); // The default behavior disallows this since there constructor is marked explicit.
	// If ALLOW_IMPLICIT_CONVERSIONS is defined, then this will compile, and a vector MathLbr::vector<int, 2> with all elements
	// initialized to 5 will be passed to "foo".
	
	
	// Constructor (3)
	MathLbr::vector<double> a22(5); // Initializes the internal std::vector with 5 default values. The size of the container is 5.


	// Constructor (4)
	std::vector<int> vector{ 1, 2, 3, 4, 5 }; // OtherSize = 5
	MathLbr::vector<int> v(vector.begin(), vector.end()); // ok, v.size() = 5, elements = 1,2,3,4,5
	MathLbr::vector<int, 7> v1(vector.begin(), vector.end()); // ok, v1.size() = 7, elements = 1,2,3,4,5,uninitialized,uninitialized
	MathLbr::vector<int, 3> v2(vector.begin(), vector.end()); // assert fail: CurrentSize < OtherSize

	std::vector<double> vector2{ {1., 2., 3.} }; // note: vector of doubles
	MathLbr::vector<int> v3(vector2.begin(), vector2.end()); // narrowing conversion allowed only if ALLOW_IMPLICIT_CONVERSIONS defined,
	// otherwise this results in a compile time error

	// Constructor (5)
	// Initializes a vector of complex numbers, the underlying container used is std::array.
	MathLbr::vector<std::complex<double>, 3> a4{ { {1,3}, {2,4} } };
	MathLbr::vector<short> a44{ { 2, 3, 4, 5, 6} }; // Initializes a vector of 5 shorts, the underlying container is std::vector.

	// Constructor (6)
	// Initializes the vector of complex numbers with random real and imaginary parts in the range [2, 9].
	// Uses std::vector as underlying container. The vector's size is 6 (the last parameter).
	MathLbr::vector<std::complex<int>> a5(2, 9, 6);
	MathLbr::vector<std::complex<double>, 2> a55(2, 9); // Same as above, but uses std::array. The last parameter is ignored.

	// Constructor (7): Same as (6), but with a type that is not std::complex.

	// Constructor (8)
	MathLbr::vector<int> a6{ {1,2,3} };
	MathLbr::vector<int, 3> a7(a6); // Initializes a7 with the contents of a6.

	// Constructor (9): same as (8), but by using std::move.
}

```
