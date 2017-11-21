#ifndef SHAPES_BOX_HPP
#define SHAPES_BOX_HPP

#include <builders/CustomShapeBuilder.hpp>
#include <shapes/Shape.hpp>
#include <shapes/Rectangle.hpp>

#include <iostream>

static constexpr double pi() { return std::atan(1.0) * 4.0; }

namespace shapes
{

    class Box : public Shape
    {
    private:
        std::array<Rectangle, 6> m_sides;

    public:
        Box(const geometry::Vector3& size, const geometry::Point3& location, const geometry::Vector3& orientation, const std::shared_ptr<Surface>& surface) :
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
            geometry::Vector3 s = size * 0.5;
            geometry::Vector3 origin = static_cast<geometry::Vector<double, 3ul>>(location);

            auto rotationTransform = geometry::rotation(orientation[0] * 2 * pi(), orientation[1] * 2 * pi(), orientation[2] * 2 * pi());

            std::array<geometry::Point3, 8> points{
                rotationTransform * geometry::Point3{-s[0],  s[1], -s[2]} + origin,
                rotationTransform * geometry::Point3{ s[0],  s[1], -s[2]} + origin,
                rotationTransform * geometry::Point3{ s[0],  s[1],  s[2]} + origin,
                rotationTransform * geometry::Point3{-s[0],  s[1],  s[2]} + origin,
                rotationTransform * geometry::Point3{-s[0], -s[1], -s[2]} + origin,
                rotationTransform * geometry::Point3{ s[0], -s[1], -s[2]} + origin,
                rotationTransform * geometry::Point3{ s[0], -s[1],  s[2]} + origin,
                rotationTransform * geometry::Point3{-s[0], -s[1],  s[2]} + origin
            };

            m_sides[0] = Rectangle(points[0], points[1], points[3], surface);
            m_sides[1] = Rectangle(points[4], points[5], points[0], surface);
            m_sides[2] = Rectangle(points[5], points[6], points[1], surface);
            m_sides[3] = Rectangle(points[6], points[7], points[2], surface);
            m_sides[4] = Rectangle(points[7], points[4], points[3], surface);
            m_sides[5] = Rectangle(points[7], points[4], points[6], surface);
        }

        virtual IntersectionResult calculateRayIntersection(const geometry::Ray3& ray) const
        {
            IntersectionResult nearestIntersection;

            for (const auto& side : m_sides)
            {
                IntersectionResult i = side.calculateRayIntersection(ray);

                if (i.distance() < nearestIntersection.distance())
                {
                    nearestIntersection = i;
                }
            }

            return nearestIntersection;
        }

        virtual geometry::Vector3 calculateNormal(const geometry::Point3&) const
        {
            return geometry::Vector3{0, 0, 0};
        }

        virtual geometry::Point2 textureMap(const geometry::Point3& p) const
        {
            (void)p;
            return geometry::Point2{0, 0};
        }
    };

    class BoxBuilder : public builders::CustomShapeBuilder
    {
    public:
        BoxBuilder()
        {
            using namespace builders;

            parameter("location", ParamType::ePoint3, REQUIRED);
            parameter("dimensions", ParamType::eVector3, REQUIRED);
            parameter("orientation", ParamType::eVector3, OPTIONAL, Vector3(0, 0, 0));
            parameter("surface", ParamType::eSurface, REQUIRED);
        }

    private:
        static builders::ShapeBuilder::Registration sm_registration;

        virtual std::shared_ptr<Shape> construct(const builders::BuilderArgs& args)
        {
            Point3 location = args.get<Point3>("location");
            Vector3 dimensions = args.get<Vector3>("dimensions");
            Vector3 orientation = args.get<Vector3>("orientation");
            const auto& surface = args.get<std::shared_ptr<Surface>>("surface");

            return std::make_shared<Box>(dimensions, location, orientation, surface);
        }
    };

    builders::ShapeBuilder::Registration BoxBuilder::sm_registration("box", std::make_unique<BoxBuilder>());

}

#endif
