#ifndef SHAPES_RECTANGLE_HPP
#define SHAPES_RECTANGLE_HPP

#include <shapes/Shape.hpp>
#include <builders/CustomShapeBuilder.hpp>

namespace shapes
{

    class Rectangle : public Shape
    {
    private:
        geometry::Point3 m_p0;
        geometry::Point3 m_p1;
        geometry::Point3 m_p2;
        geometry::Vector3 m_normal;
        geometry::Vector3 m_v0;
        geometry::Vector3 m_v1;
        float m_v0_v1;
        float m_v1_v1;
        float m_v0_v0;
        float m_recipDenominator;

    public:
        Rectangle(const geometry::Point3& p0, const geometry::Point3& p1, const geometry::Point3& p2,
                const std::shared_ptr<Surface>& surface) :
            Shape(surface),
            m_p0(p0),
            m_p1(p1),
            m_p2(p2),
            m_normal(normalize(cross_product(p1 - p0, p2 - p0))),
            m_v0(m_p2 - m_p0),
            m_v1(m_p1 - m_p0),
            m_v0_v1(m_v0 * m_v1),
            m_v1_v1(m_v1 * m_v1),
            m_v0_v0(m_v0 * m_v0),
            m_recipDenominator(1.0 / (m_v0_v0 * m_v1_v1 - m_v0_v1 * m_v0_v1))
        {

        }

        virtual IntersectionResult calculateRayIntersection(const geometry::Ray3& ray) const override
        {
            geometry::Vector3 diff = m_p0 - ray.origin();
            double distance = (m_normal * diff) / (m_normal * ray.direction());
            geometry::Point3 poi = ray.origin() + distance * ray.direction();

            geometry::Vector3 v2 = poi - m_p0;

            double v2v0 = v2 * m_v0;
            double v2v1 = v2 * m_v1;

            float u = (m_v1_v1 * v2v0 - m_v0_v1 * v2v1) * m_recipDenominator;

            if (u >= 0 && u <= 1)
            {
                float v = (m_v0_v0 * v2v1 - m_v0_v1 * v2v0) * m_recipDenominator;

                if (v >= 0 && v <= 1)
                {
                    return IntersectionResult(distance, this);
                }
            }

            return IntersectionResult();
        }

        virtual geometry::Vector3 calculateNormal(const geometry::Point3&) const override
        {
            return m_normal;
        }

        virtual geometry::Point2 textureMap(const geometry::Point3& p) const override
        {
            float dummy;
            float x = std::modf((p - m_p0) * (m_p1 - m_p0), &dummy);
            float y = std::modf((p - m_p0) * (m_p2 - m_p0), &dummy);

            x = (x >= 0 ? x : x + 1.0);
            y = (y >= 0 ? y : y + 1.0);

            return geometry::Point2{x, y};
        }

        virtual geometry::Point3 sampleSurface() const override
        {
            thread_local std::uniform_real_distribution<double> dist(1.0);
            return dist(RandomGenerator::get_instance()) * m_v0 + dist(RandomGenerator::get_instance()) * m_v1 + m_p0;
        }
    };

    class RectangleBuilder : public builders::CustomShapeBuilder
    {
    public:
        RectangleBuilder()
        {
            using namespace builders;

            parameter("point1", ParamType::ePoint3, REQUIRED);
            parameter("point2", ParamType::ePoint3, REQUIRED);
            parameter("point3", ParamType::ePoint3, REQUIRED);
            parameter("surface", ParamType::eSurface, REQUIRED);
        }

    private:
        static builders::ShapeBuilder::Registration sm_registration;

        virtual std::shared_ptr<Shape> construct(const builders::BuilderArgs& args)
        {
            Point3 point1 = args.get<Point3>("point1");
            Point3 point2 = args.get<Point3>("point2");
            Point3 point3 = args.get<Point3>("point3");
            const auto& surface = args.get<std::shared_ptr<Surface>>("surface");

            return std::make_shared<Rectangle>(point1, point2, point3, surface);
        }
    };

    builders::ShapeBuilder::Registration RectangleBuilder::sm_registration("rectangle", std::make_unique<RectangleBuilder>());
}

#endif
