#include "gtest/gtest.h"
#include "../LinearAlgebraLatest/vector.h"

TEST(Constructors, Contructor1) {
  MathLbr::vector<int, 3> vec(4);
  for (int i = 0; i < 3; ++i) {
	  EXPECT_EQ(vec[i], 4);
  }
  EXPECT_EQ(3ULL, vec.size());
}

TEST(Constructors, Constructor2) {
	MathLbr::vector<double> vec(4);
	for (int i = 0; i < 4; ++i) {
		EXPECT_EQ(vec[i], double());
	}
	EXPECT_EQ(4ULL, vec.size());
}

TEST(Constructors, Constructor3) {
	int arr[5] = { 1, 2, 3, 4, 5 };

	// First case: internal container is std::array
	auto lambda = [&] {MathLbr::vector<std::complex<int>, 4>(std::begin(arr), std::end(arr)); };
	// Needs to assert: actual size [4] < passed size [5]
	EXPECT_DEATH(lambda(), "");
	MathLbr::vector<std::complex<int>, 7> myVec(std::begin(arr), std::end(arr));
	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(myVec[i], arr[i]);
	}
	EXPECT_EQ(myVec[5], int());
	EXPECT_EQ(myVec[6], int());
	EXPECT_EQ(myVec.size(), 7);

	// Second case: internal container is std::vector
	MathLbr::vector<int> myVec2(std::begin(arr), std::end(arr));
	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(myVec2[i], arr[i]);
	}
	EXPECT_EQ(myVec2.size(), 5);
}

TEST(Constructors, Constructor4) {
	//auto lambda = [] { MathLbr::vector<int, 5>{ {1, 2, 3, 4, 5} }; };
	//EXPECT_DEATH(lambda(), ""); -> Useless: compile-time error
	MathLbr::vector<int, 3> myArrayVector{ {1, 2, 3}};
	EXPECT_EQ(myArrayVector.size(), 3);
	double first = 1;
	for (int i = 0; i < myArrayVector.size(); ++i) {
		EXPECT_EQ(myArrayVector[i], first);
		first += 1;
	}

	MathLbr::vector<std::complex<int>> myVectorVector{{{1,3}, {2,4}, {3,5}}};
	EXPECT_EQ(myVectorVector.size(), 3);
	std::complex<int> curr = { 1, 3 };
	for (int i = 0; i < myVectorVector.size(); ++i) {
		EXPECT_EQ(myVectorVector[i], curr);
		curr = { curr.real() + 1, curr.imag() + 1 };
	}
}

TEST(Constructors, Constructor5) {
	MathLbr::vector<int, 4> a{ {1,2,3,4} };
	auto lambda = [&a] { MathLbr::vector<int, 3> b{ a }; };
	EXPECT_DEATH(lambda(), ""); // new size is smaller than old size

	MathLbr::vector<int, 5> c{ a };
	for (int i = 0; i < 4; ++i) {
		EXPECT_EQ(c[i], a[i]);
	}
	EXPECT_EQ(c[4], int());
	EXPECT_EQ(c.size(), 5);

	MathLbr::vector<double> d{ a };
	EXPECT_EQ(d.size(), 4);
	for (int i = 0; i < 4; ++i) {
		EXPECT_EQ(d[i], a[i]);
	}

	auto lambda2 = [&d] {MathLbr::vector<double, 3> e{ d }; };
	EXPECT_DEATH(lambda2(), "");
}

TEST(OperatorOverloading, Sum) {
	// Non dynamic
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
	MathLbr::vector<std::complex<double>> complex{ {{1.3,3},{2,2.3}} };
	MathLbr::vector<std::complex<double>, 4> complex2{ {{1.3,3},{2,2.3},{3,3},{4,4}} };
	MathLbr::vector<std::complex<double>, 4> expected1{ {{2.6,6},{4,4.6},{3,3},{4,4}} };
	MathLbr::vector<std::complex<double>> expected2{ {{2.6,6},{4,4.6}} };
	EXPECT_EQ(complex2 + complex, expected1);
	EXPECT_EQ(complex + complex2, expected2);
	
}

TEST(OperatorOverloading, Subtraction) {
	// Non dynamic
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
	MathLbr::vector<int> a{ {3, 4, 1} };
	MathLbr::vector<int> b{ {1, 9, 10} };
	MathLbr::vector<int> expected1{ {31, -29, 23} };
}

TEST(Modifiers, InnerProduct) {
	// Non complex case
	MathLbr::vector<double> a{ {3, 4.9, 1} };
	MathLbr::vector<double> b{ {5.6, 3.3, 2} };
	EXPECT_EQ(34.97, MathLbr::Vector::inner_product(a, b));

	// Complex case
	MathLbr::vector<std::complex<int>> c{ {{2,3}, {7,3}} };
	MathLbr::vector<std::complex<int>> d{ {{1,2},{2,2}} };

	std::complex<int> cmp{ 28,9 };
	EXPECT_EQ(cmp, MathLbr::Vector::inner_product<MathLbr::ComplexInnerProduct::ANTILINEAR_FIRST_ARGUMENT>(c, d));
}

TEST(Modifiers, complexDotProduct) {
	EXPECT_EQ(MathLbr::Vector::complex_inner_product(std::complex<double>(3, -4), std::complex<double>(-4, 3)), -24);
}

TEST(Modifiers, tripleScalarProduct) {
	MathLbr::vector<int> a{ {1,2,3} };
	MathLbr::vector<int> b{ {5,4,3} };
	MathLbr::vector<int> c{ {9,8,9} };
	MathLbr::vector<int> d{ {3,3,3,3} };
	int result = MathLbr::Vector::scalar_triple_product(a, b, c);
	EXPECT_EQ(result, -12);
	EXPECT_DEATH(MathLbr::Vector::scalar_triple_product(a, b, d), "");
}

TEST(Modifiers, tripleVectorProduct) {
	MathLbr::vector<int> a{ {1,2,3} };
	MathLbr::vector<int> b{ {3,4,4} };
	MathLbr::vector<int> c{ {9,8,9} };
	MathLbr::vector<int> result = MathLbr::Vector::vector_triple_product(a, b, c);
	MathLbr::vector<int> correctResult{ {-61, -18, 77} };
	EXPECT_EQ(result, correctResult);
	MathLbr::vector<int> d{ {3,3,3,3} };
	EXPECT_DEATH(MathLbr::Vector::scalar_triple_product(a, b, d), "");
}

TEST(SimilarityChecks, parallelism) {
	MathLbr::vector<int> a{ {4,6} };
	MathLbr::vector<int> b{ {10, 15} };
	EXPECT_EQ(true, MathLbr::Vector::are_parallel(a, b));

	MathLbr::vector<double, 3> aa{ {3.3, 4.4, 5.5} };
	MathLbr::vector<double, 3> bb{ {6.6, 8.8, 11} };
	EXPECT_EQ(true, MathLbr::Vector::are_parallel(aa, bb));


}

TEST(SimilarityChecks, perpendicularity) {
	MathLbr::vector<std::complex<double>> c{ { {0,1},{1,1},{1,0} } };
	MathLbr::vector<std::complex<double>> d{ {{0,0},{1 / sqrt(3),0},{-1 / sqrt(3),1 / sqrt(3)}} };
	MathLbr::vector<std::complex<double>> e{ {{3 / (2 * sqrt(3)),0},{-1 / (2 * sqrt(3)), 1 / (2 * sqrt(3))},{0,1 / (2 * sqrt(3))}} };
	EXPECT_EQ(1, MathLbr::Vector::are_perpendicular(d, c));
	EXPECT_EQ(1, MathLbr::Vector::are_perpendicular(c, e));
	EXPECT_EQ(1, MathLbr::Vector::are_perpendicular(e, d));
}

TEST (SimilarityChecks, coplanarity) {
	MathLbr::vector<double> first{ {1,1,1} };
	MathLbr::vector<double> second{ {1,3,1} };
	MathLbr::vector<double> third{ {2,2,2} };
	EXPECT_EQ(MathLbr::Vector::are_coplanar(first, second, third), true);
}

TEST(Modifiers, projections) {
	MathLbr::vector<double> a{ {2,4,3,5,6} };
	MathLbr::vector<double> b{ {5,6,7,10,0} };
	MathLbr::vector<double> res{ { 7 / 3, 14 / 3, 7 / 2, 35 / 6, 7} };
}

TEST(NonModifiers, directionCosines) {
	MathLbr::vector<double> a{ {2, 3, 19.3} };
	EXPECT_NEAR(a.direction_cosine(MathLbr::Cos::X_AXIS), 0.10186, 1E-4);
}

TEST(NonModifiers, magnitude) {
	MathLbr::vector<int, 4> a{ {1,4,19,-4} };
	EXPECT_NEAR(a.magnitude(), 19.8494, 1E-4);
}

TEST(NonModifiers, angleBetween) {
	MathLbr::vector<double> a{ {1.7, 26} };
	MathLbr::vector<double> bb{ {5.5, 6.6} };
	EXPECT_NEAR(a.angle_between_degrees(bb), 36.065, 1E-3);
	EXPECT_NEAR(a.angle_between_radiants(bb), 0.6294, 1E-3);

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
	a.from_polar(5, 1.2);
	EXPECT_NEAR(a[0], 1.8117, 1E-4);
	EXPECT_NEAR(a[1], 4.66019, 1E-4);

	MathLbr::vector<double> b(3);
	b.from_spherical(5, 2, 3);
	EXPECT_NEAR(b[0], -0.293633, 1E-4);
	EXPECT_NEAR(b[1], 0.6416003, 1E-4);
	EXPECT_NEAR(b[2], -4.94996, 1E-4);

	MathLbr::vector<double> c(3);
	c.from_cylindrical(3, 2, 5.4);
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
