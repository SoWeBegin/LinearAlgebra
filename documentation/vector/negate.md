# Operator overloads
## Negation
```cpp
constexpr vector operator-();
```
This operator negates all the elements of the current vector, returning the result in a new vector.
The underlying type `T` must not be unsigned, otherwise this function doesn't exist.

## Example
```cpp

#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<int> vec{ {5, 4, 3, -2, -1} };
	vec = -vec;
	for (auto current : vec) std::cout << current << ' ';
}
```
