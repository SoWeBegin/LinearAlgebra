#ifndef UTILITY_HEADER
#define UTILITY_HEADER

#include <complex>
#include <type_traits>
#include <cassert>
#include <random>
#include <concepts>


namespace MathLbr
{
	inline constexpr std::size_t dynamic_extent = -1;

	enum class Cos : std::size_t
	{
		X_AXIS = 0, Y_AXIS, Z_AXIS
	};

	enum class ComplexInnerProduct
	{
		ANTILINEAR_FIRST_ARGUMENT, ANTILINEAR_SECOND_ARGUMENT
	};

	enum class Norm
	{
		L0, L1, L2, LINF
	};
}


namespace concepts 
{
#ifdef ALLOW_IMPLICIT_CONVERSIONS
	struct allow_implicit_conversions : std::true_type {};
#else
	struct allow_implicit_conversions : std::false_type {};
#endif

	template <typename T>
	using Arr = T[1];
	template<typename From, typename To>
	concept is_narrowing_conversion = not requires { Arr<To>{ std::declval<From>() }; };

	// The following attempt doesn't work due to corner cases such as aggregate initialization having precedente in some scenarios,
	// see: https://discord.com/channels/331718482485837825/1170461462083420211
	/*
	template<typename From, typename To>
	concept is_narrowing_conversion = not requires{ To{ std::declval<From>() }; };
	*/

	template<typename T> 
	struct is_complex : std::false_type {};

	template<typename T> 
	struct is_complex<std::complex<T>> : std::true_type
	{
		using value_type = T;
	};

	template<typename T>
	struct is_not_complex : std::bool_constant<not is_complex<T>::value> {};

	template<typename... Ts>
	concept is_not_all_complex = std::conjunction_v<is_not_complex<Ts>...>;

	template<typename... Ts>
	concept is_all_complex = std::conjunction_v<is_complex<Ts>...>;

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

	template<std::size_t V1, std::size_t V2 = MathLbr::dynamic_extent>
	concept dynamic_extent_enabled = V1 == V2;

	template<std::size_t V1, std::size_t V2 = MathLbr::dynamic_extent>
	concept dynamic_extent_disabled = V1 != V2;

	template<std::size_t V1, std::size_t V2>
	concept are_sizes_compiletime = V1 != MathLbr::dynamic_extent
		and V2 != MathLbr::dynamic_extent;

	template<typename T, typename U>
	concept convertible_to_or_not_narrowing_conversion = (concepts::allow_implicit_conversions::value
		? std::convertible_to<T, U>
		: not is_narrowing_conversion<T, U>);

	template<typename T, typename T2>
	concept convertible_to_or_not_narrowing_complex_product = concepts::convertible_to_or_not_narrowing_conversion<decltype(
		std::declval<T>() * std::declval<T2>() - std::declval<T>() * std::declval<T2>()), T>;

	template<typename T, typename T2>
	concept convertible_to_or_not_narrowing_inner_product = concepts::convertible_to_or_not_narrowing_conversion<decltype(
		std::declval<T>()* std::declval<T2>() + std::declval<T>() * std::declval<T2>()), T>;

	template<typename T>
	using dispatched_underlying_type = typename std::conditional<concepts::is_complex<T>::value,
		typename concepts::is_complex<T>::value_type, T>::type;
}


namespace detail
{
	bool is_near_zero(double val, double epsilon)
	{
		const auto abs_val = std::abs(val);
		return abs_val >= 0 && abs_val <= std::abs(epsilon);
	}

	template<typename T>
	constexpr bool is_near_zero_complex(const std::complex<T>& val, double epsilon)
	{
		const auto abs_epsilon = std::abs(epsilon);
		const auto abs_val_real = std::abs(val.real());
		const auto abs_val_imag = std::abs(val.imag());
		return abs_val_real >= 0 && abs_val_real <= abs_epsilon
			&& abs_val_imag >= 0 && abs_val_imag <= abs_epsilon;
	}

	template<std::size_t Size = -1, std::size_t OtherSize = -1>
	constexpr void dispatch_assert_size_mismatch(std::size_t NonTemplateSize, std::size_t NonTemplateOtherSize)
	{
		if constexpr (concepts::are_sizes_compiletime<Size, OtherSize>)
		{
			static_assert(Size >= OtherSize, 
				"Size mismatch: Size of LHS vector must be greater than size of RHS vector!");
		}
		else
		{
			assert(NonTemplateSize >= NonTemplateOtherSize and 
				"Size mismatch: Size of LHS vector must be greater than size of RHS vector!");
		}
	}

	template<std::size_t Size, std::size_t Value>
	constexpr void dispatch_assert_equals(std::size_t NonTemplateSize, std::size_t OtherNonTemplateSize)
	{
		if constexpr (concepts::dynamic_extent_disabled<Size>)
		{
			static_assert(Size == Value, 
				"Size of the vector must be of the correct dimension to perform specific operations!"
				"These operations include : Cross product(3D), Scalar Triple Product(3D)");
		}
		else
		{
			assert(NonTemplateSize == Value and OtherNonTemplateSize == Value
			and "Size of the vector must be of the correct dimension to perform specific operations!"
				 "These operations include : Cross product(3D), Scalar Triple Product(3D)");
		}
	}

	template<std::size_t Size, std::size_t Value>
	constexpr void dispatch_assert_equals(std::size_t NonTemplateSize)
	{
		if constexpr (concepts::dynamic_extent_disabled<Size>)
		{
			static_assert(Size == Value,
				"Size of the vector must be of the correct dimension to perform specific operations!" 
				"These operations include : Cross product(3D), Scalar Triple Product(3D), Coplanarity Checks(3D)");
		}
		else
		{
			assert(NonTemplateSize == Value and
				"Size of the vector must be of the correct dimension to perform specific operations!" 
				"These operations include : Cross product(3D), Scalar Triple Product(3D), Coplanarity Checks(3D)");
		}
	}

	template<typename Value, typename T2>
	constexpr void assert_div_byzero(T2 given_value)
	{
		assert((given_value != Value{} and "Cannot perform division by zero!"));
	}

	template<std::size_t Size, typename T, typename T2>
	constexpr void assert_same_value(T lhs, T2 rhs)
	{
		if constexpr (concepts::dynamic_extent_enabled<Size>)
		{
			assert(lhs == rhs && "Dynamic sizes of both lhs and rhs must be equal to perform certain operations!");
		}
	}
}

#endif
