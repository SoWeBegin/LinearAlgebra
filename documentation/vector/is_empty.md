# is_empty()

```cpp
constexpr bool is_empty() const;
```

Checks whether the vector is empty, that is, if its size is 0 or all of its elements are zero (which means the vector is also null).

## Example
```cpp

#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<int> vec{ {5, 4, 3, -2, -1} };
	std::cout << vec.is_empty() << std::endl;
	MathLbr::vector<int> vec2(3); // default initializes 3 ints, such as int{}
	std::cout << vec2.is_empty();
}
```
