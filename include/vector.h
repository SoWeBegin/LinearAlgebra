#ifndef VECTOR_MATH_MINILIBRARY
#define VECTOR_MATH_MINILIBRARY

#include <array>
#include <algorithm>
#include <cmath>
#include <complex>
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


// Todo: remove friend functions from vector and move them in namespace Math::

#define ASSERT_SIZE_MISMATCH(Size, Size2) assert((Size2 >= Size and "Size mismatch!"));

#define ASSERT_DIV_BYZERO(Value) assert((Value != T{} and "Cannot perform division by zero!"));

#define STATICASSERT_COMPLEX_DIFFTYPES(Type1, Type2)\
if constexpr (is_complex<T>::value)\
static_assert(std::same_as<Type1, Type2>);\



namespace MathLbr {

	enum class Cos { X_AXIS = 0, Y_AXIS, Z_AXIS };

	template<typename T> struct is_complex : std::false_type {};
	template<typename T> struct is_complex<std::complex<T>> : std::true_type {};

	// only allows arithmetic types (chars excluded) and complex numbers. 
	// const types are unneccessary as they disallow most operations.
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

	/*
		General vector expressed in cartesian coordinates - accepts the specified type and any size.
		Not all internal operations are yet defined for complex numbers.
		For a specialization of this vector representation with polar coordinates, see @...
	*/

	template<underlying_vector_type T, std::size_t Size>
	class vector {

	private:
		template<underlying_vector_type T2, std::size_t Size2>
		friend class vector;

		std::array<T, Size> _vector;

	public:
		using size_type = std::size_t;
		using value_type = T;
		using pointer_type = T*;
		using reference_type = T&;
		using const_pointer_type = T* const;

		/*
		Constructors: default (1), same value for all elements (2), 
		initialize internal vector through another container (3),
		initialize internal vector through an multiple values (4), 
		fill the internal vector with random
		numbers (5), others = defaulted
		*/

		constexpr vector() = default;

		constexpr explicit vector(value_type value) {
			std::fill(std::begin(_vector), std::end(_vector), value);
		}

		template<std::input_iterator InputIter>
		constexpr vector(InputIter first, InputIter last) {
			static_assert(std::convertible_to<value_type,
				typename std::iterator_traits<InputIter>::value_type>,
				"The type of InputIter must be convertible to the type of Math::Vector");
			ASSERT_SIZE_MISMATCH(std::distance(first, last), Size);

			std::copy(first, last, std::begin(_vector));
		}

		/* [4]
		template<typename First, typename... Rest>
		requires (std::convertible_to<value_type, First>
		and std::conjunction_v<std::is_convertible<First, Rest>...>
		and sizeof...(Rest) + 1 <= Size)
		constexpr explicit vector(const First& first, const Rest... rest)
			: _vector{first, rest...} {}
		*/

		/* Call such as Math::vector<int, 6> vec({3,4,5,2});
		   This constructor was added instead of constructor [4] to avoid
		   a call such as Math::vector<int, 2> vec(2, 3) to call constructor [4],
		   and instead prefer the constructor initializing the vector through random numbers
		   (by passing std::size_t lower, std::size_t higher).
		*/
		template<std::size_t Sz>
		constexpr explicit vector(const T(&arr)[Sz])
		requires (std::convertible_to<value_type, T>)
			: vector(arr, std::make_index_sequence<Sz>{}) {}

	private:
		// Perhaps taking const T2(&arr)[Size] would make this slower in case Size is very big and the caller
		// only passes a small array? Take advantage of that and take a size that represents the actual size of the passed array
		// instead: needs further checks too see if this improves anything
		template <std::size_t... Index>
		constexpr explicit vector(const T(&arr)[sizeof...(Index)],
			std::index_sequence<Index...>)
			: _vector{ arr[Index]... } {}

	public:
		template<typename T2, std::size_t Size2>
		constexpr vector(const vector<T2, Size2>& other)
		requires (Size2 <= Size) {
			std::copy(std::begin(other._vector), std::end(other._vector),
				std::begin(_vector));
		}

		// Currently useless; keep for future changes of the allowed types.
		template<typename T2, size_type Size2>
		constexpr vector(vector<T2, Size2>&& other) /*noexcept (std::is_nothrow_move_constructible_v<T>)*/
		requires (Size2 <= Size) {
			std::move(std::begin(other._vector), std::end(other._vector),
				std::begin(_vector));
		}

	private:

		template<typename Type>
		constexpr auto get_random(Type lower, Type higher) {
			static std::mt19937 mt(std::random_device{}());
			static std::uniform_real_distribution<double> un{ static_cast<double>(lower),
			static_cast<double>(higher) };

			return un(mt);
		}

		constexpr static bool is_near_zero(double val, double epsilon) {
			return val >= 0 && val <= std::abs(epsilon);
		}

	public:

		constexpr explicit vector(T lower, T higher) {
			for (size_type i = 0; i < Size; ++i) {
				_vector[i] = get_random(lower, higher);
			}
		}

		// Initialize the complex vector with random initial real and imaginary values
		constexpr explicit vector(long lower, long higher)
		requires (is_complex<T>::value) {
			using Tp = typename T::value_type;

			for (size_type i = 0; i < Size; ++i) {
				_vector[i] = { get_random<Tp>(lower, higher),
				get_random<Tp>(lower, higher) };
			}
		}

		constexpr vector(const vector& other) = default;
		constexpr vector(vector&& other) /*noexcept*/ = default;
		constexpr vector& operator=(vector&& other) /*noexcept*/ = default;
		constexpr vector& operator=(const vector& other) = default;


		template<typename Function>
		constexpr vector apply_foreach(const Function& function)
		noexcept(noexcept(function)) {

			for (size_type i{}; i < Size; ++i) {
				function(_vector[i]);
			}
			return *this;
		}

		constexpr const T& operator[] (size_type index) const noexcept {
			return _vector[index];
		}

		constexpr T& operator[] (size_type index) noexcept {
			return _vector[index];
		}

		constexpr const T& get_x() const noexcept {
			return _vector[0];
		}

		constexpr const T& get_y() const noexcept
			requires (Size > 1) {
			return _vector[1];
		}

		constexpr const T& get_z() const noexcept
			requires (Size > 2) {
			return _vector[2];
		}

		constexpr const T& get_w() const noexcept
			requires (Size > 3) {
			return _vector[3];
		}

		// Addition operations
		template<typename T2>
		constexpr vector& operator+=(const vector<T2, Size>& rhs) {
			STATICASSERT_COMPLEX_DIFFTYPES(T, T2);

			for (size_type i = 0; const auto & element : rhs._vector) {
				_vector[i++] += element;
			}

			return *this;
		}

		template<underlying_vector_type T2>
		constexpr vector& operator+=(T2 val) {
			std::for_each(std::begin(_vector), std::end(_vector),
				[val](T& current) {
					current += val;
				});

			return *this;
		}

		template<underlying_vector_type T2>
		friend constexpr vector operator+(vector lhs, T2 val) {
			return lhs += val;
		}

		template<underlying_vector_type T2>
		friend constexpr vector operator+(T2 val, vector lhs) {
			return lhs += val;
		}

		template<typename T2>
		friend constexpr vector operator+(vector lhs, const vector<T2, Size>& rhs) {
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
		template<typename T2>
		constexpr vector& substract(const vector<T2, Size>& rhs) {
			STATICASSERT_COMPLEX_DIFFTYPES(T, T2);

			for (size_type i = 0; const auto & element : rhs._vector) {
				_vector[i++] -= element;
			}

			return *this;
		}

		template<underlying_vector_type T2>
		constexpr vector& substract(T2 val) {
			std::for_each(std::begin(_vector), std::end(_vector),
				[val](auto& current) {
					current -= val;
				});

			return *this;
		}

	public:
		template<typename T2>
		constexpr vector& operator-=(const vector<T2, Size>& rhs) {
			return substract(rhs);
		}

		template<typename T2>
		[[deprecated("substracting vector of type unsigned might cause issues!")]]
		constexpr vector& operator-=(const vector<T2, Size>& rhs)
			requires std::is_unsigned<T>::value{
				return substract(rhs);
		}

		template<typename T2>
		friend constexpr vector operator-(vector lhs, const vector<T2, Size>& rhs) {
			return lhs -= rhs;
		}

		template<underlying_vector_type T2>
		constexpr vector& operator-=(T2 val) {
			return substract(val);
		}

		template<underlying_vector_type T2>
		[[deprecated("substracting from an unsigned vector might cause issues!")]]
		constexpr vector& operator-=(T2 val)
			requires std::is_unsigned<T>::value{
				return substract(val);
		}

		template<underlying_vector_type T2>
		friend constexpr vector operator-(vector lhs, T2 val) {
			return lhs -= val;
		}

	private:
		template<underlying_vector_type T2>
		constexpr vector& lambda_multiplicator(T2 lambda) {
			std::for_each(begin(), end(), [lambda](auto& current) {
				current *= lambda;
				});

			return *this;
		}

	public:
		// Multiplication with a Lambda - Scalar multiplication
		template<underlying_vector_type T2>
		constexpr vector& operator*=(T2 lambda) {
			return lambda_multiplicator(lambda);
		}

		template<underlying_vector_type T2>
		[[deprecated("multiplying vector of type unsigned might cause issues!")]]
		constexpr vector& operator*=(T2 lambda)
		requires std::is_unsigned<T>::value{
				return lambda_multiplicator(lambda);
		}

		template<underlying_vector_type T2>
		friend constexpr vector operator*(vector lhs, T2 lambda) {
			return lhs *= lambda;
		}

		template<underlying_vector_type T2>
		friend constexpr vector operator*(T2 lambda, vector lhs) {
			return lhs *= lambda;
		}


		// Cross product
	private:
		template<typename T2>
		constexpr vector& cross_product_internal(const vector<T2, Size>& rhs)
		requires (Size == 3
		and std::is_convertible<T2, value_type>::value) {
			vector temp{ *this };

			if constexpr (is_complex<T>::value) {
				STATICASSERT_COMPLEX_DIFFTYPES(T, T2);

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
		template<typename T2>
		constexpr vector& cross_product(const vector<T2, Size>& rhs) {
			return cross_product_internal(rhs);
		}


		template<typename T2>
		[[deprecated("Multiplication on two vectors of type unsigned might cause issues!")]]
		constexpr vector& cross_product(const vector<T2, Size>& rhs)
		requires std::is_unsigned<T>::value{
				return cross_product_interna(rhs);
		}

			// Not in-place
			template<typename T2>
		friend constexpr vector cross_product(vector lhs, const vector<T2, Size>& rhs) {
			return lhs.cross_product_internal(rhs);
		}

		// Division with a constant/lambda - for precision and correctness, use multiplication instead
		template<underlying_vector_type T2>
		constexpr vector& operator/=(T2 lambda) {
			ASSERT_DIV_BYZERO(lambda);

			std::for_each(std::begin(_vector), std::end(_vector),
				[lambda](auto& current) {
					current /= lambda;
				});

			return *this;
		}

		template<underlying_vector_type T2>
		friend constexpr vector operator/(vector lhs, T2 lambda) {
			return lhs /= lambda;
		}

		template<underlying_vector_type T2>
		constexpr vector& operator%=(T2 lambda)
		requires (std::is_integral_v<T>
		and std::is_integral_v<T2>
		and not is_complex<T>::value) {
			ASSERT_DIV_BYZERO(lambda);

			std::for_each(std::begin(_vector), std::end(_vector),
				[lambda](auto& current) {
					current %= lambda;
				});

			return *this;
		}


		constexpr void reset() noexcept {
			std::fill(std::begin(_vector), std::end(_vector), T{});
		}

		template<underlying_vector_type T1>
		constexpr auto inner_product(const vector<T1, Size>& other) const {
			return std::inner_product(std::begin(_vector), std::end(_vector),
				std::begin(other._vector), T{});
		}

		friend constexpr auto scalar_triple_product(const vector& first,
			const vector& other2, const vector& other3)
		requires (Size == 3
		and not is_complex<T>::value) {
			vector temp{ first };
			return other3.inner_product(temp.cross_product(other2));
		}

		// Not in place
		friend constexpr vector vector_triple_product(const vector& first, const vector& other2,
			const vector& other3)
		requires (Size == 3
		and not is_complex<T>::value) {
			vector temp{ first };
			vector temp3{ other3 };
			return temp3.cross_product(temp.cross_product(other2));
		}

		friend constexpr bool are_coplanar(const vector& first, const vector& other2,
			const vector& other3, double epsilon = 1E-6)
		requires (Size == 3
		and not is_complex<T>::value) {
			auto result = scalar_triple_product(first, other2, other3);
			return is_near_zero(result, epsilon);
		}


		// Calculates projection of other to this, then stores the result in this (inplace)
		template<typename T2>
		constexpr vector& vector_projection_from(const vector<T2, Size>& other) {
			T denominator = inner_product(*this);
			// a null vector results in a scalar product of 0, thus a denominator of 0
			// todo: decide whether this is the appropriate way to proceed, or whether another result can be given (eg. using limits)
			ASSERT_DIV_BYZERO(denominator);
			*this *= (inner_product(other) / denominator);

			return *this;
		}

		// Return direction of a 2D vector. The vector components must be cartesian coordinates. x will return (angle)x, 
		// y returns (angle)y. 
		constexpr double direction_radiants_y() const
		requires (Size == 2
		and not is_complex<T>::value) {
			return std::atan2(_vector[0], _vector[1]); // (x / y)
		}

		constexpr double direction_radiants_x() const
		requires (Size == 2
		and not is_complex<T>::value) {
			return std::atan2(_vector[1], _vector[0]); // (y / x)
		}

		constexpr double direction_degrees_y() const {
			return direction_radiants_y() * 180.0 / std::numbers::pi;
		}

		constexpr double direction_degrees_x() const {
			return direction_radiants_x() * 180.0 / std::numbers::pi;
		}


		// Direction cosines of 3D vectors.
		constexpr double direction_cosine(Cos type) const
		requires (Size == 3
		and not is_complex<T>::value) {
			double denominator = magnitude();
			ASSERT_DIV_BYZERO(denominator);

			return static_cast<double>(_vector[static_cast<size_type>(type)] / denominator);
		}

		// Direction cosines of N-D vectors (with indexes)
		constexpr double direction_cosine(std::size_t index) const
		requires (not is_complex<T>::value) {
			double denominator = magnitude();
			ASSERT_DIV_BYZERO(denominator);

			return static_cast<double>(_vector[index] / denominator);
		}

		constexpr double direction_angle(Cos type) const
		requires (Size == 2
		and not is_complex<T>::value) {
			return std::acos(direction_cosine(type));
		}

		constexpr vector& normalize() {
			double denominator = magnitude();
			ASSERT_DIV_BYZERO(denominator);
			*this *= (1 / denominator);
			return *this;
		}

		// Return || magnitude || of this vector
		constexpr double magnitude() const {
			double sum = std::accumulate(begin(), end(), T{},
				[](auto internal_sum, const auto& element) {
					return internal_sum + std::pow(element, 2);
				});

			return std::sqrt(sum);
		}

		// Computes the norm for a complex vector
		constexpr double magnitude() const
		requires (is_complex<T>::value) {
			// The norm of a complex vector has a different definition than that of a vector with R components.
			double sum = std::accumulate(std::begin(_vector), std::end(_vector), 0,
				[](auto sum, const auto& element) {
					return sum + std::pow(std::abs(element.real()), 2)
						+ std::pow(std::abs(element.imag()), 2);
				});

			return std::sqrt(sum);
		}

		// Vector Parallelism Condition
		// 3 dimensions case: checks whether the cross product is near 0
		template<typename T2>
		friend constexpr bool are_parallel(const vector& first, const vector<T2, Size>& other, double epsilon = 1E-6)
		requires (Size == 3
		and not is_complex<T>::value
			and not is_complex<T2>::value)
		{
			vector temp_cross = first;
			vector temp = temp_cross.cross_product(other);
			// Two vectors are parallel in 3D only if their cross product result is near zero.
			return temp[0] >= 0 && temp[0] <= std::abs(epsilon)
				&& temp[1] >= 0 && temp[1] <= std::abs(epsilon)
				&& temp[2] >= 0 && temp[2] <= std::abs(epsilon);
		}


		// Other dimensions: scalar product method. Parallelism condition if result's near 0 (approx 1E-6)
		template<typename T2>
		friend constexpr bool are_parallel(const vector& first, const vector<T2, Size>& other, double epsilon = 1E-6)
		requires (not is_complex<T>::value
		and not is_complex<T2>::value) {
			auto sum_and_pow = [](auto sum, const auto& element) {
				return sum + std::pow(element, 2);
			};

			T inner_product = first.inner_product(other);
			double lhs = std::pow(inner_product, 2);
			double rhs1 = std::accumulate(std::begin(first._vector), std::end(first._vector),
				0, sum_and_pow);
			double rhs2 = std::accumulate(std::begin(other._vector), std::end(other._vector),
				0, sum_and_pow);
			return is_near_zero(std::abs(rhs1 * rhs2 - lhs), epsilon);
		}

		// Perpendicular conditions
		template<typename T2>
		friend constexpr bool are_perpendicular(const vector& first, const vector<T2, Size>& other, double epsilon = 1E-6)
		requires (not is_complex<T>::value
		and not is_complex<T2>::value) {
			auto inner_product = first.inner_product(other);
			return is_near_zero(inner_product, epsilon);
		}

		// Angle between two vectors (in radiants)
		// Todo: Add support for complex vectors
		template<typename T2>
		constexpr double angle_between_radiants(const vector<T2, Size>& other)
			requires (not is_complex<T>::value) {
			double inner_prod = inner_product(other);
			double magnitude_mult = magnitude() * other.magnitude();
			ASSERT_DIV_BYZERO(magnitude_mult);

			return std::acos(inner_prod / magnitude_mult);
		}

		template<typename T2>
		constexpr double angle_between_degrees(const vector<T2, Size>& other) {
			return angle_between_radiants(other) * 180 / std::numbers::pi;
		}

		constexpr bool is_empty() const {
			return (Size == 0 || std::all_of(std::begin(_vector), std::end(_vector),
				[](value_type i) { return i == value_type{}; }));
		}

		auto operator<=>(const vector&) const = default;

		friend std::ostream& operator<< (std::ostream& stream, const vector& rhs) {
			for (auto x : rhs._vector) {
				stream << x << ' ';
			}
			stream << '\n';

			return stream;
		}


		consteval value_type size() const noexcept {
			return Size;
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

	};



	// Not in place counterparts
	template<underlying_vector_type T, std::size_t Size, typename T2>
	constexpr auto lambda_multiplication(const vector<T, Size>& vec, T2 lambda) {
		using Type = decltype(std::declval<T>()* std::declval<T2>());
		vector<Type, Size> temp(vec);

		return temp.lambda_multiplicator(lambda);
	}


	template<typename T, std::size_t Size>
	constexpr vector<T, Size> normalize(const vector<T, Size>& other) {
		vector<T, Size> copy(other);
		copy.normalize();

		return copy;
	}



	// Deduction guides
	// [1] normal constructor calls
	template<typename T2, std::same_as<T2>...Args>
	vector(T2, Args...)->vector<T2, sizeof...(Args) + 1>;

	// [2] parameter pack constructor calls
	template<typename T, std::size_t Sz>
	vector(const T(&)[Sz])->vector<T, Sz>;

}

#endif
