## Element Access
The following functions can be used to access an element of the container.

1) ```cpp
   constexpr const T& operator[] (size_type index) const noexcept;
   ```
2) ```cpp
   constexpr T& operator[] (size_type index) noexcept;
   ```
3) ```cpp
   constexpr const T& get_x() const noexcept;
   ```
4) ```cpp
   constexpr const T& get_y() const noexcept;
   ```
5) ```cpp
   constexpr const T& get_z() const noexcept;
   ```
6) ```cpp
   constexpr const T& get_w() const noexcept;
   ```
<hr><br>

1) Access the element at the given `index`. No bound testing is performed, but the underlying container might assert.<br>
2) Same as 2), but can be used with non-`const` objects.<br>
3) Access the first element. Equivalent of doing `myVector[0]` on a `const` vector. No bound testing is performed, but the underlying container might assert.<br>
4) Same as 3), but accesses the second element. Equivalent of doing `myVector[1]`.<br>
5) Same as 3), but accesses the third element. Equivalent of doing `myVector[2]`.<br>
6) Same as 3), but accesses the fourth element. Equivalent of doing `myVector[3]`.
<br>
In all cases, no bound checks are performed at all, but the underlying container might assert. To disable assertions, see <a href="https://en.cppreference.com/w/cpp/error/assert">assert</a>.
<br>

## Examples

```cpp
#include "vector.h"
#include <iostream>

int main() {
	MathLbr::vector<int> myVector{ {1,2,3,4,5,6,7} };
	std::cout << "x: " << myVector.get_x() << std::endl;
	std::cout << "Element 2: " << myVector[1];
	myVector[10]; // assert might be performed by underlying container
}
```
