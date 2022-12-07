#ifndef UTILITY_HEADER
#define UTILITY_HEADER

#include <complex>
#include <type_traits>

#define MATHLBR_ASSERT_SIZE_MISMATCH(Size, Size2)\
if constexpr (!Concepts::dynamic_extent_enabled<Size2, MathLbr::dynamic_extent>) {\
	assert((Size2 >= Size and "Size mismatch!"));\
}\

#define ASSERT_DIV_BYZERO(Value) assert((Value != value_type{} and "Cannot perform division by zero!"));

#define STATICASSERT_COMPLEX_DIFFTYPES(Type1, Type2)\
if constexpr (Concepts::is_complex<T>::value)\
static_assert(std::same_as<Type1, Type2>);\

#define MATHLBR_VECTOR_INIT_IF_DYNAMIC(count)\
if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>)\
initialize_vector(count);\

#define VALUE_INITIALIZE_REMAINING_ELEMENTS(otherSize, currentSize)\
for (size_type i = otherSize; i < currentSize; ++i) {\
	_vector[i] = value_type{};\
}\


namespace Concepts {

	template<typename T> struct is_complex : std::false_type {};
	template<typename T> struct is_complex<std::complex<T>> : std::true_type {
		using value_type = T;
	};

	template<typename T>
	concept underlying_vector_type =
		(is_complex<T>::value or std::is_arithmetic_v<T>)
		and not std::is_same_v<T, char>
		and not std::is_const_v<T>;

	template<typename T>
	concept arithmetic_char_const_excluded =
		std::is_arithmetic_v<T>
		and not std::is_same_v<T, char>
		and not std::is_const_v<T>;

	template<std::size_t V1, std::size_t V2>
	concept dynamic_extent_enabled = V1 == V2;
}

namespace MathLbr {
	enum Cos : std::size_t { X_AXIS = 0, Y_AXIS, Z_AXIS };

	enum class ComplexInnerProduct { ANTILINEAR_FIRST_ARGUMENT, ANTILINEAR_SECOND_ARGUMENT };

	inline constexpr std::size_t dynamic_extent = -1;

	bool is_near_zero(double val, double epsilon) {
		return std::abs(val) >= 0 && std::abs(val) <= std::abs(epsilon);
	}

	template<typename T>
	constexpr bool is_near_zero_complex(const std::complex<T>& val, double epsilon) {
		return std::abs(val.real()) >= 0 && std::abs(val.real()) <= std::abs(epsilon)
		&& (std::abs(val.imag() >= 0 && std::abs(val.imag()) <= std::abs(epsilon)));
	}
}

#endif
