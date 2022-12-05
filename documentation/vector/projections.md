# Projection

1) ```cpp
   template<typename T2>
   constexpr vector& vector_projection_from(const vector<T2, Size>& other);
   ```
2) ```cpp
   template<typename T, std::size_t Size>
   constexpr vector<T, Size> projection(vector<T, Size> first, const vector<T, Size>& other);
   ```
  
<br>

1) Projects `other` on the current instance `this`, storing the result in `this`. An assert is performed to ensure that both sizes are equal. A further assert is performed to ensure that inner_product() does not equal 0 (since it is used as a denominator to calculate the projection).
If asserts are disabled and the sizes are not equal, the function still attempts to calculate the projection, which results in undefined behavior.
As it currently stands, this function is only defined for non-complex vectors. That is, using `std::complex` as the underlying vector's type will disallow it.
2) Same as 1), but projects `first` on `other`, resulting in a new vector with the same type `T` and `Size`. All the other rules explained in point 1) stand: internally, this functions calls 1).

 
 ## Examples
