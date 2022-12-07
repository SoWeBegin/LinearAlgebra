# operator<<
Prints the contents of the vector.
```cpp
friend std::ostream& operator<< (std::ostream& stream, const vector& rhs);
```

## Example
```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector a{ {2,3,4} };

	std::cout << a;
}
```
