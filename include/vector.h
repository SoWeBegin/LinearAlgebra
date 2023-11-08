#ifndef VECTOR_MATH_MINILIBRARY
#define VECTOR_MATH_MINILIBRARY

#include "utility.h"

#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <concepts>
#include <initializer_list> 
#include <iterator>
#include <numeric>
#include <numbers>
#include <optional>
#include <random>
#include <type_traits>
#include <utility>



#if (defined(_MSVC_LANG) && _MSVC_LANG < 202002L)
#error "MathLbr: Only C++20 and newer language standards are supported."
#elif (!defined(_MSVC_LANG) && __cplusplus < 202002L)
#pragma error "MathLbr: Only C++20 and newer language standards are supported."
#endif

namespace MathLbr 
{
	template<concepts::underlying_vector_type T, std::size_t Size = dynamic_extent>
	class vector;

	namespace Vector 
	{
		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			concepts::underlying_vector_type T, concepts::underlying_vector_type T2, std::size_t Size>
		constexpr auto inner_product(const vector<T, Size>& lhs, const vector<T2, Size>& other)
		requires (concepts::is_all_complex<T, T2>)
		{
			// std::complex does not support std::complex<T> OP std::complex<T2>, so static_cast is needed
			detail::assert_same_value<Size>(lhs.size(), other.size());
			using resulting_type_mult = decltype(std::declval<concepts::dispatched_underlying_type<T>>() *
				std::declval<concepts::dispatched_underlying_type<T2>>());
			using resulting_type = std::complex<resulting_type_mult>;

			return std::transform_reduce(lhs.begin(), lhs.end(), other.begin(), resulting_type{},
				std::plus<resulting_type>{}, [](const auto& lhs_elem, const auto& other_elem)
				{
					if constexpr (definition_type == ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT)
					{
						return std::conj(static_cast<resulting_type>(lhs_elem)) * static_cast<resulting_type>(other_elem);
					}
					// Conjugate linear == anti linear 
					else if constexpr (definition_type == ComplexInnerProduct::ANTILINEAR_SECOND_ARGUMENT)
					{
						return static_cast<resulting_type>(lhs_elem) * std::conj(static_cast<resulting_type>(other_elem));
					}
				});
		}

		template<concepts::underlying_vector_type T, concepts::underlying_vector_type T2, std::size_t Size>
		constexpr auto inner_product(const vector<T, Size>& lhs, const vector<T2, Size>& other)
		requires (concepts::is_not_all_complex<T, T2>)
		{
			detail::assert_same_value<Size>(lhs.size(), other.size());
			return std::inner_product(lhs.begin(), lhs.end(),
				other.begin(), decltype(std::declval<T>() * std::declval<T2>()){});
		}

		template<concepts::underlying_vector_type T, concepts::underlying_vector_type T2, std::size_t Size1>
		constexpr auto cross_product(vector<T, Size1> lhs, const vector<T2, Size1>& rhs)
		{
			return lhs.cross_product(rhs);
		}

		template<concepts::underlying_vector_type T, concepts::underlying_vector_type T2>
		constexpr auto complex_inner_product(const std::complex<T>& lhs, const std::complex<T2>& rhs)
		-> decltype(std::declval<T>() * std::declval<T2>() + std::declval<T>() * std::declval<T2>()) 
		{
			return lhs.real() * rhs.real() + lhs.imag() * rhs.imag();
		}

		template<concepts::underlying_vector_type T, concepts::underlying_vector_type T2>
		constexpr auto complex_cross_product(const std::complex<T>& lhs, const std::complex<T2> rhs)
		-> decltype(std::declval<T>() * std::declval<T2>() - std::declval<T>() * std::declval<T2>()) 
		{
			return lhs.real() * rhs.imag() - lhs.imag() * rhs.real();
		}

		template <concepts::underlying_vector_type T, concepts::underlying_vector_type T2, 
			concepts::underlying_vector_type T3, std::size_t Size>
		constexpr auto scalar_triple_product(const vector<T, Size>& first, const vector<T2, Size>& other2, const vector<T3, Size>& other3)
		requires (concepts::is_not_all_complex<T, T2, T3> 
		and concepts::convertible_to_or_not_narrowing_complex_product<T, T2>)
		{
			// necessary assert (dispatch) performed by inner_product and cross_product internally
			return inner_product(other3, cross_product(first, other2));
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT, 
			concepts::underlying_vector_type T, concepts::underlying_vector_type T2, 
			concepts::underlying_vector_type T3, std::size_t Size>
		constexpr auto scalar_triple_product(const vector<T, Size>& first, const vector<T2, Size>& other2, const vector<T3, Size>& other3)
		requires (concepts::is_all_complex<T, T2, T3> and concepts::convertible_to_or_not_narrowing_complex_product<T, T2>)
		{
			// necessary assert (dispatch) performed by inner_product and cross_product internally
			return inner_product<definition_type>(other3, cross_product(first, other2));
		}

		template<concepts::underlying_vector_type T, concepts::underlying_vector_type T2,
			concepts::underlying_vector_type T3, std::size_t Size>
		constexpr auto vector_triple_product(const vector<T, Size>& first, const vector<T2, Size>& other2, const vector<T3, Size>& other3) 
		requires (concepts::is_not_all_complex<T, T2, T3>
		and concepts::convertible_to_or_not_narrowing_complex_product<T, T2>
		and concepts::convertible_to_or_not_narrowing_complex_product<T3, T>)
		{
			// necessary assert (dispatch) performed by inner_product and cross_product internally
			return cross_product(other3, cross_product(first, other2));
		}

		template<concepts::underlying_vector_type T, concepts::underlying_vector_type T2, std::size_t Size>
		constexpr bool are_parallel(const vector<T, Size>& first, const vector<T2, Size>& other, double epsilon = 1E-6)
		requires (concepts::is_not_all_complex<T, T2>)
		{
			detail::assert_same_value<Size>(other.size(), first.size());

			if (first.size() == 3) 
			{
				const auto temp = cross_product(first, other);
				const auto eps = std::abs(epsilon);
				// Two vectors are parallel in 3D only if their cross product result is near zero.
				return temp[0] >= 0 && temp[0] <= eps
					&& temp[1] >= 0 && temp[1] <= eps
					&& temp[2] >= 0 && temp[2] <= eps;
			}

			// Other dimensions: scalar product method. Parallelism condition is true if result's near 0 (approx 1E-6 by default)
			const auto sum_and_pow = [](auto sum, const auto& element) 
			{
				return sum + std::pow(element, 2);
			};

			const auto inner_prod = inner_product(first, other);
			const double rhs1 = std::accumulate(std::begin(first), std::end(first),
				decltype(inner_prod){}, sum_and_pow);
			const double rhs2 = std::accumulate(std::begin(other), std::end(other),
				decltype(inner_prod){}, sum_and_pow);
			return detail::is_near_zero(std::abs(rhs1 * rhs2 - std::pow(inner_prod, 2)), epsilon);
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			concepts::underlying_vector_type T, concepts::underlying_vector_type T2, std::size_t Size>
		constexpr bool are_parallel(const vector<T, Size>& first, const vector<T2, Size>& other, double epsilon = 1E-6)
		requires (concepts::is_all_complex<T, T2>)
		{
			// Beware: this calculation won't work correctly with integral types, because truncation happens inside normalize()
			// This is a non issue for std::complex, because int types aren't accepted in the first place
			// (This is also why we use a different formulae for the non-complex case, to avoid this issue with int types!)
			// Unfortunately, this formulae loses a bit of precision compared to the non-complex one...
			detail::assert_same_value<Size>(other.size(), first.size());

			const auto result = std::abs(inner_product<definition_type>(normalize(first), normalize(other)).real());
			return ((result >= 1 && result <= (1 + epsilon)) || (result <= 1 && result >= (1 - epsilon)));
		}

		template<concepts::underlying_vector_type T, concepts::underlying_vector_type T2,
			std::size_t Size>
		constexpr bool are_perpendicular(const vector<T, Size>& first, const vector<T2, Size>& other, double epsilon = 1E-6)
		requires (concepts::is_not_all_complex<T, T2>)
		{
			detail::assert_same_value<Size>(other.size(), first.size());
			return detail::is_near_zero(inner_product(first, other), epsilon);
		}

		// Should be OK, see https://www.sciencedirect.com/topics/mathematics/orthogonal-set-of-vector
		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			concepts::underlying_vector_type T, concepts::underlying_vector_type T2, std::size_t Size>
		constexpr bool are_perpendicular(const vector<T, Size>& first, const vector<T2, Size>& other, double epsilon = 1E-6)
		requires (concepts::is_all_complex<T, T2>)
		{
			detail::assert_same_value<Size>(other.size(), first.size());
			return detail::is_near_zero_complex(inner_product<definition_type>(first, other), epsilon);
		}

		template<concepts::underlying_vector_type T, concepts::underlying_vector_type T2,
			concepts::underlying_vector_type T3, std::size_t Size>
		constexpr bool are_coplanar(const vector<T, Size>& first, const vector<T2, Size>& other2,
			const vector<T3, Size>& other3, double epsilon = 1E-6)
		requires (concepts::is_not_all_complex<T, T2, T3> and concepts::convertible_to_or_not_narrowing_complex_product<T, T2>)
		{
			// necessary assert (dispatch) performed by inner_product and cross_product internally
			return detail::is_near_zero(scalar_triple_product(first, other2, other3), epsilon);
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			concepts::underlying_vector_type T, concepts::underlying_vector_type T2,
			concepts::underlying_vector_type T3, std::size_t Size>
		constexpr bool are_coplanar(const vector<T, Size>& first, const vector <T2, Size>& other2,
			const vector<T3, Size> other3, double epsilon = 1E-6)
		requires (concepts::is_all_complex<T, T2, T3> and concepts::convertible_to_or_not_narrowing_complex_product<T, T2>)
		{
			// necessary assert (dispatch) performed by inner_product and cross_product internally
			return detail::is_near_zero_complex(scalar_triple_product<definition_type>(first, other2, other3), epsilon);
		}


		template<concepts::underlying_vector_type T, concepts::underlying_vector_type T2, std::size_t Size>
		constexpr auto projection(vector<T, Size> to, const vector<T2, Size>& from)
		requires (concepts::is_not_all_complex<T, T2>) 
		{
			return to.vector_projection_from(from);
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			concepts::underlying_vector_type T, concepts::underlying_vector_type T2, std::size_t Size>
		constexpr auto projection(vector<T, Size> to, const vector<T2, Size>& from)
		requires (concepts::is_all_complex<T, T2>)
		{
			return to.vector_projection_from<definition_type>(from);
		}

		template<typename T, std::size_t Size>
		constexpr auto normalize(vector<T, Size> other)
		{
			return other.normalize();
		}
	}


	/*
		General vector expressed in cartesian coordinates - accepts the specified type and any size.
		Internally, std::vector is used if Size == -1. Otherwise, std::array is used.
		Most operations are defined for complex vectors (vectors with complex entries) as well,
		but some are explicitly disabled because they make little sense with complex entries.
	*/

	template<concepts::underlying_vector_type T, std::size_t Size>
	class vector
	{

	private:
		template<concepts::underlying_vector_type T2, std::size_t Size2>
		friend class vector;

		std::conditional_t<concepts::dynamic_extent_enabled<Size, dynamic_extent>,
			std::vector<T>, std::array<T, Size>> _vector;

	public:
		using size_type = std::size_t;
		using value_type = T;
		using pointer_type = T*;
		using reference_type = T&;
		using const_pointer_type = const T*;
		using const_reference_type = const T&;
		using underlying_container = std::conditional_t<concepts::dynamic_extent_enabled<Size>,
			std::vector<T>, std::array<T, Size>>;
		template<typename _T>
		using complex_internal_value_type = typename concepts::is_complex<_T>::value_type;


		constexpr vector() = default;

		// constructor [1]
		template<concepts::underlying_vector_type T2>
		constexpr explicit(not concepts::allow_implicit_conversions::value) vector(T2 value)
		requires (concepts::dynamic_extent_disabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>)
		{
			std::fill(std::begin(_vector), std::end(_vector), value);
		}

		// constructor [2]
		constexpr explicit(not concepts::allow_implicit_conversions::value) vector(size_type count)
		requires (concepts::dynamic_extent_enabled<Size>)
		: _vector(count)
		{
		}

		// constructors [3]
		template<std::input_iterator InputIter>
		constexpr vector(InputIter first, InputIter last)
		requires (concepts::dynamic_extent_enabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<typename std::iterator_traits<InputIter>::value_type, value_type>)
			: _vector(first, last)
		{
		}

		template<std::input_iterator InputIter>
		constexpr vector(InputIter first, InputIter last)
		requires (concepts::dynamic_extent_disabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<typename std::iterator_traits<InputIter>::value_type, value_type>)
		{
			const auto distance = std::distance(first, last);
			detail::dispatch_assert_size_mismatch(Size, distance);

			std::copy(first, last, std::begin(_vector));
			// remaining array elements uninitialized
		}

		// constructor [4]
		template<size_type Sz, concepts::underlying_vector_type T2>
		constexpr explicit(not concepts::allow_implicit_conversions::value) vector(const T2(&arr)[Sz])
		requires (Size >= Sz // note: always true for dynamic_extent
		and concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>)
			: vector(arr, std::make_index_sequence<Sz>{})
		{
		}

		// support stuff like: MathLbr::vector<std::complex<double>> v{ { {3, 3}, {3, 3} } };
		template<size_type Sz>
		constexpr explicit(not concepts::allow_implicit_conversions::value) vector(const value_type(&arr)[Sz])
		requires (Size >= Sz) : vector(arr, std::make_index_sequence<Sz>{})
		{
		}

	private:
		template <size_type... Index, concepts::underlying_vector_type T2>
		constexpr explicit vector(const T2(&arr)[sizeof...(Index)],
			std::index_sequence<Index...>)
			: _vector{ static_cast<value_type>(arr[Index])... }
			// Allow "implicit" conversions from the user, explicit cast needed for compilation unlike other constructors 
			// where only a simple warning is triggered (which is fine)
			// Note: remaining elements are implicitly zero-initialized for std::array [See: aggregate initialization for fundamental
			// types and class types]
			// for std::vector, there are no "remaining" elements because all elements in the initializer list are used
		{
		}

	public:
		/*
		* If the following constructors are not explicit (the user allows for implicit conversions),
		* then it is possible to pass a vector V1 of an arbitrary size to a function accepting a
		* cv-qualified vector V2 of a different size or type! [See cppinsight example]
		*/

		// Constructors [5]
		template<concepts::underlying_vector_type T2, size_type Size2>
		constexpr explicit(not concepts::allow_implicit_conversions::value) vector(const vector<T2, Size2>& other)
		requires (concepts::dynamic_extent_enabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>)
			: _vector{ other.begin(), other.end() }
		{
		}

		template<concepts::underlying_vector_type T2, size_type Size2>
		constexpr explicit(not concepts::allow_implicit_conversions::value) vector(const vector<T2, Size2>& other)
		requires (concepts::dynamic_extent_disabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>)
			// We could've dispatched to the constructor taking InputIterators, but that way we can not dispatch the assert/static_assert
		{
			detail::dispatch_assert_size_mismatch<Size, Size2>(Size, other.size());

			std::copy(std::begin(other._vector), std::end(other._vector),
				std::begin(_vector));

			// Remaining elements have uninitialized values.
		}

		// Mimics move constructor, but accepts different sizes
		// Only the elements themselves are moved, not the underlying vector or array.
		// Constructors [6]
		template<concepts::underlying_vector_type T2, size_type Size2>
		constexpr explicit(not concepts::allow_implicit_conversions::value)
		vector(vector<T2, Size2>&& other) noexcept (std::is_nothrow_move_constructible_v<T2>)
		requires (concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>
		and concepts::dynamic_extent_enabled<Size>)
			: _vector{ std::make_move_iterator(other.begin())
			, std::make_move_iterator(other.end()) }
		{
		}

		template<concepts::underlying_vector_type T2, size_type Size2>
		constexpr explicit(not concepts::allow_implicit_conversions::value)
		vector(vector<T2, Size2>&& other) noexcept (std::is_nothrow_move_constructible_v<T2>)
		requires (concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>
		and concepts::dynamic_extent_disabled<Size>)
		{
			detail::dispatch_assert_size_mismatch<Size, Size2>(Size, other.size());

			std::move(std::begin(other._vector), std::end(other._vector),
				std::begin(_vector));
			// Remaining elements remain uninitialized
		}

	private:
		template<concepts::underlying_vector_type Type>
		Type get_random(Type lower, Type higher)
		{
			static std::mt19937 mt(std::random_device{}());

			// Choose the appropriate distribution
			std::conditional_t<std::is_floating_point_v<Type>,
				std::uniform_real_distribution<Type>,
				std::uniform_int_distribution<Type>> un{ lower, higher };
			return un(mt);
		}


	public:
		// Constructors [7]
		template<concepts::underlying_vector_type T2>
		explicit(not concepts::allow_implicit_conversions::value) vector(T2 lower, T2 higher)
		requires (concepts::is_not_complex<value_type>::value
		and concepts::dynamic_extent_disabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>)
		{
			std::generate(_vector.begin(), _vector.end(), [=]() 
				{
				return get_random<T2>(lower, higher);
				});
		}

		template<concepts::underlying_vector_type T2>
		explicit(not concepts::allow_implicit_conversions::value) vector(T2 lower, T2 higher, size_type count)
		requires (concepts::is_not_complex<value_type>::value
		and concepts::dynamic_extent_enabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>)
			: _vector(count)
		{
			std::generate_n(_vector.begin(), count, [=]() 
				{
				return get_random<T2>(lower, higher);
				});
		}

		template<concepts::underlying_vector_type T2>
		constexpr explicit(not concepts::allow_implicit_conversions::value) vector(T2 lower, T2 higher)
		requires (concepts::is_complex<value_type>::value
		and concepts::dynamic_extent_disabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>)
		{
			using Tp = typename T::value_type;
			std::generate(_vector.begin(), _vector.end(), [=]() 
				{
				return std::complex<Tp>(get_random<T2>(lower, higher),
					get_random<T2>(lower, higher));
				});
		}

		template<concepts::underlying_vector_type T2>
		constexpr explicit(not concepts::allow_implicit_conversions::value) vector(T2 lower, T2 higher, size_type count)
		requires (concepts::is_complex<value_type>::value
		and concepts::dynamic_extent_enabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>)
			: _vector(count)
		{
			using Tp = typename T::value_type;
			std::generate_n(_vector.begin(), count, [=]() 
				{
				return std::complex<Tp>(get_random<T2>(lower, higher), get_random<T2>(lower, higher));
				});
		}


		constexpr vector(const vector& other) = default;
		constexpr vector(vector&& other) noexcept = default;
		constexpr vector& operator=(vector&& other) noexcept = default;
		constexpr vector& operator=(const vector& other) = default;


		constexpr const value_type& operator[] (size_type index) const noexcept
		{
			return _vector[index];
		}

		constexpr value_type& operator[] (size_type index) noexcept
		{
			return _vector[index];
		}

		constexpr const value_type& x() const noexcept
		{
			return _vector[0];
		}

		constexpr const value_type& y() const noexcept
		{
			return _vector[1];
		}

		constexpr const value_type& z() const noexcept
		{
			return _vector[2];
		}

		constexpr const value_type& w() const noexcept
		{
			return _vector[3];
		}


		// Addition operations
		template<concepts::underlying_vector_type T2, size_type OtherSize>
		constexpr vector& operator+=(const vector<T2, OtherSize>& rhs)
		requires (concepts::convertible_to_or_not_narrowing_conversion<decltype(std::declval<value_type>() + std::declval<T2>()), value_type>)
		{
			// The first elements [0, min_size] of both containers must be correctly initialized
			const size_type min_size = std::min(size(), rhs.size());
			const auto begin_t = begin();

			std::transform(begin_t, begin_t + min_size, rhs.begin(), begin_t, [](const T& lhs, const T2& rhs)
				{
					return lhs + rhs;
				});

			return *this;
		}

		template<concepts::underlying_vector_type T2>
		constexpr vector& operator+=(T2 val)
		requires (concepts::convertible_to_or_not_narrowing_conversion<decltype(std::declval<value_type>() + std::declval<T2>()), value_type>)
		{
			std::for_each(begin(), end(),
				[val](value_type& current)
				{
					current += val;
				});

			return *this;
		}

		template<concepts::underlying_vector_type T2>
		friend constexpr vector operator+(vector lhs, T2 val)
		{
			return lhs += val;
		}

		template<concepts::underlying_vector_type T2>
		friend constexpr vector operator+(T2 val, vector lhs)
		{
			return lhs += val;
		}

		template<concepts::underlying_vector_type T2, size_type OtherSize>
		friend constexpr vector operator+(vector lhs, const vector<T2, OtherSize>& rhs)
		{
			return lhs += rhs;
		}

		// Negation
		constexpr vector operator-() const
		requires (not std::is_unsigned<value_type>::value)
		{
			vector temp = *this;
			std::for_each(temp.begin(), temp.end(),
				[](value_type& current)
				{
					current = -current;
				});
			return temp;
		}

		// Substraction operations
	private:
		template<concepts::underlying_vector_type T2, size_type OtherSize>
		constexpr vector& substract(const vector<T2, OtherSize>& rhs)
		requires (concepts::convertible_to_or_not_narrowing_conversion<decltype(std::declval<value_type>() - std::declval<T2>()), value_type>)
		{
			const size_type min_size = std::min(size(), rhs.size());
			const auto begin_t = begin();

			std::transform(begin_t, begin_t + min_size, rhs.begin(), begin_t, [](const T& lhs, const T2& rhs)
				{
					return lhs - rhs;
				});

			return *this;
		}

		template<concepts::underlying_vector_type T2>
		constexpr vector& substract(T2 val)
			requires (concepts::convertible_to_or_not_narrowing_conversion<decltype(std::declval<value_type>() - val), value_type>)
		{
			std::for_each(begin(), end(),
				[val](auto& current) {
					current -= val;
				});

			return *this;
		}

	public:
		template<concepts::underlying_vector_type T2, size_type Size2>
		constexpr vector& operator-=(const vector<T2, Size2>& rhs)
		{
			return substract(rhs);
		}

		template<concepts::underlying_vector_type T2, size_type Size2>
		[[deprecated("substracting vector of type unsigned might cause issues!")]]
		constexpr vector& operator-=(const vector<T2, Size2>& rhs)
			requires (std::is_unsigned<T>::value)
		{
			return substract(rhs);
		}

		template<concepts::underlying_vector_type T2, size_type Size2>
		friend constexpr vector operator-(vector lhs, const vector<T2, Size2>& rhs)
		{
			return lhs -= rhs;
		}

		template<concepts::underlying_vector_type T2>
		constexpr vector& operator-=(T2 val)
		{
			return substract(val);
		}

		template<concepts::underlying_vector_type T2>
		[[deprecated("substracting from an unsigned vector might cause issues!")]]
		constexpr vector& operator-=(T2 val)
			requires (std::is_unsigned<T>::value)
		{
			return substract(val);
		}

		template<concepts::underlying_vector_type T2>
		friend constexpr vector operator-(vector lhs, T2 val)
		{
			return lhs -= val;
		}

	private:
		template<concepts::underlying_vector_type T2>
		constexpr vector& lambda_multiplicator(T2 lambda)
		requires (concepts::convertible_to_or_not_narrowing_conversion<decltype(std::declval<value_type>() * lambda), value_type>)
		{
			std::for_each(begin(), end(), [lambda](auto& current)
				{
					current *= lambda;
				});

			return *this;
		}

	public:
		// Multiplication with a Lambda - Scalar multiplication
		template<concepts::underlying_vector_type T2>
		constexpr vector& operator*=(T2 lambda)
		{
			return lambda_multiplicator(lambda);
		}

		template<concepts::underlying_vector_type T2>
		[[deprecated("multiplying vector of type unsigned might cause issues!")]]
		constexpr vector& operator*=(T2 lambda) requires (std::is_unsigned<T>::value)
		{
			return lambda_multiplicator(lambda);
		}

		template<concepts::underlying_vector_type T2>
		friend constexpr vector operator*(vector lhs, T2 lambda)
		{
			return lhs *= lambda;
		}

		template<concepts::underlying_vector_type T2>
		friend constexpr vector operator*(T2 lambda, vector lhs)
		{
			return lhs *= lambda;
		}


	private:
		// If std::array, Size == 3 for both lhs and rhs
		// If std::vector, check rhs.size()
		template<concepts::underlying_vector_type T2>
		constexpr vector& cross_product_impl(const vector<T2, Size>& rhs)
		requires (concepts::convertible_to_or_not_narrowing_complex_product<value_type, T2>)
		{
			detail::dispatch_assert_equals<Size, 3>(size(), rhs.size());

			vector temp = *this;
			if constexpr (concepts::is_complex<value_type>::value)
			{
				// The cross product for complex vectors has the same formulaes as a cross product in R3, with the only
				// difference being that the final step is taking the complex conjugates of the results.
				_vector[0] = std::conj((temp[1] * rhs[2] - temp[2] * rhs[1]));
				_vector[1] = std::conj((temp[2] * rhs[0] - temp[0] * rhs[2]));
				_vector[2] = std::conj((temp[0] * rhs[1] - temp[1] * rhs[0]));
				return *this;
			}
			_vector[0] = temp[1] * rhs[2] - temp[2] * rhs[1];
			_vector[1] = temp[2] * rhs[0] - temp[0] * rhs[2];
			_vector[2] = temp[0] * rhs[1] - temp[1] * rhs[0];
			return *this;
		}


	public:
		template<concepts::underlying_vector_type T2>
		constexpr vector& cross_product(const vector<T2, Size>& rhs)
		{
			return cross_product_impl(rhs);
		}


		template<concepts::underlying_vector_type T2>
		[[deprecated("Multiplication on two vectors of type unsigned might cause issues!")]]
		constexpr vector& cross_product(const vector<T2, Size>& rhs)
		requires (std::is_unsigned<value_type>::value
		or std::is_unsigned<T2>::value)
		{
			return cross_product_impl(rhs);
		}

		// Division with a constant/lambda - for precision and correctness, use multiplication instead
		template<concepts::underlying_vector_type T2>
		constexpr vector& operator/=(T2 lambda)
		requires (concepts::convertible_to_or_not_narrowing_conversion<decltype(std::declval<T>() / lambda), value_type>)
		{
			// By implicit conversions, the user might pass (for example) 0.9 to <int>,
			// which is truncated to 0 -- static_cast needed for that reason
			detail::assert_div_byzero<value_type>(static_cast<value_type>(lambda));

			std::for_each(std::begin(_vector), std::end(_vector),
				[lambda](auto& current)
				{
					current /= lambda;
				});

			return *this;
		}

		template<concepts::underlying_vector_type T2>
		friend constexpr vector operator/(vector lhs, T2 lambda)
		{
			return lhs /= lambda;
		}

		template<concepts::underlying_vector_type T2>
		constexpr vector& operator%=(T2 lambda)
		requires (std::is_integral_v<value_type> and std::is_integral_v<T2>
		and concepts::is_not_complex<T>::value)
		{
			// No implicit conversions can happen in this function that would cause issues -- no static_cast needed
			detail::assert_div_byzero<value_type>(lambda);

			std::for_each(std::begin(_vector), std::end(_vector),
				[lambda](auto& current)
				{
					current %= lambda;
				});

			return *this;
		}

		template<concepts::underlying_vector_type T2>
		friend constexpr vector operator%(vector lhs, T2 lambda) 
		{
			return lhs %= lambda;
		}

		constexpr void reset() noexcept
		{
			std::fill(std::begin(_vector), std::end(_vector), value_type{});
		}


		// OK + TESTED
	public:
		// Calculates projection of other to this, then stores the result in this 
		template<concepts::underlying_vector_type T2>
		constexpr vector& vector_projection_from(const vector<T2, Size>& from)
		requires (concepts::is_not_all_complex<value_type, T2>)
		{
			detail::assert_same_value<Size>(size(), from.size());

			// Formula: [a DOT b / (b DOT b)] * a
			const auto self = *this;
			const auto denominator = Vector::inner_product(self, self);
			// a null vector results in a scalar product of 0, thus a denominator of 0
			// todo: decide whether this is the appropriate way to proceed, or whether another result can be given (eg. using limits)
			detail::assert_div_byzero<decltype(denominator)>(denominator);
			*this *= (Vector::inner_product(from, self) / denominator);

			return *this;
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			concepts::underlying_vector_type T2>
		constexpr vector& vector_projection_from(const vector<T2, Size>& from)
		requires (concepts::is_all_complex<value_type, T2>) 
		{
			detail::assert_same_value<Size>(size(), from.size());
			const auto self = *this;
			auto denominator = Vector::inner_product<definition_type>(self, self);
			detail::assert_div_byzero<decltype(denominator)>(denominator);
			*this *= Vector::inner_product<definition_type>(from, self);

			return *this;
		}

		// Return direction of a 2D vector. The vector components must be cartesian coordinates. x will return the angle in respect
		// to the x axis, y returns the angle in respect to the y axis 
		constexpr double direction_radians_y() const
		requires (not concepts::is_complex<value_type>::value)
		{
			detail::dispatch_assert_equals<Size, 2>(size());
			return std::atan2(_vector[0], _vector[1]); // (x / y)
		}

		constexpr double direction_radians_x() const
		requires (not concepts::is_complex<value_type>::value)
		{
			detail::dispatch_assert_equals<Size, 2>(size());
			return std::atan2(_vector[1], _vector[0]); // (y / x)
		}

		constexpr double direction_degrees_y() const
		{
			return direction_radians_y() * 180.0 / std::numbers::pi;
		}

		constexpr double direction_degrees_x() const
		{
			return direction_radians_x() * 180.0 / std::numbers::pi;
		}

		constexpr double direction_cosine(Cos type) const
		requires (not concepts::is_complex<value_type>::value)
		{
			detail::dispatch_assert_equals<Size, 3>(size());
			const auto denominator = norm();
			detail::assert_div_byzero<decltype(denominator)>(denominator);
			return static_cast<double>(_vector[static_cast<std::size_t>(type)] / denominator);
		}

		constexpr double direction_angle(Cos type) const
		requires (not concepts::is_complex<value_type>::value) 
		{
			return std::acos(direction_cosine(type));
		}

		constexpr vector& normalize() 
		{
			const auto denominator = norm();
			detail::assert_div_byzero<decltype(denominator)>(denominator);
			*this *= (1 / denominator);

			return *this;
		}


	private:
		// L0/Sparse "norm" for real spaces
		// Note that this is not really a norm by definition, since it doesn't fullfil all the proprieties of a norm
		template<MathLbr::Norm Norm>
		constexpr auto norm_impl() const
		requires (Norm == MathLbr::Norm::L0)
		{
			return std::count_if(begin(), end(), [](const auto& elem)
				{
					return elem != value_type{};
				});
		}

		// Taxicab norm or L1 norm for real/complex spaces
		template<MathLbr::Norm Norm>
		constexpr auto norm_impl() const
		-> concepts::dispatched_underlying_type<value_type>
		requires (Norm == MathLbr::Norm::L1)
		{
			return std::accumulate(begin(), end(), concepts::dispatched_underlying_type<value_type>{},
				[](auto internal_sum, const auto& element)
				{
					return internal_sum + std::abs(element);
				});
		}

		template<typename Function>
		constexpr auto sqrt_sum_of_powers(Function fct) const
		{
			return std::sqrt(std::accumulate(begin(), end(), concepts::dispatched_underlying_type<value_type>{},
				[fct](auto internal_sum, const auto& element) 
				{
					return internal_sum + fct(element);
				}));
		}

		// Euclidean/L2 norm for real spaces
		template<MathLbr::Norm Norm>
		constexpr auto norm_impl() const
		requires (MathLbr::Norm::L2 == Norm and concepts::is_not_complex<value_type>::value)
		{
			return sqrt_sum_of_powers([](const auto& e1) { return e1 * e1; });
		}

		// Euclidean/L2 norm for complex spaces
		template<MathLbr::Norm Norm>
		constexpr auto norm_impl() const
		requires (MathLbr::Norm::L2 == Norm and concepts::is_complex<value_type>::value)
		{
			return sqrt_sum_of_powers([](const auto& e1)
				{
					const auto res = std::abs(e1);
					return res * res;
				});
		}

		// L-inf norm for real and complex spaces
		template<MathLbr::Norm Norm>
		constexpr auto norm_impl() const -> concepts::dispatched_underlying_type<value_type>
		requires (Norm == MathLbr::Norm::LINF)
		{
			return *std::max_element(begin(), end(), [](const auto& a, const auto& b)
				{
					return std::abs(a) < std::abs(b);
				});
		}

	public:
		// Norm generalization. Let p be the number of the norm, e.g. "L-p". Constraint: P>=1; the exceptional case
		// (where p=0) has been covered above, but that case is technically not an actual norm.
		template<std::size_t P>
		constexpr auto p_norm() const requires (P >= 1)
		{
			auto result = std::accumulate(begin(), end(), concepts::dispatched_underlying_type<value_type>{},
				[=](auto internal_sum, const auto& element)
				{
					return internal_sum + (std::pow(std::abs(element), P));
				});
			return std::pow(result, 1. / P);
		}

		template<MathLbr::Norm Norm = MathLbr::Norm::L2>
		constexpr auto norm() const
		{
			return norm_impl<Norm>();
		}


		// Angle between two vectors (in radians)
		template<concepts::underlying_vector_type T2>
		constexpr auto angle_between_radians(const vector<T2, Size>& other) const
		requires (concepts::is_not_all_complex<T, T2>) 
		{
			detail::assert_same_value<Size>(size(), other.size());
			const auto magnitude_mult = norm() * other.norm();
			detail::assert_div_byzero<decltype(magnitude_mult)>(magnitude_mult);

			return std::acos(Vector::inner_product(*this, other) / magnitude_mult);
		}

		template<concepts::underlying_vector_type T2>
		constexpr auto angle_between_degrees(const vector<T2, Size>& other) const
		requires (concepts::is_not_all_complex<T, T2>) 
		{
			return angle_between_radians(other) * 180 / std::numbers::pi;
		}

		// Angle between complex vectors
		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
		concepts::underlying_vector_type T2>
		constexpr auto angle_between_radians(const vector<T2, Size>& other) const
		requires (concepts::is_all_complex<T, T2>) 
		{
			detail::assert_same_value<Size>(size(), other.size());

			const auto magnitude_mult = norm() * other.norm();
			detail::assert_div_byzero<decltype(magnitude_mult)>(magnitude_mult);
			return std::acos(Vector::inner_product<definition_type>(*this, other).real() / magnitude_mult);
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
		concepts::underlying_vector_type T2>
		constexpr auto angle_between_degrees(const vector<T2, Size>& other) const
		requires (concepts::is_all_complex<T, T2>)
		{
			return angle_between_radians<definition_type>(other) *
				static_cast<complex_internal_value_type<value_type>>(180) / std::numbers::pi;
		}

		constexpr bool is_empty() const 
		{
			return (size() == 0 || std::all_of(begin(), end(),
				[](value_type i) { return i == value_type{}; }));
		}

		constexpr auto operator<=>(const vector&) const = default;

		// Conversions: cartesian --> other coordinate system
		constexpr underlying_container to_polar_coords() const
		requires (concepts::is_not_complex<value_type>::value) 
		{
			detail::dispatch_assert_equals<Size, 2>(size());
			detail::assert_div_byzero<value_type>(_vector[0]);
			if constexpr (concepts::dynamic_extent_enabled<Size>) 
			{
				return std::vector<value_type>
				{
					std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2)),
					std::atan2(_vector[1], _vector[0])
				};
			}
			else 
			{
				return std::array<value_type, 2>
				{
					std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2)),
					std::atan2(_vector[1] / _vector[0])
				};
			}
		}

		constexpr underlying_container to_spherical_coords() const
		requires (concepts::is_not_complex<value_type>::value) 
		{
			detail::dispatch_assert_equals<Size, 3>(size());
			detail::assert_div_byzero<value_type>(_vector[0]);

			const auto res = std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2) + std::pow(_vector[2], 2));
			detail::assert_div_byzero<decltype(res)>(res);

			// [0] = alpha, [1] = beta, [3] = theta
			if constexpr (concepts::dynamic_extent_enabled<Size>) 
			{
				return std::vector<value_type>
				{
					res,
					std::atan2(_vector[1], _vector[0]),
					std::acos(_vector[2] / res)
				};
			}
			else 
			{
				return std::array<value_type, 3>
				{
					res,
					std::atan2(_vector[1], _vector[0]),
					std::acos(_vector[2] / res)
				};
			}
		}

		constexpr underlying_container to_cylindrical_coords() const
		requires (concepts::is_not_complex<value_type>::value) 
		{
			detail::dispatch_assert_equals<Size, 3>(size());
			detail::assert_div_byzero<value_type>(_vector[0]);

			if constexpr (concepts::dynamic_extent_enabled<Size>) 
			{
				return std::vector<value_type> 
				{
					std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2)),
					std::atan2(_vector[1], _vector[0]),
					_vector[2]
				};
			}
			else
			{
				return std::array<T, 3> {
					std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2)),
					std::atan2(_vector[1], _vector[0]),
					_vector[2]
				};
			}
		}

		// Polar coordinates --> cartesian vector
		// radius in radians
		constexpr void from_polar(T r, T radius)
		requires (concepts::is_not_complex<T>::value) 
		{
			detail::dispatch_assert_equals<Size, 2>(size());

			_vector[0] = r * std::cos(radius);
			_vector[1] = r * std::sin(radius);
		}

		// Cylindrical coordinates --> cartesian vector
		constexpr void from_cylindrical(T magnitude, T radius, T z)
		requires (concepts::is_not_complex<T>::value) 
		{
			detail::dispatch_assert_equals<Size, 3>(size());

			_vector[0] = magnitude * std::cos(radius);
			_vector[1] = magnitude * std::sin(radius);
			_vector[2] = z;
		}

		// Spherical coordinates --> cartesian vector
		constexpr void from_spherical(T alpha, T beta, T theta)
		requires (concepts::is_not_complex<T>::value) 
		{
			detail::dispatch_assert_equals<Size, 3>(size());

			_vector[0] = alpha * std::sin(theta) * std::cos(beta);
			_vector[1] = alpha * std::sin(theta) * std::sin(beta);
			_vector[2] = alpha * std::cos(theta);
		}

		friend std::ostream& operator<< (std::ostream& stream, const vector& rhs) 
		{
			for (auto x : rhs._vector) {
				stream << x << ' ';
			}
			stream << '\n';

			return stream;
		}

		// Todo: Remove rotations and simplify calculus when Math::Matrix is available
		constexpr void rotate_around_z(T rotation_angle)
		requires (concepts::is_not_complex<T>::value) 
		{
			detail::dispatch_assert_equals<Size, 3>(size());

			const auto cos = std::cos(rotation_angle);
			const auto sin = std::sin(rotation_angle);
			const T tempX = _vector[0] * cos - _vector[1] * sin;
			const T tempY = _vector[0] * sin + _vector[1] * cos;
			_vector[0] = tempX;
			_vector[1] = tempY;
		}

		constexpr void rotate_around_y(T rotation_angle)
		requires (concepts::is_not_complex<T>::value) 
		{
			detail::dispatch_assert_equals<Size, 3>(size());

			const auto cos = std::cos(rotation_angle);
			const auto sin = std::sin(rotation_angle);
			const T tempX = _vector[0] * cos + _vector[2] * sin;
			const T tempZ = -(_vector[0]) * sin + _vector[2] * cos;
			_vector[0] = tempX;
			_vector[2] = tempZ;
		}

		constexpr void rotate_around_x(T rotation_angle)
		requires (concepts::is_not_complex<T>::value) 
		{
			detail::dispatch_assert_equals<Size, 3>(size());

			const auto cos = std::cos(rotation_angle);
			const auto sin = std::sin(rotation_angle);
			const T tempY = _vector[1] * cos - _vector[2] * sin;
			const T tempZ = _vector[1] * sin + _vector[2] * cos;
			_vector[1] = tempY;
			_vector[2] = tempZ;
		}

		template<typename Function>
		constexpr vector apply_foreach(Function function) noexcept(noexcept(function)) 
		{
			for (size_type i = 0; i < size(); ++i) 
			{
				function(_vector[i]);
			}
			return *this;
		}

		constexpr size_type size() const noexcept 
		{
			if constexpr (concepts::dynamic_extent_enabled<Size>) 
			{
				return _vector.size();
			}
			else return Size;
		}

		constexpr auto begin() noexcept
		{
			return std::begin(_vector);
		}

		constexpr auto begin() const noexcept 
		{
			return std::cbegin(_vector);
		}

		constexpr auto cbegin() const noexcept
		{
			return std::cbegin(_vector);
		}

		constexpr auto end() noexcept
		{
			return std::end(_vector);
		}

		constexpr auto end() const noexcept 
		{
			return std::cend(_vector);
		}

		constexpr auto cend() const noexcept
		{
			return std::cend(_vector);
		}

		constexpr auto rbegin() noexcept
		{
			return std::rbegin(_vector);
		}

		constexpr auto rbegin() const noexcept
		{
			return std::crbegin(_vector);
		}

		constexpr auto crbegin() const noexcept
		{
			return std::crbegin(_vector);
		}

		constexpr auto rend() noexcept
		{
			return std::rend(_vector);
		}

		constexpr auto rend() const noexcept
		{
			return std::crend(_vector);
		}

		constexpr auto crend() const noexcept
		{
			return std::crend(_vector);
		}

		template<concepts::underlying_vector_type T2>
		constexpr void add(T2 new_element)
		requires (concepts::dynamic_extent_enabled<Size>
		and concepts::convertible_to_or_not_narrowing_conversion<T2, value_type>) 
		{
			_vector.push_back(static_cast<value_type>(new_element));
		}

		constexpr std::vector<value_type>& underlying_data()
		requires (concepts::dynamic_extent_enabled<Size>) 
		{
			return _vector;
		}

		constexpr std::array<T, Size>& underlying_data() 
		requires (not concepts::dynamic_extent_disabled<Size>)
		{
			return _vector;
		}

	};

	// Deduction guides
	template<typename T, std::size_t Sz>
	vector(const T(&)[Sz])->vector<T, Sz>;

	template<typename InputIt>
	vector(InputIt, InputIt)->vector<typename std::iterator_traits<InputIt>::value_type>;
}

#endif
