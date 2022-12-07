# Angle between two vectors

1. ```cpp
   constexpr double angle_between_radiants(const vector& other);
   ```
2. ```cpp
   constexpr double angle_between_degrees(const vector& other);
   ```
3. ```cpp
   template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT>
   constexpr double angle_between_radiants(const vector& other);
   ```
4. ```cpp
   template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT>
   constexpr double angle_between_degrees(const vector& other);
   ```

1) Calculates the angle between two non-`std::complex` vectors and returns the result. If the underlying container used to store the elements is `std::vector`, this function performs an assert to check whether both vector sizes are equal.
2) Same as 2, but returns the angle in degrees.
3) Same as 1, but works on complex vectors. The same assert is also performed.
4) Same as 3, but returns the angle in degrees.

## Example
```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<double> a{ {1.7, 26} };
	MathLbr::vector<double> bb{ {5.5, 6.6} };
	std::cout << a.angle_between_radiants(bb) << std::endl;

	MathLbr::vector<std::complex<double>> b{ {{2,-2}}};
	MathLbr::vector < std::complex<double>> c{ {{1, -sqrt(3)}}};
	std::cout << b.angle_between_degrees(c) << std::endl; 
}
```
