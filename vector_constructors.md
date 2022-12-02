# MathLbr::vector::vector
1. ```cpp
   constexpr vector() = default;
   ```
2. ```cpp
   constexpr explicit vector(value_type value);
   ```
3. ```cpp
   constexpr explicit vector(size_type count);
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
  
1) **Default constructor**. Constructs an empty vector. The underlying container used to store elements depends on whether an explicit size (as a second template argument) was passed. If the underlying container is `std::vector`, this constructor does not provide any size to it, and pushing elements in the vector through specific member functions will be required.
2) Contructs the container with each element initialized to `value`. This constructor effectively exists only if the underlying container is `std::array`, that is, if a size was passed as a second template parameter (such as `MathLbr::vector<int, 4>`).
3) Constructs the container with `count` default-inserted instances of T (internally, calling `std::vector<T> myVector(count)`. This constructor effectively exists only if the underlying container is `std::vector`, that is, if no size was passed as a second template parameter (such as `MathLbr::vector<double> myVector`).
4) Constructs the vector with the contents of the range [first, last].
   <br> The type of the range must be convertible to the container's type (for example, a range of `std::complex` types cannot be used to initialize a vector of `int`s).
   <br>*Note:* For a non-dynamically allocated vector (that is, a vector created with an explicitly passed template size), an assert is performed to ensure that the container's   size is greater or equal than the size of the passed range (Size >= range size). Asserts might be disabled, refer to <a href="https://en.cppreference.com/w/cpp/error/assert">assert</a>.
   If the total values passed are smaller than the container's size, and the underlying container is `std::array`, then the remaining elements are value-initialized such as `T{}`.
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

int main() {
	// Constructor (1)
	MathLbr::vector<int, 6> a1; // Empty vector of explicit size 6 whose underlying type is std::array
	MathLbr::vector<int> a11; // Empty vector whose underlying type is std::vector, since no explicit size was passed as the second template argument.

	// Constructor (2)
	MathLbr::vector<int, 3> a2(4); // Initializes the internal std::array with 3 values equal to 4.
	
	// Constructor (3)
	MathLbr::vector<double> a22(5); // Initializes the internal std::vector with 5 default values. The size of the container is 5.

	// Constructor (4)
	std::vector<int> values{ 1,2,3,4,5,6 };
	MathLbr::vector<int, 7> a3(values.begin(), values.end()); // Initializes a3 with the contents of values. Last element is default-initialized.
	//MathLbr::vector<int, 3> a33(values.begin(), values.end()); // Produces an assert: 3 is smaller than 6!
	MathLbr::vector<int> a333(values.begin(), values.end()); // Initializes the vector with the contents of value, std::vector is used as internal container.

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
