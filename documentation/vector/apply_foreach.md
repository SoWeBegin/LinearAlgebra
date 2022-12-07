# apply_foreach
```cpp
template<typename Function>
constexpr vector apply_foreach(const Function& function) noexcept(noexcept(function));
```

Calls the given function on each element of the vector.  There are no particular constraints.

## Example
```cpp

#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<int> vec{ {5, 4, 3, -2, -1} };
	vec.apply_foreach([](int& curr) {curr += 2; });

	for (auto i : vec) std::cout << i << ' ';
}
```
