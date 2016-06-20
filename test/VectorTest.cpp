#include <gtest/gtest.h>

#include <cmath>

#include <geometry/Vector.hpp>

using namespace geometry;

namespace
{
    const geo_type epsilon = 1e-10;
}

template <typename T, std::size_t Dimensions>
::testing::AssertionResult VectorIsNear(const Vector<T, Dimensions>& v1, const Vector<T, Dimensions>& v2, T epsilon) {
    bool is_near = std::equal(v1.begin(), v1.end(), v2.begin(), [=](auto c1, auto c2) {
        return std::abs(c1 - c2) < epsilon;
    });

    if (is_near) {
        return ::testing::AssertionSuccess();
    } else {
        return ::testing::AssertionFailure() << "Expected: " << v1 << ", actual: " << v2;
    }
}

#define VECTOR_EXPECT_NEAR(v1, v2, eps) EXPECT_TRUE(VectorIsNear(v1, v2, eps))

TEST(VectorTest, Addition)
{
    VECTOR_EXPECT_NEAR(Vector3(1, 2, 3) + Vector3(6, 5, 4), Vector3(7, 7, 7), epsilon);
}

TEST(VectorTest, Subtraction)
{
    VECTOR_EXPECT_NEAR(Vector3(10, 11, 12) - Vector3(3, 4, 5), Vector3(7, 7, 7), epsilon);
}

TEST(VectorTest, ScalarMultiplication)
{
    VECTOR_EXPECT_NEAR(Vector3(1, 2, 3) * 10, Vector3(10, 20, 30), epsilon);
    VECTOR_EXPECT_NEAR(10 * Vector3(1, 2, 3), Vector3(10, 20, 30), epsilon);
}

TEST(VectorTest, ScalarDivision)
{
    VECTOR_EXPECT_NEAR(Vector3(10, 20, 30) / 10, Vector3(1, 2, 3), epsilon);
}

TEST(VectorTest, UnaryPlus)
{
    VECTOR_EXPECT_NEAR(+Vector3(10, 20, 30), Vector3(10, 20, 30), epsilon);
}

TEST(VectorTest, UnaryMinus)
{
    VECTOR_EXPECT_NEAR(-Vector3(10, 20, 30), Vector3(-10, -20, -30), epsilon);
}

TEST(VectorTest, Assignment)
{
    Vector3 v1, v2, v3, v4, v5;
    v1 = v2 = v3 = v4 = v5 = Vector3(1, 1, 1);

    v2 += Vector3(0, 1, 2);
    v3 -= Vector3(2, 3, 4);
    v4 *= 5;
    v5 /= 2;

    VECTOR_EXPECT_NEAR(v1, Vector3(1, 1, 1), epsilon);
    VECTOR_EXPECT_NEAR(v2, Vector3(1, 2, 3), epsilon);
    VECTOR_EXPECT_NEAR(v3, Vector3(-1, -2, -3), epsilon);
    VECTOR_EXPECT_NEAR(v4, Vector3(5, 5, 5), epsilon);
    VECTOR_EXPECT_NEAR(v5, Vector3(0.5, 0.5, 0.5), epsilon);
}

TEST(VectorTest, DotProduct)
{
    EXPECT_NEAR(Vector3(1, 2, 3) * Vector3(-4, 5, 0), 6, epsilon);
}

TEST(VectorTest, Abs)
{
    EXPECT_NEAR(abs(Vector3(1, 2, 3)), std::sqrt(14), epsilon);
}

TEST(VectorTest, Normalize)
{
    VECTOR_EXPECT_NEAR(normalize(Vector3(1, 2, 3)), Vector3(1 / sqrt(14), 2 / sqrt(14), 3 / sqrt(14)), epsilon);
}
