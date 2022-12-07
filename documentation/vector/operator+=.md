# Overloaded operators

## operators+ [sum]
1) ```cpp
   template<typename T2, size_type Size2>
   constexpr vector& operator+=(const vector& rhs);
   ```
2) ```cpp
   template<Concepts::underlying_vector_type T2>
   constexpr vector& operator+=(T2 val);
   ```
3) ```cpp
   template<Concepts::underlying_vector_type T2>
   friend constexpr vector operator+(vector lhs, T2 val);
   ```
4) ```cpp
   template<Concepts::underlying_vector_type T2>
   friend constexpr vector operator+(T2 val, vector lhs);
   ```
5) ```cpp
   template<typename T2, size_type Size2>
   friend constexpr vector operator+(vector lhs, const vector& rhs)
   ```
<br>

1) Performs an element-wise addition between two vectors and stores the result in the current instance `this`. If the sizes differ, then the smallest size is chosen and the addition is
   performed only on the elements in the range of that size.
   The underlying element's type of `rhs` must be convertible to the current underlying type `value_type`, otherwise this overload does not exist.
   For complex values, the underlying types must be the same when calling this overload, otherwise compilation fails.
2) Sums the value `val` to each single element and stores the result in the current instance `this`.
3) Same as 2), but this overload does not store the result in the current instance `this`, it instead returns a new vector with the stored results.
4) Same as 3), but with reversed order.
5) Same as 1, but stores the result in a new vector and returns it. Effectively calls `operator+=` internally.
   The resulting vector has the same size (and type) as the first parameter `lhs`.

   
   
