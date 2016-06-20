#ifndef SHAPES_RECTANGLE_HPP
#define SHAPES_RECTANGLE_HPP

#include <shapes/Shape.hpp>

namespace shapes
{

    class Rectangle : public Shape
    {
    private:
        Point3 m_p0;
        Point3 m_p1;
        Point3 m_p2;
        Vector3 m_normal;
        Vector3 m_v0;
        Vector3 m_v1;
        float m_v0_v1;
        float m_v1_v1;
        float m_v0_v0;
        float m_recipDenominator;

    public:
        Rectangle(const Point3& p0, const Point3& p1, const Point3& p2, const SurfaceDescription& surface) :
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
        { }

        virtual IntersectionInfo calculateRayIntersection(const Ray3& ray) const {
            Vector3 diff = m_p0 - ray.origin();
            double distance = (m_normal * diff) / (m_normal * ray.direction());
            Point3 poi = ray.origin() + distance * ray.direction();

            Vector3 v2 = poi - m_p0;

            float u = (m_v1_v1 * (v2 * m_v0) - m_v0_v1 * (v2 * m_v1)) * m_recipDenominator;
            float v = (m_v0_v0 * (v2 * m_v1) - m_v0_v1 * (v2 * m_v0)) * m_recipDenominator;

            return (u >= 0 && v >= 0 && u <= 1 && v <= 1) ? IntersectionInfo(distance, this) : IntersectionInfo();
        }

        virtual Vector3 calculateNormal(const Point3&) const {
            return m_normal;
        }

        virtual Point2 textureMap(const Point3& p) const {
            float dummy;
            float x = std::modf((p - m_p0) * (m_p1 - m_p0), &dummy);
            float y = std::modf((p - m_p0) * (m_p2 - m_p0), &dummy);

            x = (x >= 0 ? x : x + 1.0);
            y = (y >= 0 ? y : y + 1.0);

            return Point2{x, y};
        }
    };

}

#endif
