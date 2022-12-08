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

namespace MathLbr {

	template<Concepts::underlying_vector_type T, std::size_t Size = dynamic_extent>
	class vector;


	namespace Vector {
		template<Concepts::underlying_vector_type T, std::size_t Size, typename T2>
		constexpr vector<T, Size> lambda_multiplication(const vector<T, Size>& vec, T2 lambda) {
			using Type = decltype(std::declval<T>()* std::declval<T2>());
			vector<Type, Size> temp(vec);

			return temp.lambda_multiplicator(lambda);
		}


		template<typename T, std::size_t Size>
		constexpr vector<T, Size> normalize(vector<T, Size> other) {
			other.normalize();
			return other;
		}


		template<Concepts::underlying_vector_type T, std::size_t Size1>
		constexpr vector<T, Size1> cross_product(vector<T, Size1> lhs, const vector<T, Size1>& rhs) {
			return lhs.cross_product(rhs);
		}

		template<Concepts::underlying_vector_type T, std::size_t Size>
		constexpr T inner_product(const vector<T, Size>& lhs, const vector<T, Size>& other)
			requires (not Concepts::is_complex<T>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(lhs.size() == other.size());
			}
			return std::inner_product(lhs.begin(), lhs.end(),
				other.begin(), T{});
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			Concepts::underlying_vector_type T, std::size_t Size>
		constexpr T inner_product(const vector<T, Size>& lhs, const vector<T, Size>& other)
			requires (Concepts::is_complex<T>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(lhs.size() == other.size());
			}

			T sum = {};
			if constexpr (definition_type == ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT) {
				for (std::size_t i = 0; i < lhs.size(); ++i) {
					sum += (std::conj(lhs[i]) * (other[i]));
				}
			} else if constexpr (definition_type == ComplexInnerProduct::ANTILINEAR_SECOND_ARGUMENT) {
				for (std::size_t i = 0; i < lhs.size(); ++i) {
					sum += (lhs[i] * std::conj(other[i]));
				}
			}
			return sum;
		}

		template<Concepts::arithmetic_char_const_excluded T, Concepts::arithmetic_char_const_excluded T2>
		constexpr auto complex_inner_product(const std::complex<T>& lhs, const std::complex<T2>& rhs)
			-> decltype(std::declval<T>()* std::declval<T2>() + std::declval<T>() * std::declval<T2>()) {
			return lhs.real() * rhs.real() + lhs.imag() * rhs.imag();
		}

		template<Concepts::arithmetic_char_const_excluded T, Concepts::arithmetic_char_const_excluded T2>
		constexpr auto complex_cross_product(const std::complex<T>& lhs, const std::complex<T2> rhs)
			-> decltype(std::declval<T>()* std::declval<T2>() - std::declval<T>() * std::declval<T2>()) {
			return lhs.real() * rhs.imag() - lhs.imag() * rhs.real();
		}

		template<typename T, std::size_t Size>
		constexpr T scalar_triple_product(const vector<T, Size>& first, const vector<T, Size>& other2,
			const vector<T, Size>& other3)
		requires (not Concepts::is_complex<T>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(first.size() == other2.size() and first.size() == other3.size());
			}

			return inner_product(other3, cross_product(first, other2));
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			typename T, std::size_t Size>
		constexpr T scalar_triple_product(const vector<T, Size>& first, const vector<T, Size>& other2,
			const vector<T, Size>& other3)
			requires (Concepts::is_complex<T>::value) {

			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(first.size() == other2.size() and first.size() == other3.size());
			}
			return inner_product<definition_type>(other3, cross_product(first, other2));
		}

		template<typename T, std::size_t Size>
		constexpr vector<T, Size> vector_triple_product(const vector<T, Size>& first, const vector<T, Size>& other2,
			const vector<T, Size>& other3) {
			if constexpr (!Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				static_assert(Size == 3, "scalar triple product only accepts 3D vectors!");
			}
			else assert(first.size() == other2.size() and first.size() == other3.size() and first.size() == 3);

			return cross_product(other3, cross_product(first, other2));
		}

		template<typename T, std::size_t Size>
		constexpr bool are_parallel(const vector<T, Size>& first, const vector<T, Size>& other,
			double epsilon = 1E-6)
		requires (not Concepts::is_complex<T>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(other.size() == first.size());
			}

			// 3 dimensions case: checks whether the cross product is near 0
			if (first.size() == 3) {
				vector<T, Size> temp = cross_product(first, other);
				// Two vectors are parallel in 3D only if their cross product result is near zero.
				return temp[0] >= 0 && temp[0] <= std::abs(epsilon)
					&& temp[1] >= 0 && temp[1] <= std::abs(epsilon)
					&& temp[2] >= 0 && temp[2] <= std::abs(epsilon);
			}
			// Other dimensions: scalar product method. Parallelism condition is true if result's near 0 (approx 1E-6)
			auto sum_and_pow = [](auto sum, const auto& element) {
				return static_cast<T>(sum + std::pow(element, 2));
			};

			T inner_prod = inner_product(first, other);
			double lhs = std::pow(inner_prod, 2);
			double rhs1 = std::accumulate(std::begin(first), std::end(first),
				T{}, sum_and_pow);
			double rhs2 = std::accumulate(std::begin(other), std::end(other),
				T{}, sum_and_pow);
			return is_near_zero(std::abs(rhs1 * rhs2 - lhs), epsilon);
		}

		// To review
		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			typename T, std::size_t Size>
		constexpr bool are_parallel(const vector<T, Size>& first, const vector<T, Size>& other,
			double epsilon = 1E-6)
		requires (Concepts::is_complex<T>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(other.size() == first.size());
			}

			auto result = std::abs(inner_product<definition_type>(normalize(first), normalize(other)).real());
			return ((result >= 1 && result <= (1 + epsilon)) || (result <= 1 && result >= (1 - epsilon)));
		}

		template<typename T, std::size_t Size>
		constexpr bool are_perpendicular(const vector<T, Size>& first, const vector<T, Size>& other, double epsilon = 1E-6)
		requires (not Concepts::is_complex<T>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(other.size() == first.size());
			}
			auto inner_prod = inner_product(first, other);
			return is_near_zero(inner_prod, epsilon);
		}

		// Should be OK, see https://www.sciencedirect.com/topics/mathematics/orthogonal-set-of-vector
		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			typename T, std::size_t Size>
		constexpr bool are_perpendicular(const vector<T, Size>& first, const vector<T, Size>& other, double epsilon = 1E-6)
		requires (Concepts::is_complex<T>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(other.size() == first.size());
			}
			auto inner_prod = inner_product<definition_type>(first, other);
			return is_near_zero_complex(inner_prod, epsilon);
		}

		template<typename T, std::size_t Size>
		constexpr bool are_coplanar(const vector<T, Size>& first, const vector<T, Size>& other2,
			const vector<T, Size>& other3, double epsilon = 1E-6)
		requires (not Concepts::is_complex<T>::value) {
			if constexpr (!Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				static_assert(Size == 3, "scalar triple product only accepts 3D vectors!");
			}
			else assert(first.size() == other2.size() and first.size() == other3.size() and first.size() == 3);

			auto result = scalar_triple_product(first, other2, other3);
			return is_near_zero(result, epsilon);
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
			typename T, std::size_t Size>
		constexpr bool are_coplanar(const vector<T, Size>& first, const vector < T, Size>& other2,
			const vector<T, Size> other3, double epsilon = 1E-6)
		requires (Concepts::is_complex<T>::value) {
			if constexpr (!Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				static_assert(Size == 3, "scalar triple product only accepts 3D vectors!");
			}
			else assert(first.size() == other2.size() and first.size() == other3.size() and first.size() == 3);

			auto result = scalar_triple_product<definition_type>(first, other2, other3);
			return is_near_zero_complex(result, epsilon);
		}


		template<typename T, std::size_t Size>
		constexpr vector<T, Size> projection(vector<T, Size> to, const vector<T, Size>& from)
		requires (not Concepts::is_complex<T>::value) {
			return to.vector_projection_from(from);
		}

		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT,
		typename T, std::size_t Size>
		constexpr vector<T, Size> projection(vector<T, Size> to, const vector<T, Size>& from)
		requires (Concepts::is_complex<T>::value) {
			return to.vector_projection_from<definition_type>(from);
		}
	}


	/*
		General vector expressed in cartesian coordinates - accepts the specified type and any size.
		Not all internal operations are yet defined for complex numbers.
		For a specialization of this vector representation with polar coordinates, see @...
	*/

	template<Concepts::underlying_vector_type T, std::size_t Size>
	class vector {


	private:
		template<Concepts::underlying_vector_type T2, std::size_t Size2>
		friend class vector;

		[[no_unique_address]] std::conditional_t<Concepts::dynamic_extent_enabled<Size, dynamic_extent>,
			std::vector<T>, std::array<T, Size>> _vector;

	public:
		using size_type = std::size_t;
		using value_type = T;
		using pointer_type = T*;
		using reference_type = T&;
		using const_pointer_type = const T*;
		using const_reference_type = const T&;
		using underlying_container = std::conditional_t<Concepts::dynamic_extent_enabled<Size, dynamic_extent>,
			std::vector<T>, std::array<T, Size>>;
		template<typename _T>
		using complex_internal_value_type = typename Concepts::is_complex<_T>::value_type;
	


		/*
		Constructors: default (1), same value for all elements (2),
		initialize internal vector through another container (3),
		initialize internal vector through an multiple values (4),
		fill the internal vector with random
		numbers (5), others = defaulted
		*/

		constexpr vector()
		requires (not Concepts::dynamic_extent_enabled<Size, dynamic_extent>) = default;
		
		constexpr explicit vector(value_type value)
		requires (!Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
			std::fill(std::begin(_vector), std::end(_vector), value);
		}

		constexpr explicit vector(size_type count)
		requires (Concepts::dynamic_extent_enabled<Size, dynamic_extent>)
			: _vector{ std::vector<T>(count) } {}


		template<std::input_iterator InputIter>
		constexpr vector(InputIter first, InputIter last) {
			static_assert(std::convertible_to<typename std::iterator_traits<InputIter>::value_type,
				value_type>,
				"The type of InputIter must be convertible to the type of Math::Vector");
			const auto distance = std::distance(first, last);
			MATHLBR_ASSERT_SIZE_MISMATCH(distance, Size);

			MATHLBR_VECTOR_INIT_IF_DYNAMIC(distance);
			std::copy(first, last, std::begin(_vector));

			VALUE_INITIALIZE_REMAINING_ELEMENTS(distance, _vector.size());
		}


		template<size_type Sz>
		constexpr explicit vector(const value_type(&arr)[Sz])
			: vector(arr, std::make_index_sequence<Sz>{}) {}

	private:
		template <size_type... Index>
		constexpr explicit vector(const value_type(&arr)[sizeof...(Index)],
			std::index_sequence<Index...>) {

			MATHLBR_VECTOR_INIT_IF_DYNAMIC(sizeof...(Index));
			_vector = { arr[Index]... }; 

			VALUE_INITIALIZE_REMAINING_ELEMENTS(sizeof...(Index), _vector.size());
		}

		constexpr void initialize_vector(size_type count, value_type val = value_type{})
		requires (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
			_vector = std::vector<T>(count, val);
		}

	public:
		template<typename T2, size_type Size2>
		constexpr vector(const vector<T2, Size2>& other)
		requires (std::convertible_to<T2, value_type>) {
			MATHLBR_ASSERT_SIZE_MISMATCH(other.size(), Size);
			MATHLBR_VECTOR_INIT_IF_DYNAMIC(other.size());

			std::copy(std::begin(other._vector), std::end(other._vector),
				std::begin(_vector));

			VALUE_INITIALIZE_REMAINING_ELEMENTS(other.size(), _vector.size());
		}

		// Currently useless; keep for future changes of the allowed types.
		template<typename T2, size_type Size2>
		constexpr vector(vector<T2, Size2>&& other) noexcept (std::is_nothrow_move_constructible_v<T>)
		requires (std::convertible_to<T2, value_type>) {
			MATHLBR_ASSERT_SIZE_MISMATCH(other.size(), Size);
			MATHLBR_VECTOR_INIT_IF_DYNAMIC(other.size());

			std::move(std::begin(other._vector), std::end(other._vector),
				std::begin(_vector));

			VALUE_INITIALIZE_REMAINING_ELEMENTS(other.size(), _vector.size());
		}


	private:
		template<typename Type>
		Type get_random(Type lower, Type higher) {
			static std::mt19937 mt(std::random_device{}());
			std::uniform_real_distribution<double> un{ static_cast<double>(lower),
				static_cast<double>(higher) };

			return static_cast<Type>(un(mt));
		}



	public:
		explicit vector(value_type lower, value_type higher, size_type count = 1) 
		requires (!Concepts::is_complex<T>::value) {
			MATHLBR_VECTOR_INIT_IF_DYNAMIC(count);

			for (size_type i = 0; i < size(); ++i) {
				_vector[i] = get_random(lower, higher);
			}
		}

		constexpr explicit vector(complex_internal_value_type<value_type> lower, complex_internal_value_type<value_type> higher,
			size_type count = 1)
		requires (Concepts::is_complex<T>::value) {
			MATHLBR_VECTOR_INIT_IF_DYNAMIC(count);

			using Tp = typename T::value_type;
			for (size_type i = 0; i < size(); ++i) {
				_vector[i] = { 
					get_random<Tp>(lower, higher),
					get_random<Tp>(lower, higher) 
				};
			}
		}

	
		constexpr vector(const vector& other) = default;
		constexpr vector(vector&& other) noexcept = default;
		constexpr vector& operator=(vector&& other) noexcept = default;
		constexpr vector& operator=(const vector& other) = default;


		constexpr const value_type& operator[] (size_type index) const noexcept {
			return _vector[index];
		}

		constexpr value_type& operator[] (size_type index) noexcept {
			return _vector[index];
		}

		constexpr const value_type& x() const noexcept {
			return _vector[0];
		}

		constexpr const value_type& y() const noexcept {
			return _vector[1];
		}

		constexpr const value_type& z() const noexcept {
			return _vector[2];
		}

		constexpr const value_type& w() const noexcept {
			return _vector[3];
		}

		// Addition operations
		template<size_type Sz2>
		constexpr vector& operator+=(const vector<T, Sz2>& rhs) {

			size_type min_size = std::min(size(), rhs.size());
			for (size_type i = 0; i < min_size; ++i) {
				_vector[i] += rhs[i];
			}

			return *this;
		}

		template<Concepts::underlying_vector_type T2>
		constexpr vector& operator+=(T2 val) {
			std::for_each(std::begin(_vector), std::end(_vector),
				[val](value_type& current) {
					current += val;
				});

			return *this;
		}

		template<Concepts::underlying_vector_type T2>
		friend constexpr vector operator+(vector lhs, T2 val) {
			return lhs += val;
		}

		template<Concepts::underlying_vector_type T2>
		friend constexpr vector operator+(T2 val, vector lhs) {
			return lhs += val;
		}

		template<size_type Size2>
		friend constexpr vector operator+(vector lhs, const vector<T, Size2>& rhs) {
			return lhs += rhs;
		}

	public:
		constexpr vector operator-()
		requires (not std::is_unsigned<T>::value) {
			vector temp = *this;
			for (auto& current : temp) {
				current = -current;
			}
			return temp;
		}


	// Substraction operations
	private:
		template<size_type Size2>
		constexpr vector& substract(const vector<T, Size2>& rhs) {

			size_type min_size = std::min(size(), rhs.size());
			for (size_type i = 0; i < min_size; ++i) {
				_vector[i] -= rhs[i];
			}
			return *this;
		}

		template<Concepts::underlying_vector_type T2>
		constexpr vector& substract(T2 val) {
			std::for_each(std::begin(_vector), std::end(_vector),
				[val](auto& current) {
					current -= val;
				});

			return *this;
		}

	public:
		template<size_type Size2>
		constexpr vector& operator-=(const vector<T, Size2>& rhs) {
			return substract(rhs);
		}

		template<size_type Size2>
		[[deprecated("substracting vector of type unsigned might cause issues!")]]
		constexpr vector& operator-=(const vector<T, Size2>& rhs)
		requires std::is_unsigned<T>::value{
				return substract(rhs);
		}

		template<size_type Size2>
		friend constexpr vector operator-(vector lhs, const vector<T, Size2>& rhs) {
			return lhs -= rhs;
		}

		template<Concepts::underlying_vector_type T2>
		constexpr vector& operator-=(T2 val) {
			return substract(val);
		}

		template<Concepts::underlying_vector_type T2>
		[[deprecated("substracting from an unsigned vector might cause issues!")]]
		constexpr vector& operator-=(T2 val)
		requires std::is_unsigned<T>::value{
				return substract(val);
		}

		template<Concepts::underlying_vector_type T2>
		friend constexpr vector operator-(vector lhs, T2 val) {
			return lhs -= val;
		}

	private:
		template<Concepts::underlying_vector_type T2>
		constexpr vector& lambda_multiplicator(T2 lambda) {
			std::for_each(begin(), end(), [lambda](auto& current) {
				current *= lambda;
				});

			return *this;
		}

	public:
		// Multiplication with a Lambda - Scalar multiplication
		template<Concepts::underlying_vector_type T2>
		constexpr vector& operator*=(T2 lambda) {
			return lambda_multiplicator(lambda);
		}

		template<Concepts::underlying_vector_type T2>
		[[deprecated("multiplying vector of type unsigned might cause issues!")]]
		constexpr vector& operator*=(T2 lambda)
		requires std::is_unsigned<T>::value {
			return lambda_multiplicator(lambda);
		}

		template<Concepts::underlying_vector_type T2>
		friend constexpr vector operator*(vector lhs, T2 lambda) {
			return lhs *= lambda;
		}

		template<Concepts::underlying_vector_type T2>
		friend constexpr vector operator*(T2 lambda, vector lhs) {
			return lhs *= lambda;
		}


	private:
		constexpr vector& cross_product_impl(const vector& rhs) {
			if constexpr (!Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				static_assert(Size == 3, "cross product only accepts 3D vectors!");
			}
			else assert(size() == rhs.size() and rhs.size() == 3);

			vector temp{ *this };

			if constexpr (Concepts::is_complex<value_type>::value) {
				// The cross product for complex vectors has the same formulas as a cross product in R3, with the only
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
		constexpr vector& cross_product(const vector& rhs) {
			return cross_product_impl(rhs);
		}


		[[deprecated("Multiplication on two vectors of type unsigned might cause issues!")]]
		constexpr vector& cross_product(const vector& rhs)
		requires std::is_unsigned<value_type>::value {
			return cross_product_internal(rhs);
		}

		// Division with a constant/lambda - for precision and correctness, use multiplication instead
		template<Concepts::underlying_vector_type T2>
		constexpr vector& operator/=(T2 lambda) {
			ASSERT_DIV_BYZERO(lambda);

			std::for_each(std::begin(_vector), std::end(_vector),
				[lambda](auto& current) {
					current /= lambda;
				});

			return *this;
		}

		template<Concepts::underlying_vector_type T2>
		friend constexpr vector operator/(vector lhs, T2 lambda) {
			return lhs /= lambda;
		}

		template<Concepts::underlying_vector_type T2>
		constexpr vector& operator%=(T2 lambda)
		requires (std::is_integral_v<T>
		and std::is_integral_v<T2>
		and not Concepts::is_complex<T>::value) {
			ASSERT_DIV_BYZERO(lambda);

			std::for_each(std::begin(_vector), std::end(_vector),
				[lambda](auto& current) {
					current %= lambda;
				});

			return *this;
		}

		template<Concepts::underlying_vector_type T2>
		friend constexpr vector operator%(vector lhs, T2 lambda) {
			return lhs %= lambda;
		}


		constexpr void reset() noexcept {
			std::fill(std::begin(_vector), std::end(_vector), T{});
		}
	
	private:
		constexpr vector inner_product(const vector& other) const {
			return std::inner_product(std::begin(_vector), std::end(_vector),
				std::begin(other._vector), T{});
		}
		
	public:


		// Calculates projection of other to this, then stores the result in this 
		constexpr vector& vector_projection_from(const vector& from) 
		requires (not Concepts::is_complex<value_type>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(size() == from.size());
			}

			T denominator = Vector::inner_product(*this, *this);
			// a null vector results in a scalar product of 0, thus a denominator of 0
			// todo: decide whether this is the appropriate way to proceed, or whether another result can be given (eg. using limits)
			ASSERT_DIV_BYZERO(denominator);
			*this *= (Vector::inner_product(from, *this) / denominator);

			return *this;
		}

		// Calculates projection of other to this for complex types, storing result in this
		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT>
		constexpr vector& vector_projection_from(const vector& from) 
		requires (Concepts::is_complex<value_type>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(size() == from.size());
			}
			T denominator = Vector::inner_product<definition_type>(*this, *this);

			ASSERT_DIV_BYZERO(denominator);
			*this *= Vector::inner_product<definition_type>(from, *this);
			return *this;
		}

		// Return direction of a 2D vector. The vector components must be cartesian coordinates. x will return (angle)x, 
		// y returns (angle)y. 
		constexpr double direction_radians_y() const 
		requires (not Concepts::is_complex<T>::value) {
			assert(size() == 2);
			return std::atan2(_vector[0], _vector[1]); // (x / y)
		}

		constexpr double direction_radians_x() const
		requires (not Concepts::is_complex<T>::value) {
			assert(size() == 2);
			return std::atan2(_vector[1], _vector[0]); // (y / x)
		}

		constexpr double direction_degrees_y() const {
			return direction_radians_y() * 180.0 / std::numbers::pi;
		}

		constexpr double direction_degrees_x() const {
			return direction_radians_x() * 180.0 / std::numbers::pi;
		}


		constexpr double direction_cosine(Cos type) const
		requires (not Concepts::is_complex<T>::value) {
			assert(size() == 3);
			double denominator = magnitude();
			ASSERT_DIV_BYZERO(denominator);

			return static_cast<double>(_vector[type] / denominator);
		}

		constexpr double direction_angle(Cos type) const
		requires (not Concepts::is_complex<T>::value) {
			return std::acos(direction_cosine(type));
		}

		constexpr vector& normalize() {
			double denominator = magnitude();
			ASSERT_DIV_BYZERO(denominator);
			*this *= static_cast<T>(1 / magnitude());
			return *this;
		}

		// Returns || magnitude || of this vector
		constexpr double magnitude() const 
		requires (not Concepts::is_complex<value_type>::value) {
			double sum = std::accumulate(begin(), end(), T{},
				[](auto internal_sum, const auto& element) {
					return static_cast<value_type>(internal_sum + std::pow(element, 2));
				});

			return std::sqrt(sum);
		}

		// Computes the norm for a complex vector
		constexpr double magnitude() const
		requires (Concepts::is_complex<value_type>::value) {
			// The norm of a complex vector has a different definition than that of a vector with R components.
			double sum = std::accumulate(std::begin(_vector), std::end(_vector), complex_internal_value_type<T>{},
				[](auto sum, const auto& element) {
					return static_cast<complex_internal_value_type<value_type>>(sum + std::pow(std::abs(element.real()), 2)
						+ std::pow(std::abs(element.imag()), 2));
				});

			return std::sqrt(sum);
		}

		

		// Angle between two vectors (in radians)
		constexpr double angle_between_radians(const vector& other)
		requires (not Concepts::is_complex<value_type>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(size() == other.size());
			}
			double magnitude_mult = magnitude() * other.magnitude();
			ASSERT_DIV_BYZERO(magnitude_mult);

			return std::acos(Vector::inner_product(*this, other) / magnitude_mult);
		}

		constexpr double angle_between_degrees(const vector& other) 
		requires (not Concepts::is_complex<value_type>::value) {
			return angle_between_radians(other) * 180 / std::numbers::pi;
		}

		// Angle between complex vectors
		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT>
		constexpr double angle_between_radians(const vector& other) 
		requires (Concepts::is_complex<value_type>::value) {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				assert(size() == other.size());
			}
			double magnitude_mult = magnitude() * other.magnitude();
			ASSERT_DIV_BYZERO(magnitude_mult);
			return std::acos(Vector::inner_product<definition_type>(*this, other).real() / magnitude_mult);
		}


		template<ComplexInnerProduct definition_type = ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT>
		constexpr double angle_between_degrees(const vector& other) 
		requires (Concepts::is_complex<value_type>::value) {
			return angle_between_radians<definition_type>(other) * 
				static_cast<complex_internal_value_type<value_type>>(180) / std::numbers::pi;
		}

		constexpr bool is_empty() const {
			return (size() == 0 || std::all_of(std::begin(_vector), std::end(_vector),
				[](value_type i) { return i == value_type{}; }));
		}

		constexpr auto operator<=>(const vector&) const = default;

		// Conversions: cartesian --> other coordinate system
		constexpr underlying_container to_polar_coords()
		requires (not Concepts::is_complex<value_type>::value) {
			assert(size() == 2);
			ASSERT_DIV_BYZERO(_vector[0]);
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				return std::vector<value_type>{
					std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2)),
					std::atan2(_vector[1], _vector[0])
				};
			} 
			else return std::array<value_type, 2>{
				std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2)),
				std::atan2(_vector[1] / _vector[0]) 
			};
		}

		constexpr underlying_container to_spherical_coords()
		requires (not Concepts::is_complex<value_type>::value) {
			assert(size() == 3);

			auto res = std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2) +
				std::pow(_vector[2], 2));
			ASSERT_DIV_BYZERO(res);
			ASSERT_DIV_BYZERO(_vector[0]);

			// [0] = alpha, [1] = beta, [3] = theta
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				return std::vector<value_type>{
					res,
					std::atan2(_vector[1], _vector[0]),
					std::acos(_vector[2] / res)
				};
			} 
			else return std::array<value_type, 3>{
				res,
				std::atan2(_vector[1], _vector[0]),
				std::acos(_vector[2] / res)
			};
		}

		constexpr underlying_container to_cylindrical_coords()
		requires (not Concepts::is_complex<value_type>::value) {
			assert(size() == 3);
			ASSERT_DIV_BYZERO(_vector[0]);

			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				return std::vector<value_type> {
					std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2)),
					std::atan2(_vector[1], _vector[0]),
					_vector[2]
				};
			} 
			else return std::array<T, 3> {
				std::sqrt(std::pow(_vector[0], 2) + std::pow(_vector[1], 2)),
				std::atan2(_vector[1], _vector[0]),
				_vector[2]
			};
		}

		// Polar coordinates --> cartesian vector
		// radius in radians
		constexpr void from_polar(T r, T radius)
		requires (not Concepts::is_complex<T>::value) {
			assert(size() == 2);

			_vector[0] = r * std::cos(radius);
			_vector[1] = r * std::sin(radius);	
		}

		// Cylindrical coordinates --> cartesian vector
		constexpr void from_cylindrical(T magnitude, T radius, T z)
		requires (not Concepts::is_complex<T>::value) {
			assert(size() == 3);

			_vector[0] = magnitude * std::cos(radius);
			_vector[1] = magnitude * std::sin(radius);
			_vector[2] = z;
		}

		// Spherical coordinates --> cartesian vector
		constexpr void from_spherical(T alpha, T beta, T theta)
		requires (not Concepts::is_complex<T>::value) {
			assert(size() == 3);

			_vector[0] = alpha * std::sin(theta) * std::cos(beta);
			_vector[1] = alpha * std::sin(theta) * std::sin(beta);
			_vector[2] = alpha * std::cos(theta);
		}

		friend std::ostream& operator<< (std::ostream& stream, const vector& rhs) {
			for (auto x : rhs._vector) {
				stream << x << ' ';
			}
			stream << '\n';

			return stream;
		}

		// Todo: Remove rotations and simplify calculus when Math::Matrix is available
		constexpr void rotate_around_z(T rotation_angle)
		requires (not Concepts::is_complex<T>::value) {
			assert(size() == 3);

			auto cos = std::cos(rotation_angle);
			auto sin = std::sin(rotation_angle);
			T tempX = _vector[0] * cos -
				_vector[1] * sin;
			T tempY = _vector[0] * sin +
				_vector[1] * cos;
			_vector[0] = tempX;
			_vector[1] = tempY;
		}

		constexpr void rotate_around_y(T rotation_angle)
		requires (not Concepts::is_complex<T>::value) {
			assert(size() == 3);

			auto cos = std::cos(rotation_angle);
			auto sin = std::sin(rotation_angle);
			T tempX = _vector[0] * cos +
				_vector[2] * sin;
			T tempZ = -(_vector[0]) * sin +
				_vector[2] * cos;
			_vector[0] = tempX;
			_vector[2] = tempZ;
		}

		constexpr void rotate_around_x(T rotation_angle)
		requires (not Concepts::is_complex<T>::value) {
			assert(size() == 3);

			auto cos = std::cos(rotation_angle);
			auto sin = std::sin(rotation_angle);
			T tempY = _vector[1] * cos -
				_vector[2] * sin;
			T tempZ = _vector[1] * sin +
				_vector[2] * cos;
			_vector[1] = tempY;
			_vector[2] = tempZ;
		}

		template<typename Function>
		constexpr vector apply_foreach(const Function& function)
			noexcept(noexcept(function)) {

			for (size_type i{}; i < size(); ++i) {
				function(_vector[i]);
			}
			return *this;
		}


		constexpr size_type size() const noexcept {
			if constexpr (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
				return _vector.size();
			} else return Size;
		}

		constexpr auto begin() const noexcept {
			return std::begin(_vector);
		}

		constexpr auto end() const noexcept {
			return std::end(_vector);
		}
		
		constexpr auto begin() noexcept {
			return std::begin(_vector);
		}

		constexpr auto end() noexcept {
			return std::end(_vector);
		}

		constexpr void add(value_type new_element) 
		requires (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
			_vector.push_back(new_element);
		}

		constexpr std::vector<T>& underlying_data() 
		requires (Concepts::dynamic_extent_enabled<Size, dynamic_extent>) {
			return _vector;
		}

	};

	// Deduction guide
	template<typename T, std::size_t Sz>
	vector(const T(&)[Sz])->vector<T, Sz>;

}

#endif
