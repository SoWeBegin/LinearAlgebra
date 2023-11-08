#include "pch.h"
#include <scoped_allocator>

#define ALLOW_IMPLICIT_CONVERSIONS
#include "../LinearAlgebraGit/include/vector.h"


TEST(DynamicExtent, AllocatorParameter) {
	MathLbr::vector<int, 4> non_dynamic_vec;
	MathLbr::vector<int> dynamic_vec_no_alloc;
	MathLbr::vector<int, MathLbr::dynamic_extent, std::scoped_allocator_adaptor<std::allocator<int>>> dynamic_vec_with_alloc;
	EXPECT_EQ(typeid(decltype(dynamic_vec_with_alloc)::allocator_type), typeid(std::scoped_allocator_adaptor<std::allocator<int>>));
}

// Only tests for disabled dynamic extent, because this constructor exists only in such cases
// Constructor number 1
TEST(Constructors, Contructor1) {
	// 1. use correct type and check resulting vector
	MathLbr::vector<int, 3> vec(4);
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(vec[i], 4);
	}
	EXPECT_EQ(3ULL, vec.size());

	// 2. use different (convertible-to) type, and check resulting vector
	MathLbr::vector<int, 3> vec1(4.6);
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(vec1[i], 4);
	}
	EXPECT_EQ(3ULL, vec1.size());

	// 3. use different (not-convertible-to) type, and check whether it fails.  <-> compilation fail
	// MathLbr::vector<double, 3>(std::complex<double>(4.5)));  // ok, fails to compile

	// 4. check that the constructor is NOT explicit (it should be because implicit conversions are allowed)
	MathLbr::vector<int, 3> vec2 = 4.6; // fails if explicit
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(vec1[i], 4);
	}
	EXPECT_EQ(3ULL, vec1.size());
}

// Only tests for enabled dynamic extent, because this constructor exists only in such cases
// Constructor number 2
TEST(Constructors, Constructor2) {
	// 1. use correct size and check correct outcome
	MathLbr::vector<double> vec(4);
	for (int i = 0; i < 4; ++i) {
		EXPECT_EQ(vec[i], double());
	}
	EXPECT_EQ(4ULL, vec.size());

	// 2. check that the constructor is NOT explicit
	MathLbr::vector<int> vec1 = 5;
}

// Constructor number 3
TEST(Constructors, Constructor3) {
	// DYNAMIC EXTENT DISABLED
	// 1. Check that the assertion is triggered when internalSize < passedSize
	int arr[5] = { 1, 2, 3, 4, 5 };
	auto lambda = [&] {MathLbr::vector<std::complex<int>, 4> foo(std::begin(arr), std::end(arr)); };
	// Needs to assert: actual size [4] < passed size [5]
	EXPECT_DEATH(lambda(), "Size mismatch: Size of LHS vector must be greater than size of RHS vector!");

	// 2. Check that when correctly initialized, the elements are correct 
	MathLbr::vector<std::complex<int>, 7> myVec(std::begin(arr), std::end(arr));
	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(myVec[i], arr[i]);
		// Remaining elements have uninitialized values
	}
	EXPECT_EQ(myVec.size(), 7); // Check correct size

	// 3. Check that non-possible implicit conversions trigger a compile error
	std::complex<double> arr2[3] = { {2,2}, {2,3}, {3,3} };
	// MathLbr::vector<double, 4>(std::begin(arr2), std::end(arr2)); // ok, fails to compile


	// DYNAMIC EXTENT ENABLED
	// 1. Check that all elements are correct and the final size is also correct
	MathLbr::vector<double> vec(std::begin(arr), std::end(arr));
	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(vec[i], arr[i]);
	}
	EXPECT_EQ(vec.size(), 5);

	// 2. Check that impossible implicit conversions trigger an error
	// MathLbr::vector<double>(std::begin(arr2), std::end(arr2)); // ok, fails to compile
}

// Constructor number 4
TEST(Constructors, Constructor4) {
	/** DYNAMIC EXTENT DISABLED **/

	// 1. Check that the size of the container is greater than the number of passed elements, otherwise should fail at compile time
	// MathLbr::vector<int, 3> vec{ {1,2,3,4} }; // ok, fails at compile time

	// 2. Check that the elements are correctly initialized
	MathLbr::vector<int, 4> vec1{ {1,2,3} };
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(vec1[i], i + 1);
	}
	EXPECT_EQ(vec1[3], int{}); // Last element should be zero initialized according to aggregate initialization rules

	// 3. Check that possible implicit conversions can happen
	MathLbr::vector<std::complex<double>, 3> vec2{ {3.3, 3., 2.} };

	// 4. Check that impossible implicit conversions cannot happen
	//MathLbr::vector<double, 1> vec3{ { std::complex<double>{1,2}} }; // ok, fails to compile


	/** DYNAMIC EXTENT ENABLED **/
	// 1. Check correct initialization and size of the container
	MathLbr::vector<int> vec4{ { 1.,2.,3.,4.,5.,6.} }; // also test possible implicit conversions
	for (int val = 1; auto i : vec4) {
		EXPECT_EQ(i, val++);
	}
	EXPECT_EQ(vec4.size(), 6);

	// Impossible implicit conversions tested above, same constructor

	// Check that the constructor is not explicit
	MathLbr::vector<int> vec5 = { { 1,2,3,4,5} };
	MathLbr::vector<std::complex<double>> vec6 = { {std::complex<double>{3.,4.}, std::complex<double>{2.,2.}} };
}

// Constructor number 5
TEST(Constructors, Constructor5) {
	/** DYNAMIC EXTENT DISABLED **/

	// 1. Test compile-time failures when other.size() > current.size()
	MathLbr::vector<int, 3> otherVector{ {1,2,3} };
	// MathLbr::vector<int, 2> currentVector(otherVector); --> ok, fails

	// 2. Test run-time assert when other.size() > current.size()
	MathLbr::vector<int> otherVector2{ {1,2,3} };
	auto lambda = [&] { MathLbr::vector<int, 2> foo(otherVector2); };
	EXPECT_DEATH(lambda(), "Size mismatch: Size of LHS vector must be greater than size of RHS vector!");

	// 3. Test that all the elements are correctly initialized, assuming the correct sizes are given
	MathLbr::vector<int, 10> v(otherVector); // the last 7 elements have uninitialized values
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(otherVector[i], v[i]);
	}
	EXPECT_EQ(v.size(), 10); // test correct size

	// 3. Test types that are convertible to each other effectively work
	MathLbr::vector<double> otherVector3{ {1., 2., 3.} };
	MathLbr::vector<int, 3> currentVector3(otherVector3);

	// 4. Test types that are NOT convertible to NOT work
	MathLbr::vector<std::complex<double>> otherVector4{ {std::complex<double>(2), std::complex<double>(2., 3)} };
	// MathLbr::vector<double, 10> currentVector4(otherVector4); --> ok, fails

	// 5. Test that the constructor is NOT explicit 
	MathLbr::vector<int, 3> v2 = otherVector2;


	/** DYNAMIC EXTENT ENABLED **/
	// 1. Check that possibly convertible types are effectively implicitly convertible.
	MathLbr::vector<int, 3> en_vec{ {1,2,3} };
	MathLbr::vector<double> en_vec_curr(en_vec);

	// 2. Check that not convertible types are not implicitly convertible
	MathLbr::vector<std::complex<double>> otherVector5{ {std::complex<double>(2), std::complex<double>(2., 3)} };
	//MathLbr::vector<double> currentVector4(otherVector5); --> ok, fails

	// 2. Check that the constructor is not explicit
	MathLbr::vector<double> en_vec_curr_2 = en_vec;

	// 3. Check that the elements are correctly initialized, and the size is correct
	for (int idx = 0; auto i : en_vec_curr) {
		EXPECT_EQ(i, en_vec[idx++]);
	}
	EXPECT_EQ(en_vec_curr.size(), en_vec.size());
}

// Constructor number 6
TEST(Constructors, Constructor6) {
	/** DYNAMIC EXTENT DISABLED **/

	// 1. Test compile-time failures when other.size() > current.size()
	MathLbr::vector<int, 3> otherVector{ {1,2,3} };
	// MathLbr::vector<int, 2> currentVector(otherVector); --> ok, fails

	// 2. Test run-time assert when other.size() > current.size()
	MathLbr::vector<int> otherVector2{ {1,2,3} };
	auto lambda = [&] { MathLbr::vector<int, 2> foo(std::move(otherVector2)); };
	EXPECT_DEATH(lambda(), "Size mismatch: Size of LHS vector must be greater than size of RHS vector!");

	// 3. Test that all the elements are correctly initialized, assuming the correct sizes are given
	MathLbr::vector<int, 10> v(std::move(otherVector)); // the last 7 elements have uninitialized values
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(otherVector[i], v[i]);
	}
	EXPECT_EQ(v.size(), 10); // test correct size

	// 3. Test types that are convertible to each other effectively work
	MathLbr::vector<double> otherVector3{ {1., 2., 3.} };
	MathLbr::vector<int, 3> currentVector3(std::move(otherVector3));

	// 4. Test types that are NOT convertible to NOT work
	MathLbr::vector<std::complex<double>> otherVector4{ {std::complex<double>(2), std::complex<double>(2., 3)} };
	// MathLbr::vector<double, 10> currentVector4(std::move(otherVector4)); --> ok, fails

	// 5. Test that the constructor is NOT explicit 
	MathLbr::vector<int, 3> v2 = std::move(otherVector2);


	/** DYNAMIC EXTENT ENABLED **/
	// 1. Check that possibly convertible types are effectively implicitly convertible.
	MathLbr::vector<int, 3> en_vec{ {1,2,3} };
	MathLbr::vector<double> en_vec_curr(std::move(en_vec));

	// 2. Check that not convertible types are not implicitly convertible
	MathLbr::vector<std::complex<double>> otherVector5{ {std::complex<double>(2), std::complex<double>(2., 3)} };
	// MathLbr::vector<double> currentVector5(std::move(otherVector4)); --> ok, fails

	// 2. Check that the constructor is not explicit
	MathLbr::vector<double> en_vec_curr_2 = std::move(en_vec);

	// 3. Check that the elements are correctly initialized, and the size is correct
	for (int idx = 0; auto i : en_vec_curr) {
		EXPECT_EQ(i, en_vec[idx++]);
	}
	EXPECT_EQ(en_vec_curr.size(), en_vec.size());
}

TEST(Constructors, Constructor7) {
	/** DYNAMIC EXTENT DISABLED **/

	// 1. Test that the constructor is not explicit
	MathLbr::vector<int, 7> v1 = { 1, 5 };
	MathLbr::vector<std::complex<double>, 2> v2 = { 3.4, 3.4 };

	// 2. Test that implicit conversions are possible if they're allowed by the user with #define
	MathLbr::vector<int, 7> v22(3.4, 7.6);
	MathLbr::vector<std::complex<double>, 8> v3(3.4, 3.9);

	// 3. Test that implicit conversions are not possible if there's no implicit conversion sequence
	//MathLbr::vector<double, 8> v4(std::complex<double>(4, 4), std::complex<double>(5, 5)); --> ok, fails, complex not convertible to double


	/** DYNAMIC EXTENT DISABLED **/
	// 1. Test that the constructor is not explicit
	MathLbr::vector<int> v5 = { 1, 5, 7 };
	MathLbr::vector<std::complex<double>> v6 = { 3.4, 3.4, 2 };

	// 2. Test that the implicit conversions are possible if they're allowed by the user with #define
	MathLbr::vector<int> v7(3.4, 7.6, 7);
	MathLbr::vector<std::complex<double>> v8(3.4, 3.9, 8);

	// Check all sizes
	EXPECT_EQ(v1.size(), 7);
	EXPECT_EQ(v2.size(), 2);
	EXPECT_EQ(v3.size(), 8);
	EXPECT_EQ(v5.size(), 7);
	EXPECT_EQ(v6.size(), 2);
}

TEST(OperatorOverloading, Sum) {
	/* DYNAMIC EXTENT DISABLED */

	// Case 1: vector += value
	// 1. Test correctness with no implicit conversions happening
	MathLbr::vector<int, 4> vec0{ {1,3,2,4} };
	vec0 += 5;
	EXPECT_EQ(vec0[0], 6);
	EXPECT_EQ(vec0[1], 8);
	EXPECT_EQ(vec0[2], 7);
	EXPECT_EQ(vec0[3], 9);

	// 2. Test that implicit conversions are allowed, and that the results are correct
	MathLbr::vector<int, 3> vec1{ {1,2,3} };
	vec1 += 5.6;
	EXPECT_EQ(vec1[0], 6);
	EXPECT_EQ(vec1[1], 7);
	EXPECT_EQ(vec1[2], 8);

	// Case 2: vector += vector
	// 1. Check correct sum with different sizes of LHS
	vec0 += vec1;
	EXPECT_EQ(vec0[0], 12);
	EXPECT_EQ(vec0[1], 15);
	EXPECT_EQ(vec0[2], 15);
	EXPECT_EQ(vec0[3], 9);

	// 2. Check correct sum with different sizes of RHS, and different underlying types
	MathLbr::vector<int, 3> lhs{ {1,2,3} };
	MathLbr::vector<double, 4> rhs{ {1.,2.8,3.,4.3} };
	lhs += rhs;
	EXPECT_EQ(lhs[0], 2);
	EXPECT_EQ(lhs[1], 4);
	EXPECT_EQ(lhs[2], 6);

	/* DYNAMIC EXTENT ENABLED */
	// Case 1: vector += value
	MathLbr::vector<int> vec00{ {1,3,2,4} };
	vec00 += 5;
	EXPECT_EQ(vec00[0], 6);
	EXPECT_EQ(vec00[1], 8);
	EXPECT_EQ(vec00[2], 7);
	EXPECT_EQ(vec00[3], 9);

	// 2. Test that implicit conversions are allowed, and that the results are correct
	MathLbr::vector<int, 3> vec11{ {1,2,3} };
	vec11 += 5.6;
	EXPECT_EQ(vec11[0], 6);
	EXPECT_EQ(vec11[1], 7);
	EXPECT_EQ(vec11[2], 8);

	// Case 2: vector += vector
	// 1. Check correct sum with different sizes of LHS
	vec00 += vec11;
	EXPECT_EQ(vec00[0], 12);
	EXPECT_EQ(vec00[1], 15);
	EXPECT_EQ(vec00[2], 15);
	EXPECT_EQ(vec00[3], 9);

	// 2. Check correct sum with different sizes of RHS, and different underlying types
	MathLbr::vector<int> lhss{ {1,2,3} };
	MathLbr::vector<double> rhss{ {1.,2.8,3.,4.3} };
	lhss += rhss;
	EXPECT_EQ(lhss[0], 2);
	EXPECT_EQ(lhss[1], 4);
	EXPECT_EQ(lhss[2], 6);

	/* MISCELLANEOUS */
	MathLbr::vector<int, 4> a{ {1,2,3,4} };
	MathLbr::vector<int, 5> b{ {1,1,1,1,2} };
	MathLbr::vector<int, 4> result1{ {2, 3, 4, 5} };
	EXPECT_EQ(a + b, result1);
	MathLbr::vector<int, 5> result2{ {2,3,4,5,2} };
	EXPECT_EQ(b + a, result2);

	EXPECT_EQ(a += b, result1); // a = {2, 3, 4, 5}
	MathLbr::vector<int, 5> result3{ {3, 4, 5, 6, 2} };
	EXPECT_EQ(b += a, result3);

	// Dynamic
	MathLbr::vector<int> a1{ {1,2,3,4} };
	MathLbr::vector<int> b1{ {1,1,1,1,2} };
	MathLbr::vector<int> result11{ {2, 3, 4, 5} };
	EXPECT_EQ(a1 + b1, result11);
	MathLbr::vector<int> result22{ {2,3,4,5,2} };
	EXPECT_EQ(b1 + a1, result22);

	EXPECT_EQ(a1 += b1, result11); // a = {2, 3, 4, 5}
	MathLbr::vector<int> result33{ {3, 4, 5, 6, 2} };
	EXPECT_EQ(b1 += a1, result33);

	// Dynamic and non dynamic
	MathLbr::vector<int> a2{ {1,2,3,4,5} };
	MathLbr::vector<int, 2> b2{ {1,2} };
	MathLbr::vector<int> result111{ {2, 4, 3, 4, 5} };
	EXPECT_EQ(a2 + b2, result111);
	MathLbr::vector<int, 2> result222{ {2,4} };
	EXPECT_EQ(b2 + a2, result222);

	// Complex
	MathLbr::vector<std::complex<double>> complex{ {{1.3,3.},{2.,2.3}} };
	MathLbr::vector<std::complex<double>, 4> complex2{ {{1.3,3.},{2.,2.3},{3.,3.},{4.,4.}} };
	MathLbr::vector<std::complex<double>, 4> expected1{ {{2.6,6.},{4.,4.6},{3.,3.},{4.,4.}} };
	MathLbr::vector<std::complex<double>> expected2{ {{2.6,6.},{4.,4.6}} };
	EXPECT_EQ(complex2 + complex, expected1);
	EXPECT_EQ(complex + complex2, expected2);
}

TEST(OperatorOverloading, Negation) {
	MathLbr::vector<int> v{ {1,2,3,4,5} };
	MathLbr::vector<int> v_expect{ {-1,-2,-3,-4,-5} };
	v = -v;
	for (int idx = 0; auto i : v) {
		EXPECT_EQ(i, v_expect[idx++]);
	}

	MathLbr::vector<std::complex<double>> v2{ {{1.2, 1.3}, {1.3, 1.4}} };
	MathLbr::vector<std::complex<double>> v2_expect{ {{-1.2, -1.3}, {-1.3, -1.4}} };
	v2 = -v2;
	for (int idx = 0; auto i : v2) {
		EXPECT_EQ(i, v2_expect[idx++]);
	}
}

TEST(OperatorOverloading, Subtraction) {
	/* DYNAMIC EXTENT DISABLED */

	// Case 1: vector -= value
	// 1. Test correctness with no implicit conversions happening
	MathLbr::vector<int, 4> vec0{ {1,3,2,4} };
	vec0 -= 5;
	EXPECT_EQ(vec0[0], -4);
	EXPECT_EQ(vec0[1], -2);
	EXPECT_EQ(vec0[2], -3);
	EXPECT_EQ(vec0[3], -1);

	// 2. Test that implicit conversions are allowed, and that the results are correct
	MathLbr::vector<int, 3> vec1{ {1,2,3} };
	vec1 -= 5.6;
	EXPECT_EQ(vec1[0], -4);
	EXPECT_EQ(vec1[1], -3);
	EXPECT_EQ(vec1[2], -2);

	// Case 2: vector -= vector
	// 1. Check correct substraction with different sizes of LHS
	vec0 -= vec1;
	EXPECT_EQ(vec0[0], 0);
	EXPECT_EQ(vec0[1], 1);
	EXPECT_EQ(vec0[2], -1);
	EXPECT_EQ(vec0[3], -1);

	// 2. Check correct substraction with different sizes of RHS, and different underlying types
	MathLbr::vector<int, 3> lhs{ {1,2,3} };
	MathLbr::vector<double, 4> rhs{ {1.,2.8,3.,4.3} };
	lhs -= rhs;
	EXPECT_EQ(lhs[0], 0);
	EXPECT_EQ(lhs[1], 0);
	EXPECT_EQ(lhs[2], 0);

	/* DYNAMIC EXTENT ENABLED */
	// Case 1: vector -= value
	MathLbr::vector<int> vec00{ {1,3,2,4} };
	vec00 -= 5;
	EXPECT_EQ(vec00[0], -4);
	EXPECT_EQ(vec00[1], -2);
	EXPECT_EQ(vec00[2], -3);
	EXPECT_EQ(vec00[3], -1);

	// 2. Test that implicit conversions are allowed, and that the results are correct
	MathLbr::vector<int, 3> vec11{ {1,2,3} };
	vec11 -= 5.6;
	EXPECT_EQ(vec11[0], -4);
	EXPECT_EQ(vec11[1], -3);
	EXPECT_EQ(vec11[2], -2);

	// Case 2: vector -= vector
	// 1. Check correct substraction with different sizes of LHS
	vec00 -= vec11;
	EXPECT_EQ(vec00[0], 0);
	EXPECT_EQ(vec00[1], 1);
	EXPECT_EQ(vec00[2], -1);
	EXPECT_EQ(vec00[3], -1);

	// 2. Check correct substraction  with different sizes of RHS, and different underlying types
	MathLbr::vector<int> lhss{ {1,2,3} };
	MathLbr::vector<double> rhss{ {1.,2.8,3.,4.3} };
	lhss -= rhss;
	EXPECT_EQ(lhss[0], 0);
	EXPECT_EQ(lhss[1], 0);
	EXPECT_EQ(lhss[2], 0);


	/* MISCELLANEOUS */
	MathLbr::vector<int, 4> a{ {1,2,3,4} };
	MathLbr::vector<int, 5> b{ {1,1,1,1,2} };
	MathLbr::vector<int, 4> result1{ {0, 1, 2, 3} };
	EXPECT_EQ(a - b, result1);
	MathLbr::vector<int, 5> result2{ {0,-1,-2,-3,2} };
	EXPECT_EQ(b - a, result2);

	EXPECT_EQ(a -= b, result1); // a = {0, 1, 2, 3}
	MathLbr::vector<int, 5> result3{ {1, 0, -1, -2, 2} };
	EXPECT_EQ(b -= a, result3);

	// Dynamic
	MathLbr::vector<int> a1{ {1,2,3,4} };
	MathLbr::vector<int> b1{ {1,1,1,1,2} };
	MathLbr::vector<int> result11{ {0, 1, 2, 3} };
	EXPECT_EQ(a1 - b1, result11);
	MathLbr::vector<int> result22{ {0,-1,-2,-3,2} };
	EXPECT_EQ(b1 - a1, result22);

	EXPECT_EQ(a1 -= b1, result11); // a = {0, 1, 2, 3}
	MathLbr::vector<int> result33{ {1, 0, -1, -2, 2} };
	EXPECT_EQ(b1 -= a1, result33);

	// Dynamic and non dynamic
	MathLbr::vector<int> a2{ {1,2,3,4,5} };
	MathLbr::vector<int, 2> b2{ {1,2} };
	MathLbr::vector<int> result111{ {0, 0, 3, 4, 5} };
	EXPECT_EQ(a2 - b2, result111);
	MathLbr::vector<int, 2> result222{ {0,0} };
	EXPECT_EQ(b2 - a2, result222);

	// Complex
	MathLbr::vector<std::complex<double>> complex{ {{1.3,3},{2,2.3}} };
	MathLbr::vector<std::complex<double>, 4> complex2{ {{1.3,3},{2,2.3},{3,3},{4,4}} };
	MathLbr::vector<std::complex<double>, 4> expected1{ {{0,0},{0,0},{3,3},{4,4}} };
	MathLbr::vector<std::complex<double>> expected2{ {{0,0},{0,0}} };
	EXPECT_EQ(complex2 - complex, expected1);
	EXPECT_EQ(complex - complex2, expected2);
}

TEST(OperatorOverloading, Multiplication) {
	/* DYNAMIC EXTENT DISABLED */

	// 1. Test correctness with no implicit conversions happening
	MathLbr::vector<int, 4> vec0{ {1,3,2,4} };
	vec0 *= 5;
	EXPECT_EQ(vec0[0], 5);
	EXPECT_EQ(vec0[1], 15);
	EXPECT_EQ(vec0[2], 10);
	EXPECT_EQ(vec0[3], 20);

	// 2. Test that implicit conversions are allowed, and that the results are correct
	MathLbr::vector<int, 3> vec1{ {1,2,3} };
	vec1 *= 5.6;
	EXPECT_EQ(vec1[0], 5);
	EXPECT_EQ(vec1[1], 11);
	EXPECT_EQ(vec1[2], 16);

	// Case 2: lambda * vec
	vec1 = 2 * vec1;
	EXPECT_EQ(vec1[0], 10);
	EXPECT_EQ(vec1[1], 22);
	EXPECT_EQ(vec1[2], 32);

	// case 3: vec * lambda
	vec1 = vec1 * 2;
	EXPECT_EQ(vec1[0], 20);
	EXPECT_EQ(vec1[1], 44);
	EXPECT_EQ(vec1[2], 64);

	/* DYNAMIC EXTENT ENABLED */
	// 1. Test correctness with no implicit conversions happening
	MathLbr::vector<int> vec00{ {1,3,2,4} };
	vec00 *= 5;
	EXPECT_EQ(vec00[0], 5);
	EXPECT_EQ(vec00[1], 15);
	EXPECT_EQ(vec00[2], 10);
	EXPECT_EQ(vec00[3], 20);

	// 2. Test that implicit conversions are allowed, and that the results are correct
	MathLbr::vector<int> vec11{ {1,2,3} };
	vec11 *= 5.6;
	EXPECT_EQ(vec11[0], 5);
	EXPECT_EQ(vec11[1], 11);
	EXPECT_EQ(vec11[2], 16);

	// Case 2: lambda * vec
	vec11 = 2 * vec11;
	EXPECT_EQ(vec11[0], 10);
	EXPECT_EQ(vec11[1], 22);
	EXPECT_EQ(vec11[2], 32);

	// case 3: vec * lambda
	vec11 = vec11 * 2;
	EXPECT_EQ(vec11[0], 20);
	EXPECT_EQ(vec11[1], 44);
	EXPECT_EQ(vec11[2], 64);
}


TEST(OperatorOverloading, Division) {
	MathLbr::vector<int> a{ {4,6,8} };
	MathLbr::vector<double> b{ {5, 6, 7 } };
	a /= 2;
	MathLbr::vector<int> expected{ {2, 3, 4} };
	EXPECT_EQ(a, expected);

	MathLbr::vector<double> expected2{ { 2.5, 3, 3.5} };
	MathLbr::vector<double> c = b / 2;
	EXPECT_EQ(c, expected2);
}


TEST(Modifiers, CrossProduct) {
	/* DYNAMIC EXTENT DISABLED */
	// 1. Check no implicit conversion correctness 
	MathLbr::vector<int, 3> lhs{ {1,2,3} };
	MathLbr::vector<int, 3> rhs{ {1,3,3} };
	MathLbr::vector<int, 3> result0{ {-3, 0, 1} };
	lhs.cross_product(rhs);
	for (int idx = 0; auto i : lhs) EXPECT_EQ(i, result0[idx++]);

	// 2. Check implicit conversions correctness
	MathLbr::vector<int, 3> lhs1{ {1,2,3} };
	MathLbr::vector<double, 3> rhs1{ {2.3, 2.1, 3.} };
	MathLbr::vector<int, 3> result1{ {0, 3, -2} };
	lhs1.cross_product(rhs1);
	for (int idx = 0; auto i : lhs1) EXPECT_EQ(i, result1[idx++]);

	// 3. Check that if both are compile time, Size == 3 for both lhs and rhs throws a compile error
	MathLbr::vector<int, 5> lhs2{ {1,2,3} };
	MathLbr::vector<int, 5> rhs2(5);
	// lhs2.cross_product(rhs2); --> compile time fails, ok

	// 4. Check complex internal type correctness
	MathLbr::vector<std::complex<double>, 3> lhs3{ { {1.2, 2.}, { 1.3, 3.3 }, { 4.2, 4. } } };
	MathLbr::vector<std::complex<double>, 3> rhs3{ { {1.4, 2.1}, { 1., 3.2 }, { 4.2, 4. } } };

}


TEST(Modifiers, InnerProduct) {
	// Non complex case
	MathLbr::vector<double> a{ {3, 4.9, 1} };
	MathLbr::vector<double> b{ {5.6, 3.3, 2} };
	EXPECT_EQ(34.97, MathLbr::Vector::inner_product(a, b));

	// Test correct returned value's type
	MathLbr::vector<float> a2{ {3, 4.9, 1} };
	MathLbr::vector<int> b2{ {5, 3, 2} };
	EXPECT_FLOAT_EQ(31.7, MathLbr::Vector::inner_product(a2, b2));
	EXPECT_EQ(typeid(float), typeid(MathLbr::Vector::inner_product(a2, b2)));

	// Test that lhs.size == rhs.size for non dynamic and dynamic
	MathLbr::vector<int, 4> a3{ {1,2,3,4} };
	MathLbr::vector<int, 3> b3{ {1,2,3} };
	// MathLbr::Vector::inner_product(a3, b3);  -> ok, fails to compile

	MathLbr::vector<int> a4{ {1,2,3,4} };
	MathLbr::vector<int> b4{ {1,2,3} };
	EXPECT_DEATH(MathLbr::Vector::inner_product(a4, b4),
		"Dynamic sizes of both lhs and rhs must be equal to perform certain operations!");

	// Complex case
	MathLbr::vector<std::complex<int>> c{ {{2,3}, {7,3}} };
	MathLbr::vector<std::complex<int>> d{ {{1,2},{2,2}} };
	std::complex<int> cmp{ 28,9 };
	EXPECT_EQ(cmp, MathLbr::Vector::inner_product<MathLbr::ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT>(c, d));

	// Assert inequality of runtime sizes
	MathLbr::vector<std::complex<int>> c1{ {{2,3}, {7,3}} };
	MathLbr::vector<std::complex<int>> d2{ {{1,2},{2,2}, {3,3}} };
	EXPECT_DEATH(MathLbr::Vector::inner_product(c1, d2),
		"Dynamic sizes of both lhs and rhs must be equal to perform certain operations!");
}

TEST(Modifiers, complexDotProduct) {
	EXPECT_EQ(MathLbr::Vector::complex_inner_product(std::complex<int>(3, -4), std::complex<double>(-4, 3)), -24);
}


// complex<int> * complex<double> doesn't work?!
TEST(Modifiers, tripleScalarProduct) {
	MathLbr::vector<int> a{ {1,2,3} };
	MathLbr::vector<int> b{ {5,4,3} };
	MathLbr::vector<int> c{ {9,8,9} };
	MathLbr::vector<int> d{ {3,3,3,3} };
	int result = MathLbr::Vector::scalar_triple_product(a, b, c);
	EXPECT_EQ(result, -12);
	EXPECT_DEATH(MathLbr::Vector::scalar_triple_product(a, b, d), "Dynamic sizes of both lhs and rhs must be equal to perform certain operations!");

	MathLbr::vector<int> a2{ {1,2,3} };
	MathLbr::vector<float> b2{ {2.3,3.2,4.99} };
	MathLbr::vector<double> c2{ {3.3, 4.4, 5.5} };
	MathLbr::vector<std::complex<double>> ccc{ {std::complex{3., 4.}, std::complex{5.,5.}, std::complex{6.,6.}} };

	auto result2 = MathLbr::Vector::scalar_triple_product(a2, b2, c2);

	EXPECT_EQ(typeid(double), typeid(MathLbr::Vector::scalar_triple_product(a2, b2, c2)));

	// Test complex case
	MathLbr::vector<std::complex<double>> a3{ {std::complex{2., 3.}, std::complex{3., 3.4}, std::complex{5.5, 5.4}} };
	MathLbr::vector<std::complex<double>> b3{ {std::complex{1.2, 9.2}, std::complex{9.4, 5.4}, std::complex{2.5, 6.4}} };
	MathLbr::vector<std::complex<long double>> c3{ {std::complex{7.6, 1.}, std::complex{0.8, 9.8}, std::complex{2., 8.}} };
	std::complex<long double> result3 = MathLbr::Vector::scalar_triple_product<MathLbr::ComplexInnerProduct::ANTILINEAR_SECOND_ARGUMENT>(a3, b3, c3);
	// Note: to find correctResult3, we can use the property Determinant(A,B,C) == Scalar triple product(A,B,C)
	std::complex<long double> correctResult3{ -608.468, -434.496 };
	EXPECT_EQ(result3, correctResult3);
}


TEST(Modifiers, tripleVectorProduct) {
	MathLbr::vector<int> a{ {1,2,3} };
	MathLbr::vector<int> b{ {3,4,4} };
	MathLbr::vector<int> c{ {9,8,9} };
	MathLbr::vector<int> result = MathLbr::Vector::vector_triple_product(a, b, c);
	MathLbr::vector<int> correctResult{ {-61, -18, 77} };
	EXPECT_EQ(result, correctResult);
	MathLbr::vector<int> d{ {3,3,3,3} };
	EXPECT_DEATH(MathLbr::Vector::scalar_triple_product(a, b, d), "Dynamic sizes of both lhs and rhs must be equal to perform certain operations!");

	MathLbr::vector<int> a2{ {1,2,3} };
	MathLbr::vector<float> b2{ {2.3,3.2,4.99} };
	MathLbr::vector<double> c2{ {3.3, 4.4, 5.5} };
	auto result2 = MathLbr::Vector::vector_triple_product(a2, b2, c2);

	EXPECT_EQ(typeid(decltype(result2)::value_type), typeid(double{}));
}


TEST(SimilarityChecks, parallelism) {
	MathLbr::vector<int> a{ {4,6} };
	MathLbr::vector<int> b{ {10, 15} };
	EXPECT_EQ(true, MathLbr::Vector::are_parallel(a, b));

	MathLbr::vector<double, 3> aa{ {3.3, 4.4, 5.5} };
	MathLbr::vector<double, 3> bb{ {6.6, 8.8, 11} };
	EXPECT_EQ(true, MathLbr::Vector::are_parallel(aa, bb));

	// Check both sizes are equal (Dynamic)
	MathLbr::vector<int> a3{ {1,2} };
	MathLbr::vector<int> b3{ {1,2,3,4} };
	EXPECT_DEATH(MathLbr::Vector::are_parallel(a3, b3), "Dynamic sizes of both lhs and rhs must be equal to perform certain operations!");

	// Check implicit conversions correctness (3D case)
	MathLbr::vector<int> a5{ {1,2,3} };
	MathLbr::vector<float> b5{ {2,2,2} };
	auto result = MathLbr::Vector::are_parallel(a5, b5);

	// Check implicit conversions correctness (other dimensions)
	// Check implicit conversions correctness (3D case)
	MathLbr::vector<int> a6{ {1,2,3, 3} };
	MathLbr::vector<float> b6{ {2,2,2, 2} };
	auto result2 = MathLbr::Vector::are_parallel(a6, b6);

	/*MathLbr::vector<std::complex<double>> a6{{{1,2}, {3, 4}}};
	MathLbr::vector<std::complex<long double>> b6{ {{2,2}, {2, 0.3} } };
	auto result = MathLbr::Vector::are_parallel(a6, b6);*/
}


TEST(SimilarityChecks, perpendicularity) {
	MathLbr::vector<std::complex<double>> c{ { {0,1},{1,1},{1,0} } };
	MathLbr::vector<std::complex<long double>> d{ {{0,0},{1 / sqrt(3),0},{-1 / sqrt(3),1 / sqrt(3)}} };
	MathLbr::vector<std::complex<double>> e{ {{3 / (2 * sqrt(3)),0},{-1 / (2 * sqrt(3)), 1 / (2 * sqrt(3))},{0,1 / (2 * sqrt(3))}} };
	EXPECT_EQ(true, MathLbr::Vector::are_perpendicular(d, c));
	EXPECT_EQ(true, MathLbr::Vector::are_perpendicular(c, e));
	EXPECT_EQ(true, MathLbr::Vector::are_perpendicular(e, d));

	MathLbr::vector<int> a1{ {10,10,2} };
	MathLbr::vector<int> a2{ {2,-3,5} };
	EXPECT_EQ(true, MathLbr::Vector::are_perpendicular(a2, a1));
}


TEST(SimilarityChecks, coplanarity) {
	MathLbr::vector<double> first{ {1,1,1} };
	MathLbr::vector<double> second{ {1,3,1} };
	MathLbr::vector<double> third{ {2,2,2} };
	EXPECT_EQ(MathLbr::Vector::are_coplanar(first, second, third), true);
}


TEST(Modifiers, projections) {
	MathLbr::vector<double> a{ {2,4,3,5,6} };
	MathLbr::vector<double> b{ {5,6,7,10,0} };
	MathLbr::vector<double> res{ { 7. / 3, 14. / 3, 7. / 2, 35. / 6, 7.} };
	a.vector_projection_from(b);
	for (int idx = 0; double curr : res) {
		EXPECT_FLOAT_EQ(curr, a[idx++]);
	}

	// check division by 0 assert
	MathLbr::vector<double> a1{ {0,0,0} };
	MathLbr::vector<double> b1{ {3., 1, 0} };
	EXPECT_DEATH(a1.vector_projection_from(b1), "Cannot perform division by zero!");
}


TEST(NonModifiers, directionCosines) {
	MathLbr::vector<double> a{ {2., 3., 19.3} };
	EXPECT_NEAR(a.direction_cosine(MathLbr::Cos::X_AXIS), 0.10186, 1E-5);
}

TEST(NonModifiers, normalize) {
	MathLbr::vector<double> a{ {1.3,4.2,5.2,0.8} };
	MathLbr::vector<double> result{ {0.189604, 0.612568, 0.758417, 0.11668} };
	a.normalize();
	for (int idx = 0; double i : result) {
		EXPECT_NEAR(a[idx++], i, 1E-6);
	}

	MathLbr::vector<std::complex<double>> a2{ { {3.,4.}, {2.,-3.}, {0., 1.} } };
	a2.normalize();
	MathLbr::vector<std::complex<double>> result2{ {{3 / sqrt(39), 4 / sqrt(39)}, {2 / sqrt(39), -3 / sqrt(39)}, {0, 1 / sqrt(39)}} };
	for (int idx = 0; auto i : result2) {
		EXPECT_NEAR(a2[idx].real(), i.real(), 1E-6);
		EXPECT_NEAR(a2[idx++].imag(), i.imag(), 1E-6);
	}
}

TEST(NonModifiers, norms) {
	// L0
	MathLbr::vector<int> L0{ {1,0,3,0,2,0,4,5,0,0,3,0,0,4,2} };
	EXPECT_EQ(8, L0.norm<MathLbr::Norm::L0>());
	MathLbr::vector<std::complex<double>> L0_1{ {{0, 0}, {1,0}, {0,1}, {3,3}, {5,6}} };
	EXPECT_EQ(4, L0_1.norm<MathLbr::Norm::L0>());

	// L1
	MathLbr::vector<int> L1{ {1,2,3} };
	EXPECT_EQ(6, L1.norm<MathLbr::Norm::L1>());

	// L2
	EXPECT_EQ(sqrt(14), L1.norm<MathLbr::Norm::L2>());

	// L3
	EXPECT_EQ(std::pow(6, 2. / 3), L1.p_norm<3>());

	// LINF
	MathLbr::vector<int> LINF{ {-5, 4, 2} };
	EXPECT_EQ(-5, LINF.norm<MathLbr::Norm::LINF>());
}


TEST(NonModifiers, angleBetween) {
	MathLbr::vector<double> a{ {1.7, 26.} };
	MathLbr::vector<double> bb{ {5.5, 6.6} };
	EXPECT_NEAR(a.angle_between_degrees(bb), 36.065, 1E-3);
	EXPECT_NEAR(a.angle_between_radians(bb), 0.6294, 1E-3);

	MathLbr::vector<std::complex<double>> b{ {{2,-2}} };
	MathLbr::vector<std::complex<double>> c{ {{1, -sqrt(3)}} };
	EXPECT_NEAR(b.angle_between_degrees(c), 15, 1E-3);
}


TEST(NonModifiers, conversions_to) {
	// Polar coordinates
	MathLbr::vector<double> a{ {2, 3} };
	std::vector<double> aPolar = a.to_polar_coords();
	EXPECT_NEAR(aPolar[0], 3.6055512, 1E-3);
	EXPECT_NEAR(aPolar[1], 0.9827, 1E-3);

	// Spherical coordinates
	MathLbr::vector<double, 3> b{ {34, 4, 9} };
	std::array<double, 3> bSpherical = b.to_spherical_coords();
	EXPECT_NEAR(bSpherical[0], 35.3977, 1E-3);
	EXPECT_NEAR(bSpherical[1], 0.117108, 1E-3);
	EXPECT_NEAR(bSpherical[2], 1.31372, 1E-3);

	// Cylindrical coordinates
	MathLbr::vector<double> c{ {32.2, 2.9, -5} };
	std::vector cCylindrical = c.to_cylindrical_coords();
	EXPECT_NEAR(cCylindrical[0], 32.3303, 1E-4);
	EXPECT_NEAR(cCylindrical[1], 0.089819, 1E-4);
	EXPECT_EQ(cCylindrical[2], -5);
}

TEST(Modifiers, conversions_from) {
	MathLbr::vector<double, 2> a{};
	a.from_polar(5., 1.2);
	EXPECT_NEAR(a[0], 1.8117, 1E-4);
	EXPECT_NEAR(a[1], 4.66019, 1E-4);

	MathLbr::vector<double> b(3);
	b.from_spherical(5, 2, 3);
	EXPECT_NEAR(b[0], -0.293633, 1E-4);
	EXPECT_NEAR(b[1], 0.6416003, 1E-4);
	EXPECT_NEAR(b[2], -4.94996, 1E-4);

	MathLbr::vector<double> c(3);
	c.from_cylindrical(3., 2., 5.4);
	EXPECT_NEAR(c[0], -1.248440, 1E-4);
	EXPECT_NEAR(c[1], 2.727892, 1E-4);
	EXPECT_EQ(c[2], 5.4);
}

TEST(Modifiers, rotation) {
	MathLbr::vector<double> a{ {5,3,1} };
	a.rotate_around_x(5); // 5 == radiants
	EXPECT_EQ(a[0], 5);
	EXPECT_NEAR(a[1], 1.80991, 1E-4);
	EXPECT_NEAR(a[2], -2.59311, 1E-4);

	MathLbr::vector<double> b{ {9.4, 2, 5.5} };
	b.rotate_around_y(2.3);
	EXPECT_NEAR(b[0], -2.16162, 1E-4);
	EXPECT_EQ(b[1], 2);
	EXPECT_NEAR(b[2], -10.67415, 1E-4);

	MathLbr::vector<double> c{ {5,11, -3} };
	c.rotate_around_z(-3);
	EXPECT_NEAR(c[0], -3.39764, 1E-4);
	EXPECT_NEAR(c[1], -11.59552, 1E-4);
	EXPECT_EQ(c[2], -3);
}
