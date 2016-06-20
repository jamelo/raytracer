#ifndef SHAPES_PLANE_HPP
#define SHAPES_PLANE_HPP

#include <shapes/Shape.hpp>

namespace shapes
{

    class Plane : public Shape
    {
    private:
        Point3 m_origin;
        Vector3 m_direction1;
        Vector3 m_direction2;
        Vector3 m_normal;

    public:
        Plane(const Point3& _origin, const Vector3& _dir1, const Vector3& _dir2, const SurfaceDescription& _surface) :
        Shape(_surface),
        m_origin(_origin),
        m_direction1(normalize(_dir1)),
        m_direction2(normalize(_dir2)),
        m_normal(normalize(cross_product(_dir1, _dir2)))
        {
            //TODO: validate inputs, throw exception
        }

        virtual IntersectionInfo calculateRayIntersection(const Ray3& ray) const {
            Vector3 diff = m_origin - ray.origin();
            return IntersectionInfo((m_normal * (diff)) / (m_normal * ray.direction()), this);
        }

        virtual Vector3 calculateNormal(const Point3&) const {
            return m_normal;
        }

        virtual Point2 textureMap(const Point3& p) const {
            float dummy;
            float x = std::modf((p - m_origin) * m_direction1, &dummy);
            float y = std::modf((p - m_origin) * m_direction2, &dummy);

            x = (x >= 0 ? x : x + 1.0);
            y = (y >= 0 ? y : y + 1.0);

            return Point2{x, y};
        }
    };

}

#endif
