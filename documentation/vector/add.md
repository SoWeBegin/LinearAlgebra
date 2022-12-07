## Adding elements
The following functions can be used *only* if the underlying container is `std::vector`, and are useful to add new elements even after the container has already been constructed.

1) ```cpp
   constexpr void add(value_type new_element);
   ```
2) ```cpp
   constexpr std::vector<T>& underlying_data();
   ```
<br>

1) Internally calls `vector::push_back(new_element)`, so the same rules apply.
2) This function can instead be used if other `std::vector`'s functions are needed.


## Examples
```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<double> a(1); // First element value constructed to 0
	a.add(5);
	a.underlying_data().push_back(4);
	for (auto i : a) std::cout << i << ' ';
}
```
