#ifndef SHAPES_BOX_HPP
#define SHAPES_BOX_HPP

#include <shapes/Shape.hpp>
#include <shapes/Rectangle.hpp>

namespace shapes
{

    class Box : public Shape
    {
    private:
        std::array<Rectangle, 6> m_sides;

    public:
        Box(const Vector3& size, const Point3& location, const Vector3& orientation, const SurfaceDescription& surface) :
            Shape(surface),
            m_sides{
                Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
                Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
                Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
                Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
                Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
                Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface)
            }
        {
            Vector3 s = size * 0.5;
            Vector3 origin = static_cast<Vector<double, 3ul>>(location);

            auto rotationTransform = rotation(orientation[0], orientation[1], orientation[2]);

            std::array<Point3, 8> points{
                rotationTransform * Point3{-s[0],  s[1], -s[2]} + origin,
                rotationTransform * Point3{ s[0],  s[1], -s[2]} + origin,
                rotationTransform * Point3{ s[0],  s[1],  s[2]} + origin,
                rotationTransform * Point3{-s[0],  s[1],  s[2]} + origin,
                rotationTransform * Point3{-s[0], -s[1], -s[2]} + origin,
                rotationTransform * Point3{ s[0], -s[1], -s[2]} + origin,
                rotationTransform * Point3{ s[0], -s[1],  s[2]} + origin,
                rotationTransform * Point3{-s[0], -s[1],  s[2]} + origin
            };

            m_sides[0] = Rectangle(points[0], points[1], points[3], surface);
            m_sides[1] = Rectangle(points[4], points[5], points[0], surface);
            m_sides[2] = Rectangle(points[5], points[6], points[1], surface);
            m_sides[3] = Rectangle(points[6], points[7], points[2], surface);
            m_sides[4] = Rectangle(points[7], points[4], points[3], surface);
            m_sides[5] = Rectangle(points[7], points[4], points[6], surface);
        }

        virtual IntersectionInfo calculateRayIntersection(const Ray3& ray) const {
            IntersectionInfo nearestIntersection;

            for (const auto& side : m_sides) {
                IntersectionInfo i = side.calculateRayIntersection(ray);

                if (i.distance() < nearestIntersection.distance()) {
                    nearestIntersection = i;
                }
            }

            return nearestIntersection;
        }

        virtual Vector3 calculateNormal(const Point3&) const {
            return Vector3{0, 0, 0};
        }

        virtual Point2 textureMap(const Point3& p) const {
            return Point2{0, 0};
        }
    };

}

#endif
