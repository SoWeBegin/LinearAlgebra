# Modifiers: normalize

1. ```cpp
   constexpr vector& normalize();
   ```

 Normalizes the vector (after this function, the vector is an unit vector). This function works for all underlying types, `std::complex` included.<br> An assert is performed to ensure that the denominator (where denominator is a call to `magnitude()`) is different than `value_type{}`.
 Asserts can be disabled. If that's the case and the denominator equals `value_type{}`, undefined behavior occurs.

<hr>

## Examples

```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<std::complex<double>> a{ {{1,7},{2 - 6}} };
	a.normalize();
	std::cout << a << std::endl;

	MathLbr::vector<double> b{ { 5,4 , 1.4, 11.4 } };
	b.normalize();
	std::cout << b << std::endl;
}
```
