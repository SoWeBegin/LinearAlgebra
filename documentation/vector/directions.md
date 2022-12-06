# Directions
The following functions can be used to retrieve the direction of a given vector.

1) ```cpp
   constexpr double direction_radiants_y() const;
   ```
2) ```cpp
   constexpr double direction_radiants_x() const;
   ```
3) ```cpp
   constexpr double direction_degrees_y() const;
   ```
4) ```cpp
   constexpr double direction_degrees_x() const
   ```
5) ```cpp
   constexpr double direction_cosine(Cos type) const;
   ```
6) ```cpp
   constexpr double direction_angle(Cos type) const;
   ```

<br>

1) Calculates the direction of a 2D vector (in radiants) in respect to the y axis.
2) Same as 1), but the direction is in respect to the x axis.
3) Calculates the direction of a 2D vector in degrees, in respect to the y axis.
4) Same as 3), but the direction is in respect to the x axis.
5) Calculates the direction cosine of a 3D vector in respect to a given axis `type`, which includes: `MathLbr::Cos::X_AXIS`, `MathLbr::Cos::Y_AXIS`, `MathLbr::Cos::Z_AXIS`.
6) Same as 5, but returns performs `std::acos` on the result of 5).

In all the cases, these functions only work for non-`std::complex` types. An assert is performed for each function to check whether the sizes are correct. If asserts are disabled, then the functions will proceed with the calculations resulting in undefined behavior.

<hr>

## Examples

```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<int> a{ {2,3} };
	MathLbr::vector<double> b{ {5.5, 3.4, 2.0} };
	
	std::cout << a.direction_degrees_x();
	a.direction_radiants_y();
	b.direction_cosine(MathLbr::Cos::Y_AXIS);
	// same as:
	b.direction_cosine(MathLbr::Y_AXIS);
}
```
