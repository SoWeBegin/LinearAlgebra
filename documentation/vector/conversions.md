# Conversions
The following function allow to convert from cartesian coordinates to other coordinate systems.

1) ```cpp
   constexpr underlying_container to_polar_coords();
   ```
2) ```cpp
   constexpr underlying_container to_spherical_coords();
   ```
3) ```cpp
   constexpr underlying_container to_cylindrical_coords();
   ```
<br>

1) Converts the current vector's elements coordinates to polar coordinates, and returns the new coordinates in either a `std::vector` or `std::array`, depending on which underlying container is currently used.<br>
An assert is performed to check whether the current vector's size equals 2.
2) Same as 1, but converts the cartesian coordinates to spherical coordinates. The assert in this case checks whether the size equals 3.
3) Same as 1, but converts the cartesian coordinates to cylindrical coordinates. The assert in this case checks whether the size equals 3.

In all cases, if the current underlying type is `std::array` (that is, the vector was created with an explicit template size, such as `MathLbr::vector<int, 3>`), then the returned container with the new coordinates will be `std::array`. The same then applies to `std::vector`.


## Example
```cpp
#include "vector.h"
#include <iostream>

int main() {
	// Polar coordinates
	MathLbr::vector<double> a{ {2, 3} };
	std::vector<double> aPolar = a.to_polar_coords();

	// Spherical coordinates
	MathLbr::vector<double, 3> b{ {34, 4, 9} };
	std::array<double, 3> bSpherical = b.to_spherical_coords();
	std::cout << bSpherical[0];

	// Cylindrical coordinates
	MathLbr::vector<double> c{ {32.2, 2.9, -5} };
	std::vector cCylindrical = c.to_cylindrical_coords();

	// Compile time error, underlying vector type is std::array, not std::vector
	MathLbr::vector<double, 3> d{ {3, 4, 5} };
	//std::vector<double> error = d.to_cylindrical_coords();
}
```
