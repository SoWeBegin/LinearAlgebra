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
   template<size_type Sz, concepts::underlying_vector_type T2>
   constexpr explicit(not concepts::allow_implicit_conversions::value) vector(const T2(&arr)[Sz])
   ```
6. ```cpp
   template<concepts::underlying_vector_type T2>
   explicit(not concepts::allow_implicit_conversions::value) vector(T2 lower, T2 higher)   
   ```
7. ```cpp
   template<concepts::underlying_vector_type T2>
   explicit(not concepts::allow_implicit_conversions::value) vector(T2 lower, T2 higher, size_type count)  
   ```
8. ```cpp
   template<concepts::underlying_vector_type T2, size_type Size2>
   constexpr explicit(not concepts::allow_implicit_conversions::value) vector(const vector<T2, Size2>& other)
   ```
9. ```cpp
   template<concepts::underlying_vector_type T2, size_type Size2>
   constexpr explicit(not concepts::allow_implicit_conversions::value)
   vector(vector<T2, Size2>&& other) noexcept (std::is_nothrow_move_constructible_v<T2>)
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
The default behavior is that there must not be implicit narrowing conversions from T2 to T, where T is the type explicitly passed to the class template, otherwise a compilation error occurs.
By default behavior, this constructor is implicitly marked explicit as well.
If ALLOW_IMPLICIT_CONVERSIONS is defined, then all implicit conversions conversions from T2 to T are allowed, narrowing conversions included, and this constructor is not marked explicit.
3) Constructs the container with `count` default-inserted instances of T (internally, calling `std::vector<T> myVector(count)`. This constructor effectively exists only if the underlying container is `std::vector`, that is, if no size was passed as a second template parameter (such as `MathLbr::vector<double> myVector`).
This constructor is explicit by default. If ALLOW_IMPLICIT_CONVERSIONS is defined then this constructor is not marked explicit.
4) Constructs the vector with the contents of the range [first, last].
   The default behavior is that there must not be any implicit narrowing conversions from the internal type of the passed containers to T (where T is the type explicitly passed to the class template), and the constructor is marked explicit. If ALLOW_IMPLICIT_CONVERSIONS is defined then all implicit conversions, narrowing conversions included, are allowed from the internal type of the passed containers to T, and this constructor is not marked explicit. 
   For a MathLbr::vector whose size argument was passed to the class explicitly (such as MathLbr::vector<T, 3>):
   - an assert is performed to ensure that the size of the passed container ("OtherSize") is smaller than the size explicitly provided to MathLbr::vector, and 
   - If the size explicitly provided to MathLbr::vector is greater than "OtherSize" (that is, Size > OtherSize) then all remaining elements of MathLbr::vector are left uninitialized.
5) Constructs the vector with the contents of the passed array, similar to using a `std::initializer_list`. A pair of braces is necessary for this overload to be chosen, such as `MathLbr::vector<int> myVector{{1, 2, 3}}`. If no template size is passed, that is, if the underlying container is `std::array`, passing more values than what the vector can store (that is, total values passed > vector's actual Size) will result in a compile error.<br>
If the size of the passed array is smaller than the container's size, and if the underlying container used is `std::array`, the remaining elements are value-initialized such as `T{}`.
The default behavior of this constructor is that T2, the type of the arguments passed, must not have any implicit narrowing conversion from the passed argument's type T2 to the container's type T (std::convertible_to<T2, T> must be possible). Default behavior also means that this constructor is marked explicit.
If ALLOW_IMPLICIT_CONVERSIONS is defined, all implicit conversions are allowed from T2 to T (where T is the class template type), and this constructor is not marked explicit.
*Note*: There is a deduction guide for this constructor. See "deduction guides".
6) Constructs the vector with random real and imaginary values, where the values are in the range [lower, higher]. This constructor effectively exists only if the underlying container used is std::array, that is, if an explicit size was passed to the class MathLbr::vector during initialization.<br>
   The default behavior is that there must be no implicit narrowing conversions from T2 to T, where T is the type passed to MathLbr::vector, and the constructor is marked explicit. If ALLOW_IMPLICIT_CONVERSIONS is defined, then this constructor is not explicit, and all implicit conversions from T2 to T are allowed.
   For the underlying type std::complex, both real and imaginary parts will have random values in the range [lower, higher].
   *Important Note*: depending on the type of the passed argument, internally a real or integral distribution will be used. This means that even if MathLbr::vector has a real type, such as MathLbr::vector<double>, and we pass an int range, such as `MathLbr::vector<double, 3> v(1, 9);`, the distribution will be integral and thus the random values will result in `ints` (which are implicitly converted to `double` in this example). See the example code for this constructor!
7) Same as 6), but this constructor effectively exists only if `std::vector` is used as the underlying type, that is, no explicit size was passed to MathLbr::vector during initialization.
The only difference is that a third parameter must explicitly be passed, which is the size of the MathLbr::vector.
8) Constructs the vector with the contents of `other`. <br>
   For vectors whose underlying container is `std::array`, an assert or static_assert (depending on whether both Sizes are known at compile time) is performed to ensure that the size of `other` is smaller (or equal) to the size of the current vector. For vectors whose underlying container is `std::vector`, the current instance gets the same size as `other`. <br>
   If the underlying container is `std::array` and `other.size() < size()`, the remaining elements of the current instance are left uninitialized.
   Runtime asserts might be disabled, see <a href="https://en.cppreference.com/w/cpp/error/assert">assert</a>.<br>
The default behavior of this constructor is as follows:
	- There must not be an implicit narrowing conversions from the other vector's type T2 to the current vector's type T, otherwise compilation fails.
	- This constructor is explicit.
If ALLOW_IMPLICIT_CONVERSION is defined, then any implicit conversion from T2 to T is allowed and this constructor is not marked explicit.
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

	MathLbr::vector<int, 2> v2(5.5); // The default behavior is that this line fails to compile, because narrowing conversions are disallowed.
	// If ALLOW_IMPLICIT_CONVERSIONS is defined, then this will compile correctly: the double 5.5 will be implicitly converted to the int 5.

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
	MathLbr::vector<std::complex<int>> v{ { {3, 3}, {3, 3} } };
	MathLbr::vector<int> v2{ {3.4, 3.4} }; // only works if ALLOW_IMPLICIT_CONVERSIONS is defined (otherwise, narrowing conversions are disallowed)
	MathLbr::vector v3{ {3.4, 3.2, 2.3} }; // Deduction guide used, the vector internal type is deduced as double, size is 3
        MathLbr::vector<int, 3> v4{ {1,2,3,4} }; // Error: The size passed is greater than 3

	// Constructor (6)
	// NOTE: even if the internal container type is double, this initialization will use an integral distribution to generate the random values. Therefore, an output could, for example, be: 3 5 9.
	// This only can happen if you define ALLOW_IMPLICIT_CONVERSIONS. Otherwise, the default behavior is that the following line won't compile because an implicit conversion (from int to double) is happening!
	MathLbr::vector<double, 3> random = { 1, 9 }; // Only works if ALLOW_IMPLICIT_CONVERSIONS is defined, because the constructor is not explicit in that case, and because narrowing conversions would be allowed.
	// generates 3 random int values between 1 and 9
	MathLbr::vector<int, 3> random(1.3, 3.4); // Only works if ALLOW_IMPLICIT_CONVERSIONS is defined. Generates 3 random int values between
	// 1 and 3
	MathLbr::vector<std::complex<double>, 3> random3(3.4, 9.5); // Generates 3 random complex numbers. Each imaginary and real part has a random value
	// that is between 3.4 and 9.5


	// Constructor (7): Same as (6), but with a type that is not std::complex.

	// Constructor (8)
	MathLbr::vector<int> a6{ {1,2,3} };
	MathLbr::vector<int, 3> a7(a6); // Initializes a7 with the contents of a6.

	// Constructor (9): same as (8), but by using std::move.
}

```
