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
