# Similarity checks: Parallelism, Orthogonality, Coplanarity
The following functions take vector arguments and return a boolean result.

1) ```cpp
   template<typename T, std::size_t Size>
   constexpr bool are_parallel(const vector<T, Size>& first, const vector<T, Size>& other, double epsilon = 1E-6);
   ```
2) ```cpp
   template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT, typename T, std::size_t Size>
   constexpr bool are_parallel(const vector<T, Size>& first, const vector<T, Size>& other, double epsilon = 1E-6);
   ```
3) ```cpp
   template<typename T, std::size_t Size>
   constexpr bool are_perpendicular(const vector<T, Size>& first, const vector<T, Size>& other, double epsilon = 1E-6);
   ```
4) ```cpp
   template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT, typename T, std::size_t Size>
   constexpr bool are_perpendicular(const vector<T, Size>& first, const vector<T, Size>& other, double epsilon = 1E-6);
   ```
5) ```cpp
   template<typename T, std::size_t Size>
   constexpr bool are_coplanar(const vector<T, Size>& first, const vector<T, Size>& other2, const vector<T, Size>& other3, double epsilon = 1E-6);
   ```
6) ```cpp
   template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT, typename T, std::size_t Size>
   constexpr bool are_coplanar(const vector<T, Size>& first, const vector < T, Size>& other2, const vector<T, Size> other3, double epsilon = 1E-6);
   ```
<br>

1) Checks if two given vectors are parallel. An assert is performed to ensure that both vector sizes are equal, in case the underlying container is `std::vector`. The parameter epsilon is needed for the accuracy of the result: the smaller this parameter is, the more reliable the result of `are_parallel` will be. It is needed because in most situations, the result of the calculations will lose some precision.
2) Same as 1), but performs the parallelism check on complex vectors.
3) Checks if two given vector are perpendicular (orthogonal). An assert is performed to ensure that both vector sizes are equal, in case the underlying container is `std::vector`. For the parameter epsilon, refer to 1).
4) Same as 3), but works on complex vectors.
5) Checks whether 3 given vectors are coplanar, that is, they lie in the same plane. All the sizes must be equal, and an additional assert is performed to ensure that when the underlying container is `std::vector`. Effectively uses `triple_scalar_product()` to check whether the result is 0, in which case the 3 vectors are coplanar. The parameter epsilon can be used to counter precision loss, see 1).
6) Same as 5, but works on complex vectors.


## Example
```cpp

#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<double> first{ {1,1,1} };
	MathLbr::vector<double> second{ {1,3,1} };
	MathLbr::vector<double> third{ {2,2,2} };
	std::cout << MathLbr::Vector::are_coplanar(first, second, third) << std::endl;

	MathLbr::vector<double, 3> aa{ {3.3, 4.4, 5.5} };
	MathLbr::vector<double, 3> bb{ {6.6, 8.8, 11} };
	std::cout << MathLbr::Vector::are_parallel(aa, bb) << std::endl;

	MathLbr::vector<std::complex<double>> c{ { {0,1},{1,1},{1,0} } };
	MathLbr::vector<std::complex<double>> d{ {{0,0},{1 / sqrt(3),0},{-1 / sqrt(3),1 / sqrt(3)}} };
	std::cout << MathLbr::Vector::are_perpendicular(d, c);
}
```
