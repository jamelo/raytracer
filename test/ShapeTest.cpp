#include <gtest/gtest.h>

#include <geometry/Ray.hpp>
#include <geometry/Vector.hpp>
//#include <Shapes.hpp>

using namespace geometry;

namespace
{
    const geo_type epsilon = 1e-10;
}

TEST(ShapeTest, Plane)
{
    /*shapes::Plane p({0, 0, 0}, {1, 0, 0}, {0, 1, 0}, std::make_shared<Surface>(Surface({0, 0, 0}, 0)));

    Ray3 r1(Point3{0, 0, 1}, Vector3{0, 0, -1});
    Ray3 r2(Point3{0, 0, 1}, Vector3{1, 0, -1});
    Ray3 r3(Point3{0, 0, 1}, normalize(Vector3{1, 0, -1}));

    ASSERT_NEAR(p.calculateRayIntersection(r1).distance(), 1.0, 1e-10);
    ASSERT_NEAR(p.calculateRayIntersection(r2).distance(), 1.0, 1e-10);
    ASSERT_NEAR(p.calculateRayIntersection(r3).distance(), sqrt(2), 1e-10);*/
}

TEST(ShapeTest, Sphere)
{

}
