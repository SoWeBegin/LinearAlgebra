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
4) ```cpp
   constexpr void from_polar(T r, T radius);
   ```
5) ```cpp 
   constexpr void from_cylindrical(T magnitude, T radius, T z);
   ```
6) ```cpp
   constexpr void from_spherical(T alpha, T beta, T theta);
   ```
<br>

1) Converts the current vector's elements coordinates to polar coordinates, and returns the new coordinates in either a `std::vector` or `std::array`, depending on which underlying container is currently used.<br>
An assert is performed to check whether the current vector's size equals 2.
2) Same as 1, but converts the cartesian coordinates to spherical coordinates. The assert in this case checks whether the size equals 3.
Note: The angles are expressed in radiants.
3) Same as 1, but converts the cartesian coordinates to cylindrical coordinates. The assert in this case checks whether the size equals 3.
Note: The angles are expressed in radiants.
4) Converts the given polar coordinates to cartesian coordinates, assigning the results to the current vector (`this`). The radius and other angles must be given in radiants.
5) Same as 4, but the given coordinates are in cylindrical form.
6) Same as 4, but the given coordinates are in spherical form.

In all cases, if the current underlying type is `std::array` (that is, the vector was created with an explicit template size, such as `MathLbr::vector<int, 3>`), then the returned container with the new coordinates will be `std::array`. The same then applies to `std::vector`.



## Example
```cpp

#include "vector.h"
#include <iostream>

int main() {
	// To polar coordinates
	MathLbr::vector<double> a{ {2, 3} };
	std::vector<double> aPolar = a.to_polar_coords();

	// To Spherical coordinates
	MathLbr::vector<double, 3> b{ {34, 4, 9} };
	std::array<double, 3> bSpherical = b.to_spherical_coords();
	std::cout << bSpherical[0];

	// To Cylindrical coordinates
	MathLbr::vector<double> c{ {32.2, 2.9, -5} };
	std::vector cCylindrical = c.to_cylindrical_coords();

	// Compile time error, underlying vector type is std::array, not std::vector
	MathLbr::vector<double, 3> d{ {3, 4, 5} };
	//std::vector<double> error = d.to_cylindrical_coords();

	// From polar
	MathLbr::vector<double, 2> a{};
	a.from_polar(5, 1.2);

	// From spherical
	MathLbr::vector<double> b(3);
	b.from_spherical(5, 2, 3);

	// From cylindrical
	MathLbr::vector<double> c(3);
	c.from_cylindrical(3, 2, 5.4);
}
```
