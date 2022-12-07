# Iterators

```cpp
constexpr auto begin() const noexcept;
constexpr auto end() const noexcept;
constexpr auto begin() noexcept;
constexpr auto end() noexcept;
```

These functions allow you to use range based for loop with a `MathLbr::vector`. Internally, they effectively call `std::begin` and `std::end`.

## Example
```cpp

#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<int> vec{ {5, 4, 3, -2, -1} };
	for (int i : vec) { /* ... */ }
	std::cout << *(vec.begin());
}
```
