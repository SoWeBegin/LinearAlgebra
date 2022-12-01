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
  
1) **Default constructor**. Constructs an empty vector. The underlying container used to store elements depends on whether an explicit size (as a second template argument) was passed.
2) Contructs the container with each element initialized to `value`. This constructor effectively exists only if the underlying container is `std::array`, that is, if a size was passed as a second template parameter (such as `MathLbr::vector<int, 4>`).
3) Constructs the container with `count` default-inserted instances of T (internally, calling `std::vector<T> myVector(count)`. This constructor effectively exists only if the underlying container is `std::vector`, that is, if no size was passed as a second template parameter (such as `MathLbr::vector<double> myVector`).
4) Constructs the vector with the contents of the range [first, last].
   <br> The type of the range must be convertible to the container's type (for example, a range of `std::complex` types cannot be used to initialize a vector of `int`s).
   <br>*Note:* For a non-dynamically allocated vector (that is, a vector created with an explicitly passed template size), an assert is performed to ensure that the container's   size is greater or equal than the size of the passed range (Size >= range size). Asserts might be disabled, refer to <a href="https://en.cppreference.com/w/cpp/error/assert">assert</a>.

