# Magnitude (norm)

```cpp
constexpr double magnitude() const ;
```

Calculates the magnitude (norm) of the vector, returning it as a double value.
This function works for any underlying type, `std::complex` included (in which case a different formula is used for the calculations).
Works with any dimension.

## Examples
```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<double> a{ {1,7, 26} };
	std::cout << a.magnitude() << std::endl;

	MathLbr::vector<std::complex<double>> b{ {{ 5,4 }, {1.4}, {11.4, 8} } };
	b.normalize();
	std::cout << b.magnitude() << std::endl; // Magnitude of an unit vector is always 1
}
```
