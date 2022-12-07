# Particular Definitions, Enums

```cpp
enum Cos : std::size_t { X_AXIS = 0, Y_AXIS, Z_AXIS };
```
Represents the axis related to a direction cosine. (For example, if you want the direction cosine in respect to the X axis, you'll make a call such as `someVector.direction_cosine(MathLibrary::Cos::X_AXIS)`.

<br>

```cpp
enum class ComplexInnerProduct { ANTILINEAR_FIRST_ARGUMENT, ANTILINEAR_SECOND_ARGUMENT };
```
This enum class is used for the complex dot product. When you call a function on a `complex` vector, in most cases you'll be required to pass a template argument `ComplexInnerProduct`.
Otherwise, the default member used will be `ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT`.
Commonly, a complex dot product might have two definitions. For each of these definitions, you can pass a `ComplexInnerProduct` template argument to choose which one you'd like to use.
- $a\cdot b=\sum_i \overline {a_i} b_i$ -> Template argument to pass is `MathLbr::ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT`
- $a\cdot b=\sum_i a_i \overline {b_i}$ -> Template argument to pass is `MathLbr::ComplexInnerProduct::ANTILINEAR_SECOND_ARGUMENT`

For example:
```cpp
int main() {
  MathLbr::vector<std::complex<double>> vec{{{2,4},{3,3}}};
  MathLbr::vector<std::complex<double>> sameVec{{{2,4},{3,3}}};
  MathLbr::Vector::inner_product<MathLbr::ComplexInnerProduct::ANTILINEAR_SECOND_ARGUMENT>(vec, sameVec);
```
