# Size
```cpp
constexpr size_type size() const noexcept;
```

Returns the size of the container. For an underlying `std::array`, this effectively just returns the template parameter `Size`.
For an underlying `std::vector`, it instead calls `std::vector::size()`.
