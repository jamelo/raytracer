#include <gtest/gtest.h>

#include <cmath>

#include <Vector.hpp>

using namespace geometry;

namespace
{

    const geo_type epsilon = 1e-10;

}

template <typename T, std::size_t Dimensions>
::testing::AssertionResult VectorNear(const Vector<T, Dimensions>& v1, const Vector<T, Dimensions>& v2, T epsilon) {
    bool is_near = std::equal(v1.begin(), v1.end(), v2.begin(), [=](auto c1, auto c2) {
        return std::abs(c1 - c2) < epsilon;
    });

    if (is_near) {
        return ::testing::AssertionSuccess();
    } else {
        return ::testing::AssertionFailure() << "Expected: " << v1 << ", actual: " << v2;
    }
}

TEST(VectorTest, ScalarOperations)
{
    EXPECT_TRUE(VectorNear(Vector3(1, 2, 3) * 2, Vector3(2, 4, 6), epsilon));
    EXPECT_TRUE(VectorNear(Vector3(2, 4, 6) / 2, Vector3(1, 2, 3), epsilon));
}

TEST(VectorTest, Abs)
{
    EXPECT_NEAR(abs(Vector3{0, 0, 0}), 0.0, epsilon);
    EXPECT_NEAR(abs(Vector3{1, 0, 0}), 1.0, epsilon);
    EXPECT_NEAR(abs(Vector3{0, 1, 0}), 1.0, epsilon);
    EXPECT_NEAR(abs(Vector3{0, 0, 1}), 1.0, epsilon);
    EXPECT_NEAR(abs(Vector3{1, 1, 0}), std::sqrt(2), epsilon);
    EXPECT_NEAR(abs(Vector3{0, 1, 1}), std::sqrt(2), epsilon);
    EXPECT_NEAR(abs(Vector3{1, 0, 1}), std::sqrt(2), epsilon);
    EXPECT_NEAR(abs(Vector3{1, 1, 1}), std::sqrt(3), epsilon);
}

TEST(VectorTest, Normalize)
{
    //ASSERT_EQ(normalize(Vector3{0, 0, 0}), (Vector3{std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()}));
    EXPECT_TRUE(VectorNear(normalize(Vector3{1, 0, 0}), (Vector3{1, 0, 0}), epsilon));
    EXPECT_TRUE(VectorNear(normalize(Vector3{0, 1, 0}), Vector3(0, 1, 0), epsilon));
    EXPECT_TRUE(VectorNear(normalize(Vector3{0, 0, 1}), Vector3(0, 0, 1), epsilon));
    EXPECT_TRUE(VectorNear(normalize(Vector3{1, 1, 0}), Vector3(1.0 / std::sqrt(2), 1.0 / std::sqrt(2), 0), epsilon));
    EXPECT_TRUE(VectorNear(normalize(Vector3{0, 1, 1}), Vector3(1.0, 1.0 / std::sqrt(2), 1.0 / std::sqrt(2)), epsilon));
    EXPECT_TRUE(VectorNear(normalize(Vector3{1, 0, 1}), Vector3(1.0 / std::sqrt(2), 0, 1.0 / std::sqrt(2)), epsilon));
    EXPECT_TRUE(VectorNear(normalize(Vector3{1, 1, 1}), Vector3(1.0 / std::sqrt(3), 1.0 / std::sqrt(3), 1.0 / std::sqrt(3)), epsilon));

    //EXPECT_EQ(abs(normalize(Vector3{0, 0, 0})), 0.0);
    EXPECT_NEAR(abs(normalize(Vector3{1, 0, 0})), 1.0, epsilon);
    EXPECT_NEAR(abs(normalize(Vector3{0, 1, 0})), 1.0, epsilon);
    EXPECT_NEAR(abs(normalize(Vector3{0, 0, 1})), 1.0, epsilon);
    EXPECT_NEAR(abs(normalize(Vector3{1, 1, 0})), 1.0, epsilon);
    EXPECT_NEAR(abs(normalize(Vector3{0, 1, 1})), 1.0, epsilon);
    EXPECT_NEAR(abs(normalize(Vector3{1, 0, 1})), 1.0, epsilon);
    EXPECT_NEAR(abs(normalize(Vector3{1, 1, 1})), 1.0, epsilon);
}
